#!/usr/bin/env python3
"""Isolated black-box tests for the GCC plugin.

Each C or C++ file is compiled in a fresh process.  Assertions cross-check stable
SAxxx GCC diagnostics against the plugin's JSON and HTML reports; legacy debug
text and GCC dump evidence are never treated as test results.
"""

from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import html
import json
import math
import os
from pathlib import Path
import re
import shutil
import sys
import time

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.compare_lto_project import run_process


FINDING_CODES = ("SA001", "SA002", "SA003", "SA004")
FINDING_RE = re.compile(r"\[(SA\d{3})\]")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", required=True)
    parser.add_argument(
        "--cxx-compiler",
        help="C++ driver for cases with language=c++; defaults to the matching g++",
    )
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--manifest", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--dump", action="store_true")
    parser.add_argument("--timeout", type=float, default=15.0)
    parser.add_argument(
        "--jobs", type=int, default=min(8, os.cpu_count() or 1),
        help="isolated compiler processes in parallel (default: up to 8); use 1 for serial timing",
    )
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error("--jobs must be positive")
    if not math.isfinite(args.timeout) or args.timeout <= 0:
        parser.error("--timeout must be finite and positive")
    return args


def infer_cxx_compiler(c_compiler: str) -> str:
    path = Path(c_compiler)
    name = path.name
    if "gcc" in name:
        cxx_name = name.replace("gcc", "g++", 1)
        if path.parent != Path("."):
            sibling = path.with_name(cxx_name)
            if sibling.is_file():
                return str(sibling)
        resolved = shutil.which(cxx_name)
        if resolved:
            return resolved
    if "clang" in name:
        cxx_name = name.replace("clang", "clang++", 1)
        if path.parent != Path("."):
            sibling = path.with_name(cxx_name)
            if sibling.is_file():
                return str(sibling)
        resolved = shutil.which(cxx_name)
        if resolved:
            return resolved
    # GCC-compatible drivers select cc1plus from a C++ source suffix.  Keep
    # that behavior as the portable fallback for wrappers and cross drivers.
    return c_compiler


def case_language(case: dict[str, object], source: Path) -> str:
    declared = str(case.get("language", "")).lower()
    if declared in ("c++", "cpp", "cxx"):
        return "c++"
    if declared == "c":
        return "c"
    return "c++" if source.suffix in (".cc", ".cpp", ".cxx", ".C") else "c"


def finding_counts(text: str) -> dict[str, int]:
    counts = {code: 0 for code in FINDING_CODES}
    for code in FINDING_RE.findall(text):
        if code in counts:
            counts[code] += 1
    return counts


def expected_counts(case: dict[str, object]) -> dict[str, int]:
    declared = case.get("expect", {})
    return {code: int(declared.get(code, 0)) for code in FINDING_CODES}


def expected_inconclusive_counts(case: dict[str, object]) -> dict[str, int]:
    declared = case.get("expect_inconclusive", {})
    return {code: int(declared.get(code, 0)) for code in FINDING_CODES}


def has_lifetime_trace(roles: set[str]) -> bool:
    local = "allocation" in roles and any(
        isinstance(role, str) and "release" in role for role in roles)
    returned = {
        "callee allocation origin", "callee releases before return",
        "callee returns released pointer", "released object returned from callee",
    }.issubset(roles)
    return local or returned


