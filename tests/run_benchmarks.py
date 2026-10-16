#!/usr/bin/env python3
"""Compile legacy benchmarks in isolation and report trustworthy outcomes.

Every source first goes through the selected compiler without the plugin.  Only
translation units that pass that baseline are compiled again with the plugin.
This makes BUILD_FAIL (the fixture does not compile) distinguishable from
PLUGIN_CRASH (the same fixture compiles until the plugin is loaded).
"""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import html
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import statistics
import subprocess
import sys
import time
from typing import Any, Sequence

try:
    from .benchmark_manifest import (
        BenchmarkCase,
        EXTRACTED_FAMILIES,
        JULIET_CWES,
        REPOSITORY_ROOT,
        discover_cases,
    )
except ImportError:  # Direct execution: python3 tests/run_benchmarks.py
    from benchmark_manifest import (
        BenchmarkCase,
        EXTRACTED_FAMILIES,
        JULIET_CWES,
        REPOSITORY_ROOT,
        discover_cases,
    )


FINDING_RE = re.compile(r"\[(SA\d{3})\]")
CRASH_RE = re.compile(
    r"internal compiler error|segmentation fault|please submit a full bug report|"
    r"cannot load plugin|failed to initialize plugin|plugin .* (?:error|failed)",
    re.IGNORECASE,
)
ERROR_LINE_RE = re.compile(
    r"^.*(?:fatal error:|error:|internal compiler error:|segmentation fault).*$",
    re.IGNORECASE | re.MULTILINE,
)

COMMON_FLAGS = (
    "-fdiagnostics-color=never",
    "-O1",
    "-fipa-pta",
    "-fno-inline",
    "-fno-tree-dce",
    "-fno-tree-dse",
    "-fno-tree-fre",
    "-pthread",
)

DEFAULT_PLUGIN_ARGS = (
    "report=0",
    "memoryleakcheck=1",
    "useafterfree=1",
    "threadmod=1",
)

EXTRACTED_TARGET_CODE = {
    "leak": "SA001",
    "double_free": "SA002",
    "use_after_free": "SA003",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Isolated build/robustness runner for legacy benchmarks"
    )
    parser.add_argument(
        "--suite",
        action="append",
        choices=("extracted", "juliet", "all"),
        help="suite to run; repeatable (default: extracted)",
    )
    parser.add_argument(
        "--family",
        action="append",
        choices=EXTRACTED_FAMILIES,
        help="limit extracted cases to a family; repeatable",
    )
    parser.add_argument(
        "--cwe",
        action="append",
        choices=JULIET_CWES,
        help="limit Juliet cases to a CWE; repeatable",
    )
    parser.add_argument(
        "--match",
        help="only run cases whose repository-relative path contains this text",
    )
    parser.add_argument(
        "--limit",
        type=int,
        help="run at most this many cases after filtering (useful for smoke tests)",
    )
    parser.add_argument("--cc", default=os.environ.get("CC", "gcc"))
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"))
    parser.add_argument(
        "--plugin",
        type=Path,
        help="plugin shared object; omit for a baseline-only corpus build audit",
    )
    parser.add_argument(
        "--plugin-arg",
        action="append",
        default=[],
        metavar="KEY=VALUE",
        help="override/add a plugin argument; repeatable",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=REPOSITORY_ROOT / "artifacts" / "benchmarks",
    )
    parser.add_argument("--jobs", type=int, default=min(8, os.cpu_count() or 1))
    parser.add_argument("--timeout", type=float, default=60.0)
    parser.add_argument(
        "--keep-objects",
        action="store_true",
        help="retain successful baseline/plugin object files",
    )
    parser.add_argument("--verbose", action="store_true")
    return parser.parse_args()


