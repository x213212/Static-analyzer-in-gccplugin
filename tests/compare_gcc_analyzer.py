#!/usr/bin/env python3
"""Compare this plugin with GCC 16.2's built-in ``-fanalyzer``.

Every manifest source is compiled twice in separate processes and directories:
once with this plugin loaded into GCC 16.2, and once with the same GCC 16.2
driver's native analyzer.  Diagnostics are read from stable plugin codes and
SARIF ``ruleId`` values; human-readable wording is not part of the contract.
"""

from __future__ import annotations

import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import html
import json
import os
from pathlib import Path
import re
import shutil
import statistics
import subprocess
import sys
import time
from typing import Any, Iterable


PLUGIN_CODES = ("SA001", "SA002", "SA003", "SA004")
PLUGIN_FINDING_RE = re.compile(r"\[(SA\d{3})\]")

# GCC 16 documents these as the matching -fanalyzer diagnostics.  Keep the
# leading "-W": that exact spelling is GCC's SARIF ruleId.
GCC_RULE_TO_PLUGIN_CODE = {
    "-Wanalyzer-malloc-leak": "SA001",
    "-Wanalyzer-double-free": "SA002",
    "-Wanalyzer-use-after-free": "SA003",
}

# Families make negative cases meaningful: e.g. the exclusive-branches case
# compares double-free specifically instead of all warnings GCC might emit.
FAMILY_CODE = {
    "memory-leak": "SA001",
    "double-free": "SA002",
    "use-after-free": "SA003",
    "thread": "SA004",
}

GCC16_AUTO_CANDIDATES = (
    "gcc-16",
    "gcc-16.2",
    "gcc-16.1",
    "x86_64-linux-gnu-gcc-16",
    "gcc",
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare stable plugin diagnostics with GCC 16 -fanalyzer"
    )
    parser.add_argument("--plugin-compiler", required=True)
    parser.add_argument(
        "--plugin-cxx",
        help="matching GCC 16.2 C++ driver; derived beside --plugin-compiler if omitted",
    )
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--manifest", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument(
        "--gcc16",
        help="GCC 16 C compiler; otherwise auto-detect GCC16, gcc-16, or gcc",
    )
    parser.add_argument(
        "--gcc16-cxx",
        help="matching GCC 16.2 C++ driver; derived beside --gcc16 if omitted",
    )
    parser.add_argument(
        "--require-gcc16",
        action="store_true",
        help="exit with status 2 instead of reporting SKIP when GCC 16 is absent",
    )
    return parser.parse_args()


def resolve_executable(candidate: str) -> str | None:
    if os.sep in candidate:
        path = Path(candidate).expanduser()
        return str(path.resolve()) if path.is_file() and os.access(path, os.X_OK) else None
    return shutil.which(candidate)