def run_case(args: argparse.Namespace, tests_root: Path,
             case: dict[str, object]) -> dict[str, object]:
    name = str(case["name"])
    case_dir = args.output / name.replace("/", "__")
    case_dir.mkdir(parents=True, exist_ok=True)
    source = tests_root / str(case["source"])
    language = case_language(case, source)
    compiler = (
        args.cxx_compiler
        if language == "c++" and args.cxx_compiler
        else infer_cxx_compiler(args.compiler)
        if language == "c++"
        else args.compiler
    )
    object_file = case_dir / "case.o"
    stderr_file = case_dir / "compiler.log"
    report_stem = (case_dir / "analysis").resolve()
    json_report = report_stem.with_suffix(".json")
    html_report = report_stem.with_suffix(".html")
    plugin_options = dict(case.get("plugin_args", {}))
    expect_html = str(plugin_options.get("reporthtml", "1")) != "0"
    for stale in (object_file, stderr_file, json_report, html_report):
        stale.unlink(missing_ok=True)

    command = [
        compiler,
        f"-fplugin={args.plugin.resolve()}",
        "-fplugin-arg-newplugin-debugoutput=0",
        "-fplugin-arg-newplugin-traceallfucntion=1",
        f"-fplugin-arg-newplugin-reportpath={report_stem}",
        "-fdiagnostics-color=never",
        "-O1",
        "-fipa-pta",
        "-fno-inline",
        "-fno-tree-dce",
        "-fno-tree-dse",
        "-fno-tree-fre",
        "-pthread",
    ]
    if str(case.get("language", "")).lower() in ("c++", "cpp", "cxx"):
        command.extend(["-x", "c++"])
    command.extend(str(value) for value in case.get("compile_args", []))
    for key, value in plugin_options.items():
        command.append(f"-fplugin-arg-newplugin-{key}={value}")
    if args.dump:
        command.append("-fdump-ipa-all-details")
    command.extend(["-c", str(source.resolve()), "-o", str(object_file.resolve())])

    # Share the benchmark's non-polling wait and process-group timeout. A
    # timed-out GCC driver must not leave cc1/cc1plus occupying a worker.
    execution = run_process(command, case_dir,
                            (case_dir / "compiler").resolve(), args.timeout)
    timed_out = execution["timed_out"]
    exit_code = execution["exit_code"]
    stderr = Path(execution["stderr_path"]).read_text(
        encoding="utf-8", errors="replace")
    duration_ms = round(execution["duration_ms"], 2)
    stderr_file.write_text(stderr, encoding="utf-8")

    actual = finding_counts(stderr)
    expected = expected_counts(case)
    report_counts = {code: 0 for code in FINDING_CODES}
    report_inconclusive = {code: 0 for code in FINDING_CODES}
    report_inconclusive_reasons: dict[str, int] = {}
    report_error = None
    try:
        document = json.loads(json_report.read_text(encoding="utf-8"))
        report_counts = {
            code: int(document["summary"].get(code, 0)) for code in FINDING_CODES
        }
        if int(document.get("schema_version", 0)) < 2:
            raise ValueError("plugin report does not implement proof-status schema")
        inconclusive_summary = document["inconclusive_summary"]
        report_inconclusive = {
            code: int(inconclusive_summary["by_rule"].get(code, 0))
            for code in FINDING_CODES
        }
        report_inconclusive_reasons = {
            str(reason): int(count)
            for reason, count in inconclusive_summary["by_reason"].items()
        }
        if int(document["summary"].get("inconclusive_total", -1)) != sum(
            report_inconclusive.values()
        ):
            raise ValueError("inconclusive total/by-rule counts disagree")
        if any(
            finding.get("status") == "INCONCLUSIVE"
            for finding in document.get("findings", [])
        ):
            raise ValueError("INCONCLUSIVE record leaked into findings")
        if any(
            sample.get("status") != "INCONCLUSIVE"
            for sample in document.get("inconclusive", [])
        ):
            raise ValueError("invalid inconclusive sample status")
        for sample in document.get("inconclusive", []):
            related = sample.get("related", [])
            if len(related) > 8 or any(
                not point.get("file") or int(point.get("line", 0)) <= 0
                or not point.get("role") for point in related
            ):
                raise ValueError("invalid or unbounded inconclusive related locations")
        for reason in case.get("expect_inconclusive_related", []):
            matching = [sample for sample in document.get("inconclusive", [])
                        if sample.get("reason") == reason]
            if not matching or any(not sample.get("related") for sample in matching):
                raise ValueError(f"{reason} has no ownership barrier source location")
        observed_trace_roles = {code: set() for code in FINDING_CODES}
        observed_primary_lines = {code: [] for code in FINDING_CODES}
        observed_trace_role_lines = {code: {} for code in FINDING_CODES}
        expected_policy = str(
            plugin_options.get("analysispolicy", "proof-only")
        )
        expected_finding_status = (
            "PROVEN" if expected_policy == "proof-only" else "EXPLORATORY"
        )
        expected_confidence = (
            "high" if expected_policy == "proof-only" else "uncertain"
        )
        if plugin_options.get("calleemodel", "unknown") != "unknown":
            expected_finding_status = "ASSUMPTION_BASED"
            expected_confidence = "conditional"
        if "expect_finding_status" in case:
            expected_finding_status = case["expect_finding_status"]
            expected_confidence = case.get("expect_confidence", "conditional")
        effects = document.get("callee_effects", [])
        report_only = str(plugin_options.get("reportonly", "0")) == "1"
        if document.get("analysis", {}).get("diagnostics") != (
            "report-only" if report_only else "warning"
        ):
            raise ValueError("report/compiler diagnostic mode mismatch")
        if "expect_standard_stdio_model" in case and document.get("analysis", {}).get("standard_stdio_model") != case["expect_standard_stdio_model"]:
            raise ValueError("standard stdio model activation mismatch")
        if len(effects) > 128:
            raise ValueError("callee effect ledger exceeds its bound")
        for effect in effects:
            if not isinstance(effect.get("needs_review"), bool) or not (
                0 <= int(effect.get("argument_index", -1)) < int(effect.get("argument_count", 0))
            ):
                raise ValueError("invalid callee effect ledger")
        for symbol in case.get("expect_callee_review", []):
            if not any(effect.get("symbol") == symbol and effect.get("needs_review") for effect in effects):
                raise ValueError(f"missing unresolved callee: {symbol}")
        for finding in document.get("findings", []):
            if finding.get("status") == "MODEL_REVIEW" and (
                not finding.get("model_scope") or
                finding.get("path_verdict") not in ("feasible", "unknown") or
                document.get("analysis", {}).get("library_model") != "ffmpeg-sdk-v1"
            ):
                raise ValueError("model review lost its explicit profile/path limits")
            if (
                finding.get("status") != expected_finding_status
                or finding.get("confidence") != expected_confidence
            ):
                raise ValueError(
                    "finding status/confidence does not match the analysis policy"
                )
            remediation = finding.get("remediation")
            if (
                not isinstance(remediation, dict)
                or remediation.get("automatic_patch") is not False
                or not remediation.get("summary")
            ):
                raise ValueError("finding has no bounded manual remediation guidance")
            location = finding.get("location", {})
            if not location.get("file") or int(location.get("line", 0)) <= 0:
                raise ValueError("finding has no source location")
            if finding.get("rule_id") in ("SA002", "SA003"):
                trace = finding.get("trace", [])
                roles = {point.get("role") for point in trace}
                if not has_lifetime_trace(roles):
                    raise ValueError(
                        f"{finding.get('rule_id')} has no allocation/release trace"
                    )
            rule_id = finding.get("rule_id")
            if rule_id in observed_trace_roles:
                observed_primary_lines[rule_id].append(
                    int(finding.get("location", {}).get("line", 0))
                )
                observed_trace_roles[rule_id].update(
                    point.get("role") for point in finding.get("trace", [])
                )
                for point in finding.get("trace", []):
                    role = point.get("role")
                    if not isinstance(role, str):
                        continue
                    observed_trace_role_lines[rule_id].setdefault(
                        role, []
                    ).append(int(point.get("line", 0)))
        for rule_id, required_roles in dict(
            case.get("expect_trace_roles", {})
        ).items():
            missing = set(required_roles) - observed_trace_roles.get(
                rule_id, set()
            )
            if missing:
                raise ValueError(
                    f"{rule_id} missing trace roles: {sorted(missing)}"
                )
        for rule_id, expected_lines in dict(
            case.get("expect_primary_lines", {})
        ).items():
            actual_lines = sorted(observed_primary_lines.get(rule_id, []))
            wanted_lines = sorted(int(line) for line in expected_lines)
            if actual_lines != wanted_lines:
                raise ValueError(
                    f"{rule_id} primary lines {actual_lines} != {wanted_lines}"
                )
        for rule_id, expected_roles in dict(
            case.get("expect_trace_role_lines", {})
        ).items():
            actual_roles = observed_trace_role_lines.get(rule_id, {})
            for role, expected_lines in dict(expected_roles).items():
                actual_lines = sorted(actual_roles.get(role, []))
                wanted_lines = sorted(int(line) for line in expected_lines)
                if actual_lines != wanted_lines:
                    raise ValueError(
                        f"{rule_id} trace role {role!r} lines "
                        f"{actual_lines} != {wanted_lines}"
                    )
        if document.get("findings") and expect_html:
            rendered_report = html_report.read_text(encoding="utf-8")
            if "Suggested repair (manual):" not in rendered_report:
                raise ValueError("HTML report has no manual repair guidance")
    except (OSError, ValueError, KeyError, TypeError, json.JSONDecodeError) as error:
        report_error = str(error)
    passed = (
        not timed_out
        and exit_code == 0
        and actual == ({code: 0 for code in FINDING_CODES}
                       if str(plugin_options.get("reportonly", "0")) == "1"
                       else expected)
        and report_counts == expected
        and report_inconclusive == expected_inconclusive_counts(case)
        and report_inconclusive_reasons
        == {
            str(reason): int(count)
            for reason, count in dict(
                case.get("expect_inconclusive_reasons", {})
            ).items()
        }
        and report_error is None
        and html_report.is_file() == expect_html
    )
    return {
        "name": name,
        "source": str(source.relative_to(tests_root)),
        "language": language,
        "compiler": compiler,
        "status": "PASS" if passed else "FAIL",
        "exit_code": exit_code,
        "timed_out": timed_out,
        "expected": expected,
        "actual": actual,
        "report_actual": report_counts,
        "expected_inconclusive": expected_inconclusive_counts(case),
        "report_inconclusive": report_inconclusive,
        "report_inconclusive_reasons": report_inconclusive_reasons,
        "report_error": report_error,
        "duration_ms": duration_ms,
        "compiler_log": str(stderr_file),
        "json_report": str(json_report),
        "html_report": str(html_report),
        "html_expected": expect_html,
        "command": command,
    }