def validate_args(args: argparse.Namespace) -> None:
    if args.jobs < 1:
        raise ValueError("--jobs must be at least 1")
    if args.timeout <= 0:
        raise ValueError("--timeout must be positive")
    if args.limit is not None and args.limit < 1:
        raise ValueError("--limit must be at least 1")

    for compiler in (args.cc, args.cxx):
        if shutil.which(compiler) is None:
            raise ValueError(f"compiler not found: {compiler}")

    if args.plugin is not None:
        args.plugin = args.plugin.resolve()
        if not args.plugin.is_file():
            raise ValueError(f"plugin does not exist: {args.plugin}")

    for item in args.plugin_arg:
        if "=" not in item or not item.split("=", 1)[0]:
            raise ValueError(f"invalid --plugin-arg {item!r}; expected KEY=VALUE")


def normalized_plugin_args(overrides: Sequence[str]) -> list[str]:
    values = dict(item.split("=", 1) for item in DEFAULT_PLUGIN_ARGS)
    for item in overrides:
        key, value = item.split("=", 1)
        values[key] = value
    return [f"{key}={value}" for key, value in sorted(values.items())]


def case_directory(output: Path, case: BenchmarkCase) -> Path:
    digest = hashlib.sha256(case.case_id.encode("utf-8")).hexdigest()[:12]
    readable = re.sub(r"[^A-Za-z0-9_.-]+", "_", case.source.stem)[:72]
    return output / "cases" / case.suite / case.group / f"{readable}-{digest}"


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def snapshot_plugin(args: argparse.Namespace) -> None:
    """Freeze the plugin so a concurrent rebuild cannot mix test versions."""

    if args.plugin is None:
        args.plugin_original = None
        args.plugin_sha256 = None
        return

    args.plugin_original = args.plugin
    args.plugin_sha256 = sha256_file(args.plugin)
    snapshot_dir = args.output / "plugin-snapshot" / args.plugin_sha256[:12]
    snapshot_dir.mkdir(parents=True, exist_ok=True)
    snapshot = snapshot_dir / args.plugin.name
    if not snapshot.exists() or sha256_file(snapshot) != args.plugin_sha256:
        shutil.copy2(args.plugin, snapshot)
    args.plugin = snapshot.resolve()


def compile_command(
    args: argparse.Namespace,
    case: BenchmarkCase,
    object_file: Path,
    with_plugin: bool,
) -> list[str]:
    compiler = args.cc if case.language == "c" else args.cxx
    standard = "-std=gnu11" if case.language == "c" else "-std=gnu++14"
    command = [compiler, standard, *COMMON_FLAGS]
    for include_dir in case.include_dirs:
        command.extend(("-I", str(include_dir.resolve())))

    if with_plugin:
        assert args.plugin is not None
        plugin_name = args.plugin.stem
        command.append(f"-fplugin={args.plugin}")
        for item in normalized_plugin_args(args.plugin_arg):
            command.append(f"-fplugin-arg-{plugin_name}-{item}")

    command.extend(
        ("-c", str(case.source.resolve()), "-o", str(object_file.resolve()))
    )
    return command


def run_command(
    command: Sequence[str], cwd: Path, timeout: float
) -> tuple[int | None, str, str, float, bool]:
    started = time.monotonic()
    try:
        completed = subprocess.run(
            command,
            cwd=cwd,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout,
            check=False,
        )
        return (
            completed.returncode,
            completed.stdout,
            completed.stderr,
            round((time.monotonic() - started) * 1000, 2),
            False,
        )
    except subprocess.TimeoutExpired as error:
        stdout = error.stdout or ""
        stderr = error.stderr or ""
        if isinstance(stdout, bytes):
            stdout = stdout.decode("utf-8", errors="replace")
        if isinstance(stderr, bytes):
            stderr = stderr.decode("utf-8", errors="replace")
        return (
            None,
            stdout,
            stderr,
            round((time.monotonic() - started) * 1000, 2),
            True,
        )