def compiler_version(executable: str) -> tuple[str | None, str]:
    completed = subprocess.run(
        [executable, "-dumpfullversion", "-dumpversion"],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if completed.returncode != 0:
        reason = completed.stderr.strip() or f"version probe exited {completed.returncode}"
        return None, reason
    output = completed.stdout.strip().split()
    if not output:
        return None, "version probe returned no version"
    return output[0], ""


def compiler_banner(executable: str) -> str:
    completed = subprocess.run(
        [executable, "--version"],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    lines = (completed.stdout or completed.stderr).splitlines()
    return lines[0] if lines else "unknown"


def derive_cxx_driver(c_compiler: str) -> str | None:
    """Find the matching g++ driver without falling back to another GCC ABI."""
    c_path = Path(c_compiler).resolve()
    name = c_path.name
    candidates: list[Path] = []
    if "gcc" in name:
        candidates.append(c_path.with_name(name.replace("gcc", "g++", 1)))
    candidates.append(c_path.with_name("g++"))
    c_version, _ = compiler_version(str(c_path))
    for candidate in candidates:
        executable = resolve_executable(str(candidate))
        if executable is None:
            continue
        cxx_version, _ = compiler_version(executable)
        if cxx_version == c_version:
            return executable
    return None


def require_matching_cxx(
    requested: str | None, c_compiler: str, option_name: str
) -> tuple[str | None, str | None]:
    executable = (
        resolve_executable(requested)
        if requested is not None
        else derive_cxx_driver(c_compiler)
    )
    if executable is None:
        return None, f"{option_name}: no matching C++ driver found"
    c_version, c_error = compiler_version(c_compiler)
    cxx_version, cxx_error = compiler_version(executable)
    if c_version is None or cxx_version is None or c_version != cxx_version:
        detail = (
            f"C={c_version or c_error}, C++={cxx_version or cxx_error}"
        )
        return None, f"{option_name}: C and C++ driver versions differ ({detail})"
    if not gcc_16_2_version(cxx_version):
        return None, f"{option_name}: expected GCC 16.2.x, got {cxx_version}"
    return executable, None


def gcc_16_2_version(version: str) -> bool:
    return re.fullmatch(
        r"16\.2(?:\.\d+)+(?:[-+~][A-Za-z0-9._+~-]+)?", version
    ) is not None


def discover_gcc16(requested: str | None) -> dict[str, Any]:
    candidates: list[str] = []
    if requested:
        candidates.append(requested)
    else:
        configured = os.environ.get("GCC16")
        if configured:
            candidates.append(configured)
        candidates.extend(GCC16_AUTO_CANDIDATES)

    attempts: list[dict[str, str]] = []
    seen: set[str] = set()
    for candidate in candidates:
        if candidate in seen:
            continue
        seen.add(candidate)
        executable = resolve_executable(candidate)
        if executable is None:
            attempts.append({"candidate": candidate, "reason": "not found"})
            continue
        version, reason = compiler_version(executable)
        if version is None:
            attempts.append({"candidate": candidate, "reason": reason})
            continue
        if not gcc_16_2_version(version):
            attempts.append(
                {"candidate": candidate,
                 "reason": f"version {version}, not GCC 16.2.x"}
            )
            continue
        return {
            "status": "AVAILABLE",
            "compiler": executable,
            "version": version,
            "banner": compiler_banner(executable),
            "attempts": attempts,
        }

    tried = ", ".join(
        f"{item['candidate']} ({item['reason']})" for item in attempts
    )
    return {
        "status": "SKIP",
        "compiler": None,
        "version": None,
        "reason": "no GCC 16.2.x C compiler found" +
                  (f": {tried}" if tried else ""),
        "attempts": attempts,
    }


def zero_counts() -> dict[str, int]:
    return {code: 0 for code in PLUGIN_CODES}


def plugin_counts(stderr: str) -> dict[str, int]:
    result = zero_counts()
    for code in PLUGIN_FINDING_RE.findall(stderr):
        if code in result:
            result[code] += 1
    return result


def expected_counts(case: dict[str, Any]) -> dict[str, int]:
    declared = case.get("expect", {})
    if not isinstance(declared, dict):
        raise ValueError(f"case {case.get('name')!r}: 'expect' must be an object")
    return {code: int(declared.get(code, 0)) for code in PLUGIN_CODES}


def iter_sarif_results(document: Any) -> Iterable[dict[str, Any]]:
    if not isinstance(document, dict):
        return
    runs = document.get("runs", [])
    if not isinstance(runs, list):
        return
    for run in runs:
        if not isinstance(run, dict):
            continue
        results = run.get("results", [])
        if not isinstance(results, list):
            continue
        for result in results:
            if isinstance(result, dict):
                yield result


def parse_gcc_sarif(text: str) -> tuple[dict[str, int], dict[str, int], str | None]:
    mapped = zero_counts()
    all_analyzer_rules: Counter[str] = Counter()
    try:
        document = json.loads(text)
    except json.JSONDecodeError as error:
        return mapped, {}, f"invalid SARIF JSON: {error}"

    for result in iter_sarif_results(document):
        rule_id = result.get("ruleId")
        if not isinstance(rule_id, str) or not rule_id.startswith("-Wanalyzer-"):
            continue
        all_analyzer_rules[rule_id] += 1
        code = GCC_RULE_TO_PLUGIN_CODE.get(rule_id)
        if code:
            mapped[code] += 1
    return mapped, dict(sorted(all_analyzer_rules.items())), None


def run_command(command: list[str], cwd: Path) -> tuple[subprocess.CompletedProcess[str], float]:
    started = time.monotonic()
    completed = subprocess.run(
        command,
        cwd=cwd,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    duration_ms = round((time.monotonic() - started) * 1000, 2)
    return completed, duration_ms


def run_plugin(
    compiler: str,
    plugin: Path,
    source: Path,
    case: dict[str, Any],
    case_dir: Path,
    language: str,
) -> dict[str, Any]:
    run_dir = case_dir / "plugin"
    run_dir.mkdir(parents=True, exist_ok=True)
    log_path = run_dir / "compiler.log"
    object_path = run_dir / "case.o"
    command = [
        compiler,
        "-x",
        language,
        f"-fplugin={plugin}",
        "-fplugin-arg-newplugin-debugoutput=0",
        "-fplugin-arg-newplugin-report=0",
        "-fplugin-arg-newplugin-traceallfucntion=1",
        "-fdiagnostics-color=never",
        "-O1",
        "-fipa-pta",
        "-fno-inline",
        "-fno-tree-dce",
        "-fno-tree-dse",
        "-fno-tree-fre",
        "-pthread",
    ]
    plugin_args = case.get("plugin_args", {})
    if not isinstance(plugin_args, dict):
        raise ValueError(f"case {case.get('name')!r}: 'plugin_args' must be an object")
    for key, value in sorted(plugin_args.items()):
        command.append(f"-fplugin-arg-newplugin-{key}={value}")
    command.extend(["-c", str(source), "-o", str(object_path.resolve())])

    completed, duration_ms = run_command(command, run_dir)
    log_path.write_text(completed.stderr, encoding="utf-8")
    actual = plugin_counts(completed.stderr)
    return {
        "status": "OK" if completed.returncode == 0 else "ERROR",
        "exit_code": completed.returncode,
        "findings": actual,
        "duration_ms": duration_ms,
        "log": str(log_path.resolve()),
        "command": command,
    }


def run_gcc16(
    compiler: str, source: Path, case_dir: Path, language: str
) -> dict[str, Any]:
    run_dir = case_dir / "gcc16"
    run_dir.mkdir(parents=True, exist_ok=True)
    sarif_path = run_dir / "diagnostics.sarif"
    stdout_path = run_dir / "compiler.stdout"
    object_path = run_dir / "case.o"
    command = [
        compiler,
        "-x",
        language,
        "-fanalyzer",
        "-fanalyzer-verbosity=0",
        "-fdiagnostics-format=sarif-stderr",
        "-O0",
        "-fno-inline",
        "-pthread",
        "-c",
        str(source),
        "-o",
        str(object_path.resolve()),
    ]
    completed, duration_ms = run_command(command, run_dir)
    sarif_path.write_text(completed.stderr, encoding="utf-8")
    stdout_path.write_text(completed.stdout, encoding="utf-8")
    mapped, all_rules, parse_error = parse_gcc_sarif(completed.stderr)
    status = "OK"
    if completed.returncode != 0 or parse_error is not None:
        status = "ERROR"
    return {
        "status": status,
        "exit_code": completed.returncode,
        "findings": mapped,
        "analyzer_rule_ids": all_rules,
        "sarif_parse_error": parse_error,
        "duration_ms": duration_ms,
        "sarif": str(sarif_path.resolve()),
        "stdout": str(stdout_path.resolve()),
        "command": command,
    }


def target_code(case_name: str) -> str | None:
    return FAMILY_CODE.get(case_name.split("/", 1)[0])


def compare_case(
    case_name: str, plugin_run: dict[str, Any], gcc_run: dict[str, Any]
) -> dict[str, Any]:
    code = target_code(case_name)
    if gcc_run["status"] == "SKIP":
        return {"status": "SKIP", "code": code, "reason": gcc_run["reason"]}
    if plugin_run["status"] != "OK" or gcc_run["status"] != "OK":
        return {"status": "ERROR", "code": code}
    if code == "SA004":
        return {
            "status": "NOT_SUPPORTED",
            "code": code,
            "reason": "no direct GCC 16 -fanalyzer ruleId mapping for thread join lifetime",
        }

    codes = [code] if code is not None else list(GCC_RULE_TO_PLUGIN_CODE.values())
    delta = {
        item: int(plugin_run["findings"][item]) - int(gcc_run["findings"][item])
        for item in codes
    }
    return {
        "status": "MATCH" if all(value == 0 for value in delta.values()) else "DIFFERENT",
        "code": code,
        "plugin_minus_gcc16": delta,
    }


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def compact_counts(counts: dict[str, int], only: str | None = None) -> str:
    codes = [only] if only else list(PLUGIN_CODES)
    present = [f"{code}={counts.get(code, 0)}" for code in codes if counts.get(code, 0)]
    return ",".join(present) if present else "none"


def load_manifest(path: Path) -> list[dict[str, Any]]:
    document = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(document, list) or not document:
        raise ValueError("manifest must be a non-empty JSON array")
    for index, case in enumerate(document):
        if not isinstance(case, dict) or "name" not in case or "source" not in case:
            raise ValueError(f"manifest case {index} must contain name and source")
    return document


def case_language(case: dict[str, Any]) -> str:
    declared = case.get("language")
    if declared is not None:
        if declared not in ("c", "c++"):
            raise ValueError(
                f"case {case.get('name')!r}: language must be 'c' or 'c++'"
            )
        return str(declared)
    return "c++" if Path(str(case["source"])).suffix.lower() in (".cc", ".cpp", ".cxx") else "c"


def safe_rate(numerator: int, denominator: int) -> float | None:
    return round(numerator / denominator, 6) if denominator else None


def classification_metrics(
    results: list[dict[str, Any]], tool: str, supported_codes: set[str]
) -> dict[str, Any]:
    by_rule: dict[str, dict[str, int]] = {}
    aggregate = {"tp": 0, "fp": 0, "fn": 0, "tn": 0, "exact_count": 0,
                 "evaluated": 0}
    for item in results:
        code = target_code(str(item["name"]))
        run = item[tool]
        if code is None or code not in supported_codes or run["status"] != "OK":
            continue
        expected_count = int(item["expected_plugin_findings"].get(code, 0))
        actual_count = int(run["findings"].get(code, 0))
        counts = by_rule.setdefault(
            code, {"tp": 0, "fp": 0, "fn": 0, "tn": 0,
                   "exact_count": 0, "evaluated": 0}
        )
        expected_positive = expected_count > 0
        actual_positive = actual_count > 0
        if expected_positive and actual_positive:
            outcome = "tp"
        elif not expected_positive and actual_positive:
            outcome = "fp"
        elif expected_positive and not actual_positive:
            outcome = "fn"
        else:
            outcome = "tn"
        counts[outcome] += 1
        aggregate[outcome] += 1
        counts["evaluated"] += 1
        aggregate["evaluated"] += 1
        if expected_count == actual_count:
            counts["exact_count"] += 1
            aggregate["exact_count"] += 1

    def finalize(counts: dict[str, int]) -> dict[str, Any]:
        tp, fp, fn, tn = (counts[key] for key in ("tp", "fp", "fn", "tn"))
        precision = safe_rate(tp, tp + fp)
        recall = safe_rate(tp, tp + fn)
        f1 = (
            round(2 * precision * recall / (precision + recall), 6)
            if precision is not None and recall is not None and precision + recall
            else None
        )
        return {
            **counts,
            "precision": precision,
            "recall": recall,
            "f1": f1,
            "binary_accuracy": safe_rate(tp + tn, tp + fp + fn + tn),
            "exact_count_accuracy": safe_rate(
                counts["exact_count"], counts["evaluated"]
            ),
        }

    return {
        "scope": "one reviewed target rule per manifest case",
        "aggregate": finalize(aggregate),
        "by_rule": {code: finalize(counts) for code, counts in sorted(by_rule.items())},
    }


def duration_summary(values: list[float]) -> dict[str, float | int | None]:
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


def performance_metrics(results: list[dict[str, Any]]) -> dict[str, Any]:
    plugin = duration_summary(
        [float(item["plugin"]["duration_ms"]) for item in results
         if item["plugin"]["status"] == "OK"]
    )
    gcc16 = duration_summary(
        [float(item["gcc16"]["duration_ms"]) for item in results
         if item["gcc16"]["status"] == "OK"]
    )
    ratios: dict[str, float | None] = {
        "gcc16_over_plugin_total": None,
        "gcc16_over_plugin_median": None,
    }
    if plugin["total_ms"] and gcc16["total_ms"]:
        ratios["gcc16_over_plugin_total"] = round(
            float(gcc16["total_ms"]) / float(plugin["total_ms"]), 4
        )
    if plugin["median_ms"] and gcc16["median_ms"]:
        ratios["gcc16_over_plugin_median"] = round(
            float(gcc16["median_ms"]) / float(plugin["median_ms"]), 4
        )
    return {
        "policy": (
            "diagnostic-only process timings; the plugin uses its regression "
            "configuration at -O1 while native -fanalyzer uses its documented "
            "-O0 configuration, runs are not paired by execution order, and these "
            "values must not be used to declare a speed winner"
        ),
        "speed_comparison_valid": False,
        "plugin": plugin,
        "gcc16_fanalyzer": gcc16,
        "ratios": {
            **ratios,
            "interpretation": "descriptive only; configurations are not comparable",
        },
    }


def write_html_report(path: Path, report: dict[str, Any]) -> None:
    rows: list[str] = []
    for item in report["cases"]:
        code = item["comparison"].get("code")
        plugin_text = compact_counts(item["plugin"]["findings"], code)
        if item["gcc16"]["status"] == "SKIP":
            gcc_text = "SKIP"
        elif code == "SA004":
            gcc_text = "not supported"
        elif item["gcc16"]["status"] != "OK":
            gcc_text = "ERROR"
        else:
            gcc_text = compact_counts(item["gcc16"]["findings"], code)
        comparison = item["comparison"]["status"]
        css_class = "pass" if comparison in ("MATCH", "NOT_SUPPORTED", "SKIP") else "fail"
        rows.append(
            "<tr>"
            f"<td>{html.escape(item['name'])}</td>"
            f"<td>{html.escape(code or 'all')}</td>"
            f"<td>{html.escape(plugin_text)}</td>"
            f"<td>{html.escape(gcc_text)}</td>"
            f"<td class='{css_class}'>{html.escape(comparison)}</td>"
            "</tr>"
        )
    summary = report["summary"]
    accuracy = report["accuracy"]
    performance = report["performance"]
    gcc = report["gcc16_toolchain"]
    gcc_label = gcc.get("banner") or gcc.get("reason", "unavailable")
    document = f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Static Analyzer vs GCC 16</title>
<style>body{{font:15px system-ui,sans-serif;background:#f5f7fa;color:#172033;margin:0}}main{{max-width:1100px;margin:36px auto;padding:0 22px}}.meta{{background:#fff;border:1px solid #dce2ea;border-radius:8px;padding:14px;margin:16px 0}}table{{width:100%;border-collapse:collapse;background:#fff;border:1px solid #dce2ea}}th,td{{padding:10px;text-align:left;border-bottom:1px solid #e5e9ef}}th{{background:#eef2f7}}.pass{{color:#177245;font-weight:700}}.fail{{color:#b42336;font-weight:700}}</style></head>
<body><main><h1>Static Analyzer vs GCC 16</h1><div class="meta">GCC reference: {html.escape(str(gcc_label))}<br>Plugin regression tests: {summary['plugin_passed']} passed, {summary['plugin_failed']} failed<br>Plugin precision / recall / F1: {accuracy['plugin']['aggregate']['precision']} / {accuracy['plugin']['aggregate']['recall']} / {accuracy['plugin']['aggregate']['f1']}<br>GCC 16 precision / recall / F1: {accuracy['gcc16_fanalyzer']['aggregate']['precision']} / {accuracy['gcc16_fanalyzer']['aggregate']['recall']} / {accuracy['gcc16_fanalyzer']['aggregate']['f1']}<br>Configuration-specific median process time: plugin {performance['plugin']['median_ms']} ms · GCC 16 -fanalyzer {performance['gcc16_fanalyzer']['median_ms']} ms<br><strong>Diagnostic-only:</strong> optimization modes differ intentionally, so this report does not declare a speed winner. Use the manifest-oracle harness for same-flag paired timing.</div>
<table><thead><tr><th>Case</th><th>Rule</th><th>Plugin</th><th>GCC 16</th><th>Comparison</th></tr></thead><tbody>{''.join(rows)}</tbody></table>
</main></body></html>"""
    path.write_text(document, encoding="utf-8")


def main() -> int:
    args = parse_args()
    manifest = args.manifest.resolve()
    tests_root = manifest.parent
    plugin = args.plugin.resolve()
    output = args.output.resolve()

    plugin_compiler = resolve_executable(args.plugin_compiler)
    if plugin_compiler is None:
        print(f"ERROR: plugin compiler not found: {args.plugin_compiler}", file=sys.stderr)
        return 2
    if not plugin.is_file():
        print(f"ERROR: plugin not found: {plugin}", file=sys.stderr)
        return 2
    plugin_version, plugin_version_error = compiler_version(plugin_compiler)
    if plugin_version is None or not gcc_16_2_version(plugin_version):
        detail = plugin_version or plugin_version_error
        print(
            f"ERROR: --plugin-compiler must be GCC 16.2.x; got {detail}",
            file=sys.stderr,
        )
        return 2
    try:
        cases = load_manifest(manifest)
        languages = {case_language(case) for case in cases}
    except (OSError, ValueError, json.JSONDecodeError) as error:
        print(f"ERROR: cannot load manifest {manifest}: {error}", file=sys.stderr)
        return 2

    plugin_cxx: str | None = None
    if "c++" in languages:
        plugin_cxx, cxx_error = require_matching_cxx(
            args.plugin_cxx, plugin_compiler, "--plugin-cxx"
        )
        if plugin_cxx is None:
            print(f"ERROR: {cxx_error}", file=sys.stderr)
            return 2

    output.mkdir(parents=True, exist_ok=True)
    # Default to the exact driver used for the plugin side so this is a native
    # GCC 16.2 vs plugin-on-GCC-16.2 comparison, not a cross-version result.
    gcc16 = discover_gcc16(args.gcc16 or plugin_compiler)
    gcc16_cxx: str | None = None
    if gcc16["status"] == "AVAILABLE" and "c++" in languages:
        gcc16_cxx, cxx_error = require_matching_cxx(
            args.gcc16_cxx,
            str(gcc16["compiler"]),
            "--gcc16-cxx",
        )
        if gcc16_cxx is None:
            print(f"ERROR: {cxx_error}", file=sys.stderr)
            return 2
        gcc16["cxx_compiler"] = gcc16_cxx
        gcc16["cxx_banner"] = compiler_banner(gcc16_cxx)
    results: list[dict[str, Any]] = []
    plugin_regressions = 0
    execution_errors = 0

    for case in cases:
        name = str(case["name"])
        language = case_language(case)
        source = (tests_root / str(case["source"])).resolve()
        if not source.is_file():
            print(f"ERROR: case source not found: {source}", file=sys.stderr)
            return 2
        case_dir = output / name.replace("/", "__")
        case_dir.mkdir(parents=True, exist_ok=True)

        expected = expected_counts(case)
        plugin_driver = (
            str(plugin_cxx) if language == "c++" else plugin_compiler
        )
        plugin_run = run_plugin(
            plugin_driver, plugin, source, case, case_dir, language
        )
        expectation = (
            "PASS"
            if plugin_run["status"] == "OK" and plugin_run["findings"] == expected
            else "FAIL"
        )
        if expectation == "FAIL":
            plugin_regressions += 1
        if plugin_run["status"] != "OK":
            execution_errors += 1

        if gcc16["status"] == "AVAILABLE":
            gcc_driver = (
                str(gcc16_cxx)
                if language == "c++"
                else str(gcc16["compiler"])
            )
            gcc_run = run_gcc16(gcc_driver, source, case_dir, language)
            if gcc_run["status"] != "OK":
                execution_errors += 1
        else:
            gcc_run = {
                "status": "SKIP",
                "reason": gcc16["reason"],
                "findings": zero_counts(),
                "analyzer_rule_ids": {},
            }

        comparison = compare_case(name, plugin_run, gcc_run)
        results.append(
            {
                "name": name,
                "language": language,
                "source": str(source.relative_to(tests_root)),
                "source_sha256": sha256(source),
                "expected_plugin_findings": expected,
                "plugin_expectation": expectation,
                "plugin": plugin_run,
                "gcc16": gcc_run,
                "comparison": comparison,
            }
        )

    comparison_counts = Counter(item["comparison"]["status"] for item in results)
    accuracy = {
        "plugin": classification_metrics(results, "plugin", set(PLUGIN_CODES)),
        "gcc16_fanalyzer": classification_metrics(
            results, "gcc16", set(GCC_RULE_TO_PLUGIN_CODE.values())
        ),
    }
    performance = performance_metrics(results)
    summary = {
        "schema_version": 1,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "manifest": str(manifest),
        "plugin_toolchain": {
            "compiler": plugin_compiler,
            "cxx_compiler": plugin_cxx,
            "version": compiler_version(plugin_compiler)[0],
            "banner": compiler_banner(plugin_compiler),
            "cxx_banner": compiler_banner(plugin_cxx) if plugin_cxx else None,
            "plugin": str(plugin),
        },
        "gcc16_toolchain": gcc16,
        "summary": {
            "total": len(results),
            "plugin_passed": len(results) - plugin_regressions,
            "plugin_failed": plugin_regressions,
            "execution_errors": execution_errors,
            "comparisons": dict(sorted(comparison_counts.items())),
        },
        "accuracy": accuracy,
        "performance": performance,
        "comparison_policy": (
            "curated regression and diagnostic comparison; C++ native results are "
            "experimental and the differently configured process timings do not "
            "support a speed winner"
        ),
        "diagnostic_mapping": {
            code: rule for rule, code in GCC_RULE_TO_PLUGIN_CODE.items()
        },
        "cases": results,
    }
    report_path = output / "comparison.json"
    report_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    html_path = output / "comparison.html"
    write_html_report(html_path, summary)

    name_width = max(len(str(item["name"])) for item in results)
    print(f"{'CASE':<{name_width}}  PLUGIN  FINDINGS        GCC16           COMPARE")
    for item in results:
        code = item["comparison"].get("code")
        plugin_display = compact_counts(item["plugin"]["findings"], code)
        if item["gcc16"]["status"] == "SKIP":
            gcc_display = "SKIP"
        elif code == "SA004":
            gcc_display = "n/a"
        elif item["gcc16"]["status"] != "OK":
            gcc_display = "ERROR"
        else:
            gcc_display = compact_counts(item["gcc16"]["findings"], code)
        print(
            f"{item['name']:<{name_width}}  "
            f"{item['plugin_expectation']:<6}  "
            f"{plugin_display:<14}  "
            f"{gcc_display:<14}  "
            f"{item['comparison']['status']}"
        )

    if gcc16["status"] == "SKIP":
        print(f"\nGCC 16 comparison: SKIPPED — {gcc16['reason']}")
    else:
        print(f"\nGCC 16 comparison: {gcc16['banner']}")
    print(
        "Timing: diagnostic-only (different optimization configurations); "
        "no speed winner is reported"
    )
    print(f"HTML: {html_path}")
    print(f"JSON: {report_path}")

    if args.require_gcc16 and gcc16["status"] == "SKIP":
        return 2
    if execution_errors or plugin_regressions:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