def compact_counts(counts: dict[str, int]) -> str:
    present = [f"{code}={count}" for code, count in counts.items() if count]
    return ", ".join(present) if present else "none"


def write_html_report(path: Path, summary: dict[str, object]) -> None:
    rows = []
    for result in summary["cases"]:
        status_class = "pass" if result["status"] == "PASS" else "fail"
        report_key = "html_report" if result["html_expected"] else "json_report"
        case_report = Path(result[report_key]).resolve().relative_to(
            path.parent.resolve()
        ).as_posix()
        rows.append(
            "<tr>"
            f"<td class='{status_class}'>{html.escape(str(result['status']))}</td>"
            f"<td>{html.escape(str(result['name']))}</td>"
            f"<td>{html.escape(compact_counts(result['actual']))}</td>"
            f"<td>{result['duration_ms']:.2f} ms</td>"
            f"<td><a href='{html.escape(case_report)}'>"
            f"{'HTML' if result['html_expected'] else 'JSON'}</a></td>"
            "</tr>"
        )
    document = f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Static Analyzer Unit Tests</title>
<style>body{{font:15px system-ui,sans-serif;background:#f5f7fa;color:#172033;margin:0}}main{{max-width:1100px;margin:40px auto;padding:0 24px}}.summary{{font-size:24px;margin:20px 0}}table{{width:100%;border-collapse:collapse;background:white;border:1px solid #dce2ea}}th,td{{padding:12px;text-align:left;border-bottom:1px solid #e5e9ef}}th{{background:#eef2f7}}.pass{{color:#177245;font-weight:700}}.fail{{color:#b42336;font-weight:700}}a{{color:#185abc}}</style></head>
<body><main><h1>Static Analyzer Unit Tests</h1><div class="summary">{summary['passed']} passed · {summary['failed']} failed</div>
<table><thead><tr><th>Status</th><th>Case</th><th>Findings</th><th>Time</th><th>Report</th></tr></thead><tbody>
{''.join(rows)}</tbody></table></main></body></html>
"""
    path.write_text(document, encoding="utf-8")


def run_cases(args: argparse.Namespace, tests_root: Path,
              cases: list[dict[str, object]], on_result=None) -> list[dict[str, object]]:
    # Validate isolation before any worker removes stale files. Different
    # manifest names must not collapse onto the same output directory.
    directories = set()
    output = args.output.resolve()
    for case in cases:
        directory = (output / str(case["name"]).replace("/", "__")).resolve()
        if directory == output or output not in directory.parents:
            raise ValueError(f"case output escapes its root: {case['name']}")
        if directory in directories:
            raise ValueError(f"case output directory collision: {case['name']}")
        directories.add(directory)
    if args.jobs < 1:
        raise ValueError("--jobs must be positive")
    if not math.isfinite(args.timeout) or args.timeout <= 0:
        raise ValueError("--timeout must be finite and positive")
    results = [None] * len(cases)
    with ThreadPoolExecutor(max_workers=min(args.jobs, len(cases) or 1)) as pool:
        pending = {pool.submit(run_case, args, tests_root, case): index
                   for index, case in enumerate(cases)}
        for future in as_completed(pending):
            result = future.result()
            results[pending[future]] = result
            if on_result is not None:
                on_result(result)
    # Completion/progress order can vary; JSON and HTML keep manifest order.
    return results


def print_case_result(result: dict[str, object], width: int) -> None:
    detail = compact_counts(result["actual"])
    print(f"{result['status']:<4}  {result['name']:<{width}}  {detail}", flush=True)
    if result["status"] == "FAIL":
        print(f"      expected: {compact_counts(result['expected'])}")
        if result["report_error"]:
            print(f"      report: {result['report_error']}")
        if result["report_inconclusive"] != result["expected_inconclusive"]:
            print(
                "      inconclusive expected/actual: "
                f"{compact_counts(result['expected_inconclusive'])} / "
                f"{compact_counts(result['report_inconclusive'])}"
            )
        print(f"      log: {result['compiler_log']}", flush=True)


def main() -> int:
    args = parse_args()
    tests_root = args.manifest.resolve().parent
    args.output.mkdir(parents=True, exist_ok=True)
    cases = json.loads(args.manifest.read_text(encoding="utf-8"))

    width = max((len(str(case["name"])) for case in cases), default=0)
    started = time.monotonic()
    results = run_cases(args, tests_root, cases,
                        lambda result: print_case_result(result, width))
    wall_ms = round((time.monotonic() - started) * 1000, 2)
    passed = sum(result["status"] == "PASS" for result in results)
    failed = len(results) - passed
    summary = {
        "schema_version": 1,
        "compiler": args.compiler,
        "cxx_compiler": args.cxx_compiler or infer_cxx_compiler(args.compiler),
        "plugin": str(args.plugin.resolve()),
        "passed": passed,
        "failed": failed,
        "total": len(results),
        "execution": {"jobs": min(args.jobs, len(cases)), "wall_ms": wall_ms,
                      "timing_scope": "whole unit run; concurrent case timings are not serial speed measurements"},
        "cases": results,
    }
    report_path = args.output / "report.json"
    report_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    html_path = args.output / "report.html"
    write_html_report(html_path, summary)

    print(f"\n{passed} passed, {failed} failed; HTML: {html_path}; JSON: {report_path}")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