def write_log(path: Path, command: Sequence[str], stdout: str, stderr: str) -> None:
    text = "$ " + shlex.join(command) + "\n"
    if stdout:
        text += "\n[stdout]\n" + stdout
    if stderr:
        text += "\n[stderr]\n" + stderr
    path.write_text(text, encoding="utf-8", errors="replace")


def finding_counts(stderr: str) -> dict[str, int]:
    return dict(sorted(Counter(FINDING_RE.findall(stderr)).items()))


def diagnostic_summary(stderr: str) -> str | None:
    match = ERROR_LINE_RE.search(stderr)
    return match.group(0).strip() if match else None


def duration_summary(values: Sequence[float]) -> dict[str, float | int | None]:
    if not values:
        return {"count": 0, "total_ms": 0.0, "mean_ms": None,
                "median_ms": None, "p95_ms": None, "max_ms": None}
    ordered = sorted(values)
    p95_index = min(len(ordered) - 1, max(0, int(len(ordered) * 0.95 + 0.999999) - 1))
    return {
        "count": len(values),
        "total_ms": round(sum(values), 2),
        "mean_ms": round(statistics.fmean(values), 2),
        "median_ms": round(statistics.median(values), 2),
        "p95_ms": round(ordered[p95_index], 2),
        "max_ms": round(ordered[-1], 2),
    }


def extracted_target_recall(results: Sequence[dict[str, Any]]) -> dict[str, Any]:
    if not any(result.get("plugin") is not None for result in results):
        return {
            "scope": "not evaluated without --plugin",
            "warning": "target recall requires analyzer findings",
            "aggregate": {"targets": 0, "detected": 0, "missed": 0,
                          "recall": None},
            "by_rule": {},
        }
    by_rule: dict[str, dict[str, int]] = defaultdict(
        lambda: {"targets": 0, "detected": 0, "missed": 0}
    )
    for result in results:
        if result["suite"] != "extracted" or result["status"] != "PASS":
            continue
        code = EXTRACTED_TARGET_CODE.get(result["category"])
        if code is None:
            continue
        counts = by_rule[code]
        counts["targets"] += 1
        if int(result["findings"].get(code, 0)) > 0:
            counts["detected"] += 1
        else:
            counts["missed"] += 1

    total = {"targets": 0, "detected": 0, "missed": 0}
    finalized: dict[str, dict[str, Any]] = {}
    for code, counts in sorted(by_rule.items()):
        for key in total:
            total[key] += counts[key]
        finalized[code] = {
            **counts,
            "recall": safe_rate(counts["detected"], counts["targets"]),
        }
    return {
        "scope": "positive target category in the 100 MemFix-derived models",
        "warning": "positive-only corpus: precision and false-positive rate are undefined",
        "aggregate": {
            **total,
            "recall": safe_rate(total["detected"], total["targets"]),
        },
        "by_rule": finalized,
    }


def safe_rate(numerator: int, denominator: int) -> float | None:
    return round(numerator / denominator, 6) if denominator else None


def run_case(args: argparse.Namespace, case: BenchmarkCase) -> dict[str, Any]:
    case_dir = case_directory(args.output, case)
    case_dir.mkdir(parents=True, exist_ok=True)
    baseline_object = case_dir / "baseline.o"
    baseline_log = case_dir / "baseline.log"
    baseline_command = compile_command(args, case, baseline_object, False)
    baseline_exit, baseline_stdout, baseline_stderr, baseline_ms, timed_out = (
        run_command(baseline_command, case_dir, args.timeout)
    )
    write_log(
        baseline_log, baseline_command, baseline_stdout, baseline_stderr
    )

    result: dict[str, Any] = {
        "id": case.case_id,
        "suite": case.suite,
        "group": case.group,
        "category": case.category,
        "source": case.repository_relative_source.as_posix(),
        "language": case.language,
        "status": "BUILD_FAIL" if timed_out or baseline_exit != 0 else "PASS",
        "failure_stage": "baseline" if timed_out or baseline_exit != 0 else None,
        "failure_reason": (
            "timeout" if timed_out else "compile_error" if baseline_exit != 0 else None
        ),
        "baseline": {
            "exit_code": baseline_exit,
            "duration_ms": baseline_ms,
            "timed_out": timed_out,
            "diagnostic": diagnostic_summary(baseline_stderr),
            "log": str(baseline_log.resolve()),
            "command": baseline_command,
        },
        "plugin": None,
        "findings": {},
        "duration_ms": baseline_ms,
    }

    if timed_out or baseline_exit != 0:
        if not args.keep_objects:
            baseline_object.unlink(missing_ok=True)
        return result

    if args.plugin is None:
        if not args.keep_objects:
            baseline_object.unlink(missing_ok=True)
        return result

    plugin_object = case_dir / "plugin.o"
    plugin_log = case_dir / "plugin.log"
    plugin_command = compile_command(args, case, plugin_object, True)
    plugin_exit, plugin_stdout, plugin_stderr, plugin_ms, plugin_timeout = (
        run_command(plugin_command, case_dir, args.timeout)
    )
    write_log(plugin_log, plugin_command, plugin_stdout, plugin_stderr)
    crashed = plugin_timeout or plugin_exit != 0
    result["plugin"] = {
        "exit_code": plugin_exit,
        "duration_ms": plugin_ms,
        "timed_out": plugin_timeout,
        "crash_signature": bool(CRASH_RE.search(plugin_stderr)),
        "diagnostic": diagnostic_summary(plugin_stderr),
        "log": str(plugin_log.resolve()),
        "command": plugin_command,
    }
    result["findings"] = finding_counts(plugin_stderr)
    result["duration_ms"] = round(baseline_ms + plugin_ms, 2)
    if crashed:
        result["status"] = "PLUGIN_CRASH"
        result["failure_stage"] = "plugin"
        if plugin_timeout:
            result["failure_reason"] = "timeout"
        elif CRASH_RE.search(plugin_stderr):
            result["failure_reason"] = "compiler_or_plugin_crash"
        else:
            result["failure_reason"] = "plugin_compile_failed"

    if not args.keep_objects:
        baseline_object.unlink(missing_ok=True)
        plugin_object.unlink(missing_ok=True)
    return result


def compiler_version(compiler: str) -> str:
    completed = subprocess.run(
        [compiler, "--version"],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    return completed.stdout.splitlines()[0] if completed.stdout else compiler


def print_summary(results: Sequence[dict[str, Any]], verbose: bool) -> None:
    if verbose:
        width = max((len(result["id"]) for result in results), default=1)
        for result in results:
            findings = ", ".join(
                f"{key}={value}" for key, value in result["findings"].items()
            )
            suffix = f"  {findings}" if findings else ""
            print(f"{result['status']:<12} {result['id']:<{width}}{suffix}")

    grouped: dict[tuple[str, str], Counter[str]] = defaultdict(Counter)
    for result in results:
        grouped[(result["suite"], result["group"])][result["status"]] += 1

    print("\nSuite      Group                         PASS  BUILD_FAIL  PLUGIN_CRASH  Total")
    print("---------- ---------------------------- ----- ----------- ------------- ------")
    for (suite, group), counts in sorted(grouped.items()):
        total = sum(counts.values())
        print(
            f"{suite:<10} {group:<28} {counts['PASS']:>5} "
            f"{counts['BUILD_FAIL']:>11} {counts['PLUGIN_CRASH']:>13} {total:>6}"
        )

    total_counts = Counter(result["status"] for result in results)
    print("---------- ---------------------------- ----- ----------- ------------- ------")
    print(
        f"{'TOTAL':<10} {'':<28} {total_counts['PASS']:>5} "
        f"{total_counts['BUILD_FAIL']:>11} "
        f"{total_counts['PLUGIN_CRASH']:>13} {len(results):>6}"
    )

    failures = [result for result in results if result["status"] != "PASS"]
    if failures and not verbose:
        shown = failures[:20]
        print("\nFailures:")
        for result in shown:
            stage = result[result["failure_stage"]]
            print(f"  {result['status']:<12} {result['source']}")
            if stage.get("diagnostic"):
                print(f"    {stage['diagnostic']}")
            print(f"    log: {stage['log']}")
        if len(failures) > len(shown):
            print(f"  ... {len(failures) - len(shown)} more; see report.json")


def write_html_report(path: Path, report: dict[str, Any]) -> None:
    rows: list[str] = []
    for result in report["cases"]:
        plugin = result.get("plugin")
        stage = plugin if plugin is not None else result["baseline"]
        log_path = Path(stage["log"])
        try:
            log_href = log_path.relative_to(path.parent).as_posix()
        except ValueError:
            log_href = log_path.as_uri()
        findings = ", ".join(
            f"{code}={count}" for code, count in result["findings"].items()
        ) or "none"
        css_class = "pass" if result["status"] == "PASS" else "fail"
        rows.append(
            "<tr>"
            f"<td class='{css_class}'>{html.escape(result['status'])}</td>"
            f"<td>{html.escape(result['suite'])}</td>"
            f"<td>{html.escape(result['group'])}</td>"
            f"<td><code>{html.escape(result['source'])}</code></td>"
            f"<td>{html.escape(findings)}</td>"
            f"<td>{result['duration_ms']:.2f} ms</td>"
            f"<td><a href='{html.escape(log_href)}'>log</a></td>"
            "</tr>"
        )

    summary = report["summary"]
    recall = report["accuracy"]["extracted_target_recall"]["aggregate"]["recall"]
    plugin_median = report["performance"]["plugin"]["median_ms"]
    document = f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Static Analyzer Benchmark Report</title>
<style>body{{font:14px system-ui,sans-serif;background:#f5f7fa;color:#172033;margin:0}}main{{max-width:1500px;margin:32px auto;padding:0 20px}}.cards{{display:flex;gap:12px;flex-wrap:wrap;margin:18px 0}}.card{{background:#fff;border:1px solid #dce2ea;border-radius:8px;padding:14px 18px;min-width:130px}}.number{{font-size:24px;font-weight:700}}table{{width:100%;border-collapse:collapse;background:#fff;border:1px solid #dce2ea}}th,td{{padding:9px;text-align:left;border-bottom:1px solid #e5e9ef;vertical-align:top}}th{{position:sticky;top:0;background:#eef2f7}}code{{font-size:12px}}.pass{{color:#177245;font-weight:700}}.fail{{color:#b42336;font-weight:700}}a{{color:#185abc}}</style></head>
<body><main><h1>Static Analyzer Benchmark Report</h1>
<div class="cards"><div class="card"><div class="number">{summary['total']}</div>Total</div><div class="card"><div class="number">{summary['PASS']}</div>Pass</div><div class="card"><div class="number">{summary['BUILD_FAIL']}</div>Build failures</div><div class="card"><div class="number">{summary['PLUGIN_CRASH']}</div>Plugin crashes</div><div class="card"><div class="number">{summary['duration_ms'] / 1000:.2f}s</div>Wall time</div><div class="card"><div class="number">{recall if recall is not None else 'n/a'}</div>Extracted target recall</div><div class="card"><div class="number">{plugin_median if plugin_median is not None else 'n/a'} ms</div>Plugin median</div></div>
<table><thead><tr><th>Status</th><th>Suite</th><th>Group</th><th>Source</th><th>Findings</th><th>Time</th><th>Evidence</th></tr></thead><tbody>{''.join(rows)}</tbody></table>
</main></body></html>"""
    path.write_text(document, encoding="utf-8")


def main() -> int:
    args = parse_args()
    try:
        validate_args(args)
        suites = args.suite or ["extracted"]
        families = args.family or list(EXTRACTED_FAMILIES)
        cwes = args.cwe or list(JULIET_CWES)
        cases = discover_cases(suites, families, cwes)
    except ValueError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    if args.match:
        cases = [
            case
            for case in cases
            if args.match in case.repository_relative_source.as_posix()
        ]
    if args.limit is not None:
        cases = cases[: args.limit]
    if not cases:
        print("error: no benchmark cases matched", file=sys.stderr)
        return 2

    args.output = args.output.resolve()
    args.output.mkdir(parents=True, exist_ok=True)
    snapshot_plugin(args)
    started = time.monotonic()
    results: list[dict[str, Any]] = []
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {executor.submit(run_case, args, case): case for case in cases}
        for future in as_completed(futures):
            results.append(future.result())
    results.sort(key=lambda result: result["id"])

    counts = Counter(result["status"] for result in results)
    findings: Counter[str] = Counter()
    for result in results:
        findings.update(result["findings"])
    baseline_durations = [
        float(result["baseline"]["duration_ms"]) for result in results
        if result["baseline"]["exit_code"] == 0
    ]
    plugin_durations = [
        float(result["plugin"]["duration_ms"]) for result in results
        if result["plugin"] is not None and result["plugin"]["exit_code"] == 0
    ]
    baseline_performance = duration_summary(baseline_durations)
    plugin_performance = duration_summary(plugin_durations)
    plugin_over_baseline_total = None
    plugin_over_baseline_median = None
    if baseline_performance["total_ms"] and plugin_performance["total_ms"]:
        plugin_over_baseline_total = round(
            float(plugin_performance["total_ms"]) /
            float(baseline_performance["total_ms"]), 4
        )
    if baseline_performance["median_ms"] and plugin_performance["median_ms"]:
        plugin_over_baseline_median = round(
            float(plugin_performance["median_ms"]) /
            float(baseline_performance["median_ms"]), 4
        )
    report = {
        "schema_version": 1,
        "generated_at_unix": int(time.time()),
        "repository": str(REPOSITORY_ROOT),
        "configuration": {
            "suites": suites,
            "families": families,
            "cwes": cwes,
            "match": args.match,
            "limit": args.limit,
            "cc": args.cc,
            "cc_version": compiler_version(args.cc),
            "cxx": args.cxx,
            "cxx_version": compiler_version(args.cxx),
            "plugin": str(args.plugin_original) if args.plugin_original else None,
            "plugin_snapshot": str(args.plugin) if args.plugin else None,
            "plugin_sha256": args.plugin_sha256,
            "plugin_args": normalized_plugin_args(args.plugin_arg)
            if args.plugin
            else [],
            "jobs": args.jobs,
            "timeout_seconds": args.timeout,
        },
        "summary": {
            "PASS": counts["PASS"],
            "BUILD_FAIL": counts["BUILD_FAIL"],
            "PLUGIN_CRASH": counts["PLUGIN_CRASH"],
            "total": len(results),
            "duration_ms": round((time.monotonic() - started) * 1000, 2),
            "findings": dict(sorted(findings.items())),
        },
        "accuracy": {
            "extracted_target_recall": extracted_target_recall(results),
        },
        "performance": {
            "baseline": baseline_performance,
            "plugin": plugin_performance,
            "ratios": {
                "plugin_over_baseline_total": plugin_over_baseline_total,
                "plugin_over_baseline_median": plugin_over_baseline_median,
            },
        },
        "cases": results,
    }
    report_path = args.output / "report.json"
    temporary_report = args.output / "report.json.tmp"
    temporary_report.write_text(
        json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    temporary_report.replace(report_path)
    html_path = args.output / "report.html"
    write_html_report(html_path, report)

    print_summary(results, args.verbose)
    print(f"\nHTML: {html_path}")
    print(f"JSON: {report_path}")
    return 0 if counts["BUILD_FAIL"] == 0 and counts["PLUGIN_CRASH"] == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
