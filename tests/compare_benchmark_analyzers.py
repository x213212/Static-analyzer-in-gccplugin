#!/usr/bin/env python3
"""Compare the GCC plugin and GCC 16.2 ``-fanalyzer`` on legacy corpora.

Plain GCC, the plugin, and ``-fanalyzer`` are separate compiler processes for
every translation unit.  All three use the same GCC 16.2 language driver and
common compilation flags.  Their execution order is deterministically rotated,
and timing headlines use only translation units for which all three processes
completed successfully.

The extracted MemFix-derived models are a positive-only target corpus.  Their
directory category supplies the target warning (SA001/SA002/SA003), while the
reviewed historical allocation-site specifications localize the intended
object.  A same-rule warning elsewhere is not credited.  The runner therefore
reports localized recall but deliberately does not invent precision.  Juliet
is reported only as translation-unit coverage, warnings, and configured
compiler-process cost here; the separate compare_juliet_oracle.py runner uses
grouped bad/good ground truth.
"""

from __future__ import annotations

import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor, as_completed
from datetime import datetime, timezone
import hashlib
import html
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import signal
import statistics
import subprocess
import sys
import time
from typing import Any, Iterable, Sequence
from urllib.parse import unquote, urlparse

try:
    from .benchmark_manifest import (
        BenchmarkCase,
        EXTRACTED_FAMILIES,
        JULIET_CWES,
        REPOSITORY_ROOT,
        discover_cases,
    )
    from .extracted_oracle import (
        DEFAULT_ORACLE,
        ExtractedExpectation,
        ExtractedOracleError,
        evidence_matches_expectation,
        load_extracted_oracle,
        validate_extracted_oracle,
    )
except ImportError:  # Direct execution: python3 tests/compare_benchmark_analyzers.py
    from benchmark_manifest import (
        BenchmarkCase,
        EXTRACTED_FAMILIES,
        JULIET_CWES,
        REPOSITORY_ROOT,
        discover_cases,
    )
    from extracted_oracle import (
        DEFAULT_ORACLE,
        ExtractedExpectation,
        ExtractedOracleError,
        evidence_matches_expectation,
        load_extracted_oracle,
        validate_extracted_oracle,
    )


TOOL_STATUSES = ("OK", "ERROR", "TIMEOUT", "CRASH")
PLUGIN_WARNING_CODES = ("SA001", "SA002", "SA003", "SA004")
PLUGIN_WARNING_RE = re.compile(r"\[(SA\d{3})\]")
CRASH_RE = re.compile(
    r"internal compiler error|segmentation fault|please submit a full bug report|"
    r"fatal signal|killed signal terminated program|stack dump|bus error",
    re.IGNORECASE,
)
ERROR_LINE_RE = re.compile(
    r"^.*(?:fatal error:|error:|internal compiler error:|segmentation fault).*$",
    re.IGNORECASE | re.MULTILINE,
)

GCC_RULE_TO_PLUGIN_WARNING = {
    "-Wanalyzer-malloc-leak": "SA001",
    "-Wanalyzer-double-free": "SA002",
    "-Wanalyzer-use-after-free": "SA003",
}
EXTRACTED_TARGET_WARNING = {
    "leak": "SA001",
    "double_free": "SA002",
    "use_after_free": "SA003",
}

PLUGIN_OPTIONS = (
    "doublefreecheck=1",
    "memoryleakcheck=1",
    "reporthtml=0",
    "threadmod=0",
    "useafterfree=1",
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
# Compatibility alias for report readers and imports written against schema 1.
# There are no longer plugin-only compilation flags: every measured mode uses
# this exact tuple before its mode-specific switches are appended.
PLUGIN_COMPILE_FLAGS = COMMON_FLAGS
GCC_ANALYZER_FLAGS = (
    "-fanalyzer",
    "-fanalyzer-verbosity=0",
    "-fdiagnostics-format=sarif-stderr",
)

# Rotate all measured modes instead of permanently giving a mode the cold or
# warm process position.  The order is stable for a selected translation unit,
# so reports remain auditable and repeatable in one checkout.
TIMED_MODE_ORDERS = (
    ("baseline", "plugin", "gcc16_fanalyzer"),
    ("baseline", "gcc16_fanalyzer", "plugin"),
    ("plugin", "baseline", "gcc16_fanalyzer"),
    ("plugin", "gcc16_fanalyzer", "baseline"),
    ("gcc16_fanalyzer", "baseline", "plugin"),
    ("gcc16_fanalyzer", "plugin", "baseline"),
)


class ConfigurationError(ValueError):
    """An error that must be fixed before any corpus case can be run."""


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Compare plain GCC, a matching-ABI GCC plugin, and GCC 16.2 "
            "-fanalyzer over the extracted and/or Juliet corpora. The three "
            "processes share common flags, rotate through six deterministic "
            "orders, and report three-way-OK configured cost plus incremental "
            "cost over the plain baseline."
        ),
        epilog=(
            "Reports include runner, plugin, oracle, and selected-source hashes. "
            "Extracted results are allocation-site-localized positive-only recall; "
            "precision and false-positive rate are undefined."
        ),
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
        help="limit extracted cases to a project family; repeatable",
    )
    parser.add_argument(
        "--cwe",
        action="append",
        choices=JULIET_CWES,
        help="limit Juliet cases to a CWE; repeatable",
    )
    parser.add_argument(
        "--match",
        help="only run repository-relative source paths containing this text",
    )
    parser.add_argument(
        "--limit",
        type=int,
        help="run at most this many cases after deterministic filtering",
    )
    parser.add_argument(
        "--cc",
        "--gcc16-cc",
        dest="cc",
        default=os.environ.get("GCC16_CC", "gcc-16"),
        help="GCC 16.2 C driver (default: GCC16_CC or gcc-16)",
    )
    parser.add_argument(
        "--cxx",
        "--gcc16-cxx",
        dest="cxx",
        default=os.environ.get("GCC16_CXX", "g++-16"),
        help="GCC 16.2 C++ driver (default: GCC16_CXX or g++-16)",
    )
    parser.add_argument(
        "--plugin",
        type=Path,
        default=REPOSITORY_ROOT / "Compiler" / "newplugin.so",
        help="GCC 16.2 build of the plugin (default: Compiler/newplugin.so)",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=REPOSITORY_ROOT / "artifacts" / "gcc16-corpus-comparison",
    )
    parser.add_argument(
        "--extracted-oracle",
        type=Path,
        default=DEFAULT_ORACLE,
        help=(
            "reviewed extracted allocation-site oracle; fixture/hash drift is "
            "a configuration error"
        ),
    )
    parser.add_argument("--jobs", type=int, default=min(8, os.cpu_count() or 1))
    parser.add_argument("--timeout", type=float, default=60.0)
    parser.add_argument(
        "--verbose", action="store_true", help="print one line per case"
    )
    return parser.parse_args(argv)


def selected_cases(
    args: argparse.Namespace,
) -> tuple[list[BenchmarkCase], dict[str, Any]]:
    if args.jobs < 1:
        raise ConfigurationError("--jobs must be at least 1")
    if args.timeout <= 0:
        raise ConfigurationError("--timeout must be positive")
    if args.limit is not None and args.limit < 1:
        raise ConfigurationError("--limit must be at least 1")

    suites = args.suite or ["extracted"]
    families = args.family or list(EXTRACTED_FAMILIES)
    cwes = args.cwe or list(JULIET_CWES)
    try:
        cases = discover_cases(suites, families, cwes)
    except ValueError as error:
        raise ConfigurationError(str(error)) from error

    if args.match:
        cases = [
            case
            for case in cases
            if args.match in case.repository_relative_source.as_posix()
        ]
    if args.limit is not None:
        cases = cases[: args.limit]
    if not cases:
        raise ConfigurationError("no benchmark cases matched the selection")

    selection = {
        "suites": suites,
        "families": families,
        "cwes": cwes,
        "match": args.match,
        "limit": args.limit,
    }
    return cases, selection


def resolve_executable(candidate: str) -> str:
    executable = shutil.which(candidate)
    if executable is None:
        raise ConfigurationError(f"compiler not found or not executable: {candidate}")
    return str(Path(executable).resolve())


def short_probe(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    try:
        return subprocess.run(
            command,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=15.0,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        raise ConfigurationError(
            f"could not probe compiler {command[0]!r}: {error}"
        ) from error


def compiler_identity(executable: str) -> dict[str, str]:
    version_probe = short_probe([executable, "-dumpfullversion", "-dumpversion"])
    if version_probe.returncode != 0:
        detail = version_probe.stderr.strip() or (
            f"version probe exited {version_probe.returncode}"
        )
        raise ConfigurationError(f"cannot determine version of {executable}: {detail}")
    words = version_probe.stdout.strip().split()
    if not words:
        raise ConfigurationError(
            f"compiler version probe returned no output: {executable}"
        )
    version = words[0]
    # GCC packages normally return 16.2.0 here.  Permit a vendor suffix such
    # as 16.2.0-ubuntu1, but do not silently turn a 16.1 or 16.3 run into a
    # report labelled "GCC 16.2".
    if (
        re.fullmatch(r"16\.2\.\d+(?:\.\d+)*(?:[-+~][A-Za-z0-9._+~-]+)?", version)
        is None
    ):
        raise ConfigurationError(
            f"GCC 16.2.x is required, but {executable} reports version {version}"
        )

    banner_probe = short_probe([executable, "--version"])
    banner_lines = (banner_probe.stdout or banner_probe.stderr).splitlines()
    target_probe = short_probe([executable, "-dumpmachine"])
    target = target_probe.stdout.strip() if target_probe.returncode == 0 else "unknown"
    return {
        "executable": executable,
        "version": version,
        "banner": banner_lines[0] if banner_lines else "unknown",
        "target": target,
    }


def validate_toolchains(
    args: argparse.Namespace, cases: Sequence[BenchmarkCase]
) -> tuple[dict[str, str], dict[str, dict[str, str]]]:
    languages = {case.language for case in cases}
    compilers: dict[str, str] = {}
    identities: dict[str, dict[str, str]] = {}
    if "c" in languages:
        compilers["c"] = resolve_executable(args.cc)
        identities["c"] = compiler_identity(compilers["c"])
    if "c++" in languages:
        compilers["c++"] = resolve_executable(args.cxx)
        identities["c++"] = compiler_identity(compilers["c++"])

    if len(identities) == 2:
        c_identity = identities["c"]
        cxx_identity = identities["c++"]
        if c_identity["version"] != cxx_identity["version"]:
            raise ConfigurationError(
                "C and C++ drivers must be the same GCC 16.2 release: "
                f"{c_identity['version']} != {cxx_identity['version']}"
            )
        if c_identity["target"] != cxx_identity["target"]:
            raise ConfigurationError(
                "C and C++ drivers must target the same platform: "
                f"{c_identity['target']} != {cxx_identity['target']}"
            )
    return compilers, identities


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def source_selection_artifact(cases: Sequence[BenchmarkCase]) -> dict[str, Any]:
    """Fingerprint the ordered source selection and each source's contents."""

    digest = hashlib.sha256()
    entries: list[dict[str, str]] = []
    for case in sorted(cases, key=lambda item: item.case_id):
        source_hash = sha256_file(case.source)
        source = case.repository_relative_source.as_posix()
        entries.append(
            {"case_id": case.case_id, "source": source, "sha256": source_hash}
        )
        digest.update(case.case_id.encode("utf-8"))
        digest.update(b"\0")
        digest.update(source.encode("utf-8"))
        digest.update(b"\0")
        digest.update(source_hash.encode("ascii"))
        digest.update(b"\n")
    return {
        "case_count": len(entries),
        "sha256": digest.hexdigest(),
        "policy": "SHA-256 over sorted case id, repository-relative source, and source SHA-256",
        "sources": entries,
    }


def extracted_oracle_selection_artifact(
    cases: Sequence[BenchmarkCase],
    expectations: dict[str, ExtractedExpectation],
) -> dict[str, Any]:
    """Fingerprint the localized oracle slice used by selected extracted cases."""

    entries: list[dict[str, Any]] = []
    for case in sorted(cases, key=lambda item: item.case_id):
        if case.suite != "extracted":
            continue
        source = case.repository_relative_source.as_posix()
        expectation = expectations[source]
        entries.append(
            {
                "source": source,
                "source_sha256": expectation.source_sha256,
                "category": expectation.category,
                "target_rule": expectation.target_rule,
                "allocation_lines": sorted(expectation.target_lines),
            }
        )
    canonical = json.dumps(
        entries, sort_keys=True, separators=(",", ":"), ensure_ascii=False
    ).encode("utf-8")
    return {
        "case_count": len(entries),
        "sha256": hashlib.sha256(canonical).hexdigest(),
        "policy": (
            "SHA-256 of canonical selected source/hash/category/rule/allocation-lines"
        ),
        "cases": entries,
    }


def verify_input_provenance_unchanged(
    cases: Sequence[BenchmarkCase],
    initial_sources: dict[str, Any],
    oracle_path: Path,
    initial_oracle_sha256: str,
) -> dict[str, Any]:
    """Reject a run whose source set or oracle changed after setup."""

    final_sources = source_selection_artifact(cases)
    final_oracle_sha256 = sha256_file(oracle_path)
    changed: list[str] = []
    if final_sources["sha256"] != initial_sources["sha256"]:
        changed.append("selected source set")
    if final_oracle_sha256 != initial_oracle_sha256:
        changed.append("extracted oracle")
    if changed:
        raise ConfigurationError(
            "benchmark input drift detected after execution ("
            + ", ".join(changed)
            + "); no valid comparison report was written"
        )
    return {
        "verified_after_run": True,
        "selected_source_set_sha256": final_sources["sha256"],
        "extracted_oracle_sha256": final_oracle_sha256,
    }


def snapshot_plugin(plugin: Path, output: Path) -> tuple[Path, str]:
    original = plugin.expanduser().resolve()
    if not original.is_file():
        raise ConfigurationError(f"plugin does not exist: {original}")
    digest = sha256_file(original)
    snapshot_dir = output / "plugin-snapshot" / digest[:12]
    snapshot_dir.mkdir(parents=True, exist_ok=True)
    snapshot = snapshot_dir / original.name
    if not snapshot.exists() or sha256_file(snapshot) != digest:
        temporary = snapshot.with_name(f".{snapshot.name}.{os.getpid()}.tmp")
        shutil.copy2(original, temporary)
        temporary.replace(snapshot)
    if sha256_file(snapshot) != digest:
        raise ConfigurationError(f"plugin snapshot hash mismatch: {snapshot}")
    return snapshot.resolve(), digest


def plugin_name(plugin: Path) -> str:
    return plugin.stem


def plugin_arguments(plugin: Path, report_stem: Path | None = None) -> list[str]:
    name = plugin_name(plugin)
    arguments = [f"-fplugin-arg-{name}-{option}" for option in PLUGIN_OPTIONS]
    if report_stem is None:
        arguments.append(f"-fplugin-arg-{name}-report=0")
    else:
        arguments.extend(
            (
                f"-fplugin-arg-{name}-report=1",
                f"-fplugin-arg-{name}-reportpath={report_stem.resolve()}",
            )
        )
    return arguments


def standard_for(language: str) -> str:
    return "-std=gnu11" if language == "c" else "-std=gnu++14"


def classify_execution(returncode: int | None, stderr: str, timed_out: bool) -> str:
    if timed_out:
        return "TIMEOUT"
    if returncode is not None and returncode < 0:
        return "CRASH"
    if CRASH_RE.search(stderr):
        return "CRASH"
    if returncode != 0:
        return "ERROR"
    return "OK"


def run_process(command: Sequence[str], cwd: Path, timeout: float) -> dict[str, Any]:
    started = time.monotonic()
    try:
        process = subprocess.Popen(
            list(command),
            cwd=cwd,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            start_new_session=True,
        )
    except OSError as error:
        duration_ms = round((time.monotonic() - started) * 1000, 2)
        return {
            "status": "ERROR",
            "exit_code": None,
            "timed_out": False,
            "duration_ms": duration_ms,
            "stdout": "",
            "stderr": f"failed to start compiler: {error}\n",
        }

    timed_out = False
    try:
        stdout, stderr = process.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        timed_out = True
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        stdout, stderr = process.communicate()
    duration_ms = round((time.monotonic() - started) * 1000, 2)
    return {
        "status": classify_execution(process.returncode, stderr, timed_out),
        "exit_code": process.returncode,
        "timed_out": timed_out,
        "duration_ms": duration_ms,
        "stdout": stdout,
        "stderr": stderr,
    }


def diagnostic_summary(stderr: str) -> str | None:
    match = ERROR_LINE_RE.search(stderr)
    if match:
        return match.group(0).strip()[:500]
    stripped = stderr.strip()
    return stripped[:500] if stripped else None


def write_compiler_log(
    path: Path, command: Sequence[str], stdout: str, stderr: str
) -> None:
    content = "$ " + shlex.join(command) + "\n"
    if stdout:
        content += "\n[stdout]\n" + stdout
    if stderr:
        content += "\n[stderr]\n" + stderr
    path.write_text(content, encoding="utf-8", errors="replace")


def preflight_plugin(
    compilers: dict[str, str], plugin: Path, output: Path, timeout: float
) -> dict[str, Any]:
    """Actually load and run the plugin in each selected GCC 16.2 frontend."""

    preflight_dir = output / "preflight"
    preflight_dir.mkdir(parents=True, exist_ok=True)
    checks: dict[str, Any] = {}
    for language, compiler in sorted(compilers.items()):
        suffix = ".c" if language == "c" else ".cpp"
        label = "c" if language == "c" else "cxx"
        source = preflight_dir / f"plugin_abi_probe{suffix}"
        object_file = preflight_dir / f"plugin_abi_probe_{label}.o"
        log = preflight_dir / f"plugin_abi_probe_{label}.log"
        source.write_text(
            "int gcc_plugin_abi_probe(void) { return 0; }\n", encoding="utf-8"
        )
        object_file.unlink(missing_ok=True)
        command = [
            compiler,
            standard_for(language),
            f"-fplugin={plugin}",
            *plugin_arguments(plugin),
            *COMMON_FLAGS,
            "-c",
            str(source.resolve()),
            "-o",
            str(object_file.resolve()),
        ]
        execution = run_process(command, preflight_dir, min(timeout, 30.0))
        write_compiler_log(
            log, command, execution.pop("stdout"), execution.pop("stderr")
        )
        execution.update(
            {
                "command": command,
                "log": str(log.resolve()),
                "compiler": compiler,
            }
        )
        checks[language] = execution
        object_file.unlink(missing_ok=True)
        if execution["status"] != "OK":
            detail = diagnostic_summary(
                log.read_text(encoding="utf-8", errors="replace")
            )
            raise ConfigurationError(
                "plugin ABI/load preflight failed with the selected GCC 16.2 "
                f"{language} driver ({execution['status']}): {detail or 'see log'}; "
                f"log: {log.resolve()}"
            )
    return checks


def case_directory(output: Path, case: BenchmarkCase) -> Path:
    digest = hashlib.sha256(case.case_id.encode("utf-8")).hexdigest()[:12]
    readable = re.sub(r"[^A-Za-z0-9_.-]+", "_", case.source.stem)[:72]
    return output / "cases" / case.suite / case.group / f"{readable}-{digest}"


def base_compile_command(
    compiler: str, case: BenchmarkCase
) -> list[str]:
    command = [compiler, standard_for(case.language), *COMMON_FLAGS]
    for include_dir in case.include_dirs:
        command.extend(("-I", str(include_dir.resolve())))
    return command


def plugin_warning_counts(stderr: str) -> dict[str, int]:
    counts = Counter({code: 0 for code in PLUGIN_WARNING_CODES})
    counts.update(PLUGIN_WARNING_RE.findall(stderr))
    return dict(sorted(counts.items()))


def diagnostic_site(file_value: Any, line_value: Any, role: str) -> dict[str, Any] | None:
    if not isinstance(file_value, str):
        return None
    try:
        line = int(line_value)
    except (TypeError, ValueError):
        return None
    if line < 1:
        return None
    return {"file": file_value, "line": line, "role": role}


def deduplicate_evidence(
    evidence: Iterable[dict[str, Any]],
) -> list[dict[str, Any]]:
    result: list[dict[str, Any]] = []
    seen: set[tuple[str, int, str]] = set()
    for point in evidence:
        key = (
            str(point.get("file", "")),
            int(point.get("line", 0)),
            str(point.get("role", "")),
        )
        if key not in seen:
            seen.add(key)
            result.append(point)
    return result


def parse_plugin_report(
    path: Path,
) -> tuple[list[dict[str, Any]], dict[str, int], str | None]:
    if not path.is_file():
        return [], {}, "plugin JSON report was not created"
    try:
        document = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        return [], {}, f"invalid plugin JSON report: {error}"
    raw_findings = document.get("findings") if isinstance(document, dict) else None
    raw_summary = document.get("summary") if isinstance(document, dict) else None
    if not isinstance(raw_findings, list) or not isinstance(raw_summary, dict):
        return [], {}, "plugin JSON report must contain findings and summary"

    findings: list[dict[str, Any]] = []
    for raw in raw_findings:
        if not isinstance(raw, dict) or not isinstance(raw.get("rule_id"), str):
            continue
        location = raw.get("location")
        primary = (
            diagnostic_site(location.get("file"), location.get("line"), "primary")
            if isinstance(location, dict)
            else None
        )
        evidence: list[dict[str, Any]] = []
        if primary is not None:
            evidence.append(primary)
        trace = raw.get("trace")
        if isinstance(trace, list):
            for point in trace:
                if not isinstance(point, dict):
                    continue
                trace_site = diagnostic_site(
                    point.get("file"),
                    point.get("line"),
                    f"trace:{point.get('role', 'unspecified')}",
                )
                if trace_site is not None:
                    evidence.append(trace_site)
        findings.append(
            {
                "rule_id": raw["rule_id"],
                "message": raw.get("message"),
                "primary": primary,
                "evidence": deduplicate_evidence(evidence),
                "origin": "plugin-json",
            }
        )

    counts: dict[str, int] = {}
    for code in PLUGIN_WARNING_CODES:
        value = raw_summary.get(code, 0)
        if not isinstance(value, int) or isinstance(value, bool) or value < 0:
            return findings, {}, f"invalid plugin report count for {code}"
        counts[code] = value
    return findings, counts, None


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


def normalize_artifact_uri(value: Any) -> str:
    if not isinstance(value, str):
        return ""
    parsed = urlparse(value)
    return unquote(parsed.path if parsed.scheme == "file" else value)


def sarif_location(location: Any, role: str) -> dict[str, Any] | None:
    if not isinstance(location, dict):
        return None
    physical = location.get("physicalLocation")
    if not isinstance(physical, dict):
        return None
    artifact = physical.get("artifactLocation")
    region = physical.get("region")
    if not isinstance(artifact, dict) or not isinstance(region, dict):
        return None
    return diagnostic_site(
        normalize_artifact_uri(artifact.get("uri")), region.get("startLine"), role
    )


def parse_gcc_sarif(
    text: str,
) -> tuple[
    dict[str, int],
    dict[str, int],
    dict[str, int],
    list[dict[str, Any]],
    str | None,
]:
    mapped: Counter[str] = Counter(
        {warning: 0 for warning in GCC_RULE_TO_PLUGIN_WARNING.values()}
    )
    analyzer_rules: Counter[str] = Counter(
        {rule: 0 for rule in GCC_RULE_TO_PLUGIN_WARNING}
    )
    all_rules: Counter[str] = Counter()
    try:
        document = json.loads(text)
    except json.JSONDecodeError as error:
        return dict(mapped), dict(analyzer_rules), {}, [], f"invalid SARIF JSON: {error}"
    if not isinstance(document, dict) or not isinstance(document.get("runs"), list):
        return (
            dict(mapped),
            dict(analyzer_rules),
            {},
            [],
            "SARIF root must contain a runs array",
        )

    findings: list[dict[str, Any]] = []
    for result in iter_sarif_results(document):
        rule_id = result.get("ruleId")
        if not isinstance(rule_id, str):
            continue
        all_rules[rule_id] += 1
        if rule_id.startswith("-Wanalyzer-"):
            analyzer_rules[rule_id] += 1
        warning = GCC_RULE_TO_PLUGIN_WARNING.get(rule_id)
        if warning is not None:
            mapped[warning] += 1
        if not rule_id.startswith("-Wanalyzer-"):
            continue
        evidence: list[dict[str, Any]] = []
        primary: dict[str, Any] | None = None
        locations = result.get("locations", [])
        if isinstance(locations, list):
            for index, location in enumerate(locations):
                point = sarif_location(
                    location, "primary" if index == 0 else "related"
                )
                if point is not None:
                    if primary is None:
                        primary = point
                    evidence.append(point)
        related = result.get("relatedLocations", [])
        if isinstance(related, list):
            for location in related:
                point = sarif_location(location, "related")
                if point is not None:
                    evidence.append(point)
        code_flows = result.get("codeFlows", [])
        if isinstance(code_flows, list):
            for code_flow in code_flows:
                if not isinstance(code_flow, dict):
                    continue
                thread_flows = code_flow.get("threadFlows", [])
                if not isinstance(thread_flows, list):
                    continue
                for thread_flow in thread_flows:
                    if not isinstance(thread_flow, dict):
                        continue
                    locations = thread_flow.get("locations", [])
                    if not isinstance(locations, list):
                        continue
                    for flow_location in locations:
                        if not isinstance(flow_location, dict):
                            continue
                        point = sarif_location(
                            flow_location.get("location"), "codeflow"
                        )
                        if point is not None:
                            evidence.append(point)
        message = result.get("message")
        findings.append(
            {
                "rule_id": warning,
                "native_rule_id": rule_id,
                "message": message.get("text") if isinstance(message, dict) else None,
                "primary": primary,
                "evidence": deduplicate_evidence(evidence),
                "origin": "gcc-sarif",
            }
        )
    return (
        dict(sorted(mapped.items())),
        dict(sorted(analyzer_rules.items())),
        dict(sorted(all_rules.items())),
        findings,
        None,
    )


def finalize_tool_result(
    execution: dict[str, Any], command: list[str], log: Path
) -> tuple[dict[str, Any], str]:
    stdout = str(execution.pop("stdout"))
    stderr = str(execution.pop("stderr"))
    write_compiler_log(log, command, stdout, stderr)
    result = {
        **execution,
        "crash_signature": bool(CRASH_RE.search(stderr)),
        "diagnostic": (
            diagnostic_summary(stderr) if execution["status"] != "OK" else None
        ),
        "command": command,
        "log": str(log.resolve()),
    }
    return result, stderr


def run_baseline_case(
    compiler: str, case: BenchmarkCase, case_dir: Path, timeout: float
) -> dict[str, Any]:
    run_dir = case_dir / "baseline"
    run_dir.mkdir(parents=True, exist_ok=True)
    object_file = run_dir / "case.o"
    log = run_dir / "compiler.log"
    object_file.unlink(missing_ok=True)
    command = base_compile_command(compiler, case)
    command.extend(("-c", str(case.source.resolve()), "-o", str(object_file.resolve())))
    execution = run_process(command, run_dir, timeout)
    result, _ = finalize_tool_result(execution, command, log)
    object_file.unlink(missing_ok=True)
    return result


def run_plugin_case(
    compiler: str,
    plugin: Path,
    case: BenchmarkCase,
    case_dir: Path,
    timeout: float,
) -> dict[str, Any]:
    run_dir = case_dir / "plugin"
    run_dir.mkdir(parents=True, exist_ok=True)
    object_file = run_dir / "case.o"
    log = run_dir / "compiler.log"
    report_stem = run_dir / "analysis"
    report_json = report_stem.with_suffix(".json")
    report_html = report_stem.with_suffix(".html")
    object_file.unlink(missing_ok=True)
    report_json.unlink(missing_ok=True)
    report_html.unlink(missing_ok=True)
    command = base_compile_command(compiler, case)
    command.extend(
        (f"-fplugin={plugin}", *plugin_arguments(plugin, report_stem))
    )
    command.extend(("-c", str(case.source.resolve()), "-o", str(object_file.resolve())))
    execution = run_process(command, run_dir, timeout)
    result, stderr = finalize_tool_result(execution, command, log)
    warnings = plugin_warning_counts(stderr)
    findings, report_counts, report_error = parse_plugin_report(report_json)
    if result["status"] == "OK" and report_error is not None:
        result["status"] = "ERROR"
        result["diagnostic"] = report_error
    elif result["status"] == "OK" and report_counts != warnings:
        result["status"] = "ERROR"
        result["diagnostic"] = (
            "plugin JSON/stderr warning counts disagree: "
            f"json={report_counts}, stderr={warnings}"
        )
    result.update(
        {
            "warning_counts": warnings,
            "total_warnings": sum(warnings.values()),
            "findings": findings,
            "report_parse_error": report_error,
            "json_report": str(report_json.resolve()),
            "html_report": str(report_html.resolve()) if report_html.is_file() else None,
        }
    )
    object_file.unlink(missing_ok=True)
    return result


def run_gcc_analyzer_case(
    compiler: str, case: BenchmarkCase, case_dir: Path, timeout: float
) -> dict[str, Any]:
    run_dir = case_dir / "gcc16-fanalyzer"
    run_dir.mkdir(parents=True, exist_ok=True)
    object_file = run_dir / "case.o"
    log = run_dir / "compiler.log"
    sarif = run_dir / "diagnostics.sarif"
    object_file.unlink(missing_ok=True)
    sarif.unlink(missing_ok=True)
    command = base_compile_command(compiler, case)
    command.extend(GCC_ANALYZER_FLAGS)
    command.extend(("-c", str(case.source.resolve()), "-o", str(object_file.resolve())))
    execution = run_process(command, run_dir, timeout)
    result, stderr = finalize_tool_result(execution, command, log)
    sarif.write_text(stderr, encoding="utf-8", errors="replace")
    mapped, analyzer_rules, all_rules, findings, parse_error = parse_gcc_sarif(stderr)
    if result["status"] == "OK" and parse_error is not None:
        result["status"] = "ERROR"
        result["diagnostic"] = parse_error
    result.update(
        {
            "warning_counts": mapped,
            "total_warnings": sum(mapped.values()),
            "analyzer_rule_ids": analyzer_rules,
            "all_rule_ids": all_rules,
            "findings": findings,
            "sarif_parse_error": parse_error,
            "sarif": str(sarif.resolve()),
        }
    )
    object_file.unlink(missing_ok=True)
    return result


def internal_tool_error(tool_dir: Path, error: BaseException) -> dict[str, Any]:
    tool_dir.mkdir(parents=True, exist_ok=True)
    log = tool_dir / "internal-error.log"
    log.write_text(f"runner error: {type(error).__name__}: {error}\n", encoding="utf-8")
    result: dict[str, Any] = {
        "status": "ERROR",
        "exit_code": None,
        "timed_out": False,
        "duration_ms": 0.0,
        "crash_signature": False,
        "diagnostic": f"runner error: {type(error).__name__}: {error}",
        "command": [],
        "log": str(log.resolve()),
        "warning_counts": {},
        "total_warnings": 0,
        "findings": [],
    }
    return result


def timed_mode_order(
    case: BenchmarkCase, rotation_index: int | None = None
) -> list[str]:
    """Return a deterministic, path-independent measured-mode permutation.

    The main runner supplies the case's index in the sorted selected corpus,
    giving exact round-robin counterbalancing (counts differ by at most one).
    The case-id hash fallback keeps direct/library callers deterministic.
    """

    if rotation_index is None:
        rotation_index = int.from_bytes(
            hashlib.sha256(case.case_id.encode("utf-8")).digest()[:8], "big"
        )
    return list(TIMED_MODE_ORDERS[rotation_index % len(TIMED_MODE_ORDERS)])


def run_case(
    args: argparse.Namespace,
    case: BenchmarkCase,
    compilers: dict[str, str],
    plugin: Path,
    extracted_expectations: dict[str, ExtractedExpectation],
    rotation_index: int | None = None,
) -> dict[str, Any]:
    case_dir = case_directory(args.output, case)
    case_dir.mkdir(parents=True, exist_ok=True)
    compiler = compilers[case.language]
    source_sha256 = sha256_file(case.source)
    execution_order = timed_mode_order(case, rotation_index)
    tool_results: dict[str, dict[str, Any]] = {}

    # Keep all attempts independent: a baseline, plugin, or native failure must
    # not suppress either peer.  The rotating order makes the resulting process
    # durations suitable for a three-way common-paired configured-cost view.
    for mode in execution_order:
        try:
            if mode == "baseline":
                tool_results[mode] = run_baseline_case(
                    compiler, case, case_dir, args.timeout
                )
            elif mode == "plugin":
                tool_results[mode] = run_plugin_case(
                    compiler, plugin, case, case_dir, args.timeout
                )
            else:
                tool_results[mode] = run_gcc_analyzer_case(
                    compiler, case, case_dir, args.timeout
                )
        except Exception as error:  # Keep a 4,980-case run progressing.
            directory_name = "gcc16-fanalyzer" if mode == "gcc16_fanalyzer" else mode
            failed = internal_tool_error(case_dir / directory_name, error)
            if mode == "gcc16_fanalyzer":
                failed.update(
                    {
                        "analyzer_rule_ids": {},
                        "all_rule_ids": {},
                        "sarif_parse_error": f"runner error: {error}",
                        "sarif": None,
                    }
                )
            tool_results[mode] = failed

    baseline_result = tool_results["baseline"]
    plugin_result = tool_results["plugin"]
    gcc_result = tool_results["gcc16_fanalyzer"]

    target_warning = (
        EXTRACTED_TARGET_WARNING.get(case.category)
        if case.suite == "extracted"
        else None
    )
    target = None
    if target_warning is not None:
        source_name = case.repository_relative_source.as_posix()
        expectation = extracted_expectations[source_name]

        def localized(tool_result: dict[str, Any]) -> bool:
            return tool_result["status"] == "OK" and any(
                finding.get("rule_id") == target_warning
                and evidence_matches_expectation(
                    finding.get("evidence", []), expectation
                )
                for finding in tool_result.get("findings", [])
            )

        target = {
            "positive_only": True,
            "oracle": "historical reviewed allocation sites",
            "category": case.category,
            "warning": target_warning,
            "allocation_sites": [
                {
                    "line": site.line,
                    "original_line": site.original_line,
                    "allocator": site.allocator,
                    "deallocator": site.deallocator,
                    "mapping": site.mapping,
                    "source_text": site.source_text,
                }
                for site in expectation.allocation_sites
            ],
            "plugin_warning_emitted": (
                plugin_result["status"] == "OK"
                and int(plugin_result["warning_counts"].get(target_warning, 0)) > 0
            ),
            "gcc16_fanalyzer_warning_emitted": (
                gcc_result["status"] == "OK"
                and int(gcc_result["warning_counts"].get(target_warning, 0)) > 0
            ),
            "plugin_detected": localized(plugin_result),
            "gcc16_fanalyzer_detected": localized(gcc_result),
        }

    if sha256_file(case.source) != source_sha256:
        raise ConfigurationError(
            f"source changed while its three measured modes ran: {case.source}"
        )

    return {
        "id": case.case_id,
        "suite": case.suite,
        "group": case.group,
        "category": case.category,
        "source": case.repository_relative_source.as_posix(),
        "source_sha256": source_sha256,
        "language": case.language,
        "timed_execution_order": execution_order,
        "target": target,
        "baseline": baseline_result,
        "plugin": plugin_result,
        "gcc16_fanalyzer": gcc_result,
    }


def status_counts(results: Sequence[dict[str, Any]], tool: str) -> dict[str, int]:
    counts = Counter(result[tool]["status"] for result in results)
    return {status: counts[status] for status in TOOL_STATUSES}


def safe_rate(numerator: int | float, denominator: int | float) -> float | None:
    return round(numerator / denominator, 6) if denominator else None


def duration_summary(values: Sequence[float]) -> dict[str, float | int | None]:
    if not values:
        return {
            "count": 0,
            "total_ms": 0.0,
            "mean_ms": None,
            "median_ms": None,
            "p95_ms": None,
            "max_ms": None,
        }
    ordered = sorted(values)
    p95_index = min(
        len(ordered) - 1,
        max(0, int(len(ordered) * 0.95 + 0.999999) - 1),
    )
    return {
        "count": len(values),
        "total_ms": round(sum(values), 2),
        "mean_ms": round(statistics.fmean(values), 2),
        "median_ms": round(statistics.median(values), 2),
        "p95_ms": round(ordered[p95_index], 2),
        "max_ms": round(ordered[-1], 2),
    }


def tool_duration_summary(
    results: Sequence[dict[str, Any]], tool: str, only_ok: bool = False
) -> dict[str, float | int | None]:
    return duration_summary(
        [
            float(result[tool]["duration_ms"])
            for result in results
            if not only_ok or result[tool]["status"] == "OK"
        ]
    )


def ratio(numerator: Any, denominator: Any) -> float | None:
    if numerator is None or denominator in (None, 0, 0.0):
        return None
    return round(float(numerator) / float(denominator), 4)


def analyzer_overhead_summary(
    baseline_values: Sequence[float], analyzer_values: Sequence[float]
) -> dict[str, Any]:
    """Summarize one analyzer against the same-TU plain GCC measurements."""

    if len(baseline_values) != len(analyzer_values):
        raise ValueError("baseline and analyzer timing vectors must be paired")
    deltas = [
        analyzer - baseline
        for baseline, analyzer in zip(baseline_values, analyzer_values)
    ]
    baseline_total = sum(baseline_values)
    analyzer_total = sum(analyzer_values)
    incremental_total = analyzer_total - baseline_total
    return {
        "pair_count": len(deltas),
        "baseline_total_ms": round(baseline_total, 2),
        "analyzer_total_ms": round(analyzer_total, 2),
        "incremental_total_ms": round(incremental_total, 2),
        "summed_duration_ratio": safe_rate(analyzer_total, baseline_total),
        "incremental_percent_of_baseline": (
            round(100.0 * incremental_total / baseline_total, 4)
            if baseline_total
            else None
        ),
        "incremental_ms": duration_summary(deltas),
        "analyzer_higher_cost_pairs": sum(delta > 0 for delta in deltas),
        "analyzer_lower_cost_pairs": sum(delta < 0 for delta in deltas),
        "equal_cost_pairs": sum(delta == 0 for delta in deltas),
    }


def paired_performance(results: Sequence[dict[str, Any]]) -> dict[str, Any]:
    pairs = [
        (
            float(result["baseline"]["duration_ms"]),
            float(result["plugin"]["duration_ms"]),
            float(result["gcc16_fanalyzer"]["duration_ms"]),
        )
        for result in results
        if result["baseline"]["status"] == "OK"
        and result["plugin"]["status"] == "OK"
        and result["gcc16_fanalyzer"]["status"] == "OK"
    ]
    baseline_values = [pair[0] for pair in pairs]
    plugin_values = [pair[1] for pair in pairs]
    gcc_values = [pair[2] for pair in pairs]
    baseline = duration_summary(baseline_values)
    plugin = duration_summary(plugin_values)
    gcc = duration_summary(gcc_values)
    ratios = {
        "direction": "gcc16_fanalyzer_duration / plugin_duration",
        "interpretation": (
            "configured compiler-process cost only: greater than 1 means the "
            "plugin mode took less wall time; less than 1 means GCC 16.2 "
            "-fanalyzer mode took less wall time"
        ),
    }
    for field, label in (
        ("total_ms", "total"),
        ("mean_ms", "mean"),
        ("median_ms", "median"),
        ("p95_ms", "p95"),
        ("max_ms", "max"),
    ):
        ratios[f"gcc16_over_plugin_{label}"] = ratio(gcc[field], plugin[field])
    return {
        "scope": (
            "translation units for which plain GCC, plugin, and GCC 16.2 "
            "-fanalyzer processes were all OK"
        ),
        "case_count": len(pairs),
        "pair_count": len(pairs),
        "baseline": baseline,
        "plugin": plugin,
        "gcc16_fanalyzer": gcc,
        "plugin_over_baseline": analyzer_overhead_summary(
            baseline_values, plugin_values
        ),
        "gcc16_fanalyzer_over_baseline": analyzer_overhead_summary(
            baseline_values, gcc_values
        ),
        "configured_cost_ratios": ratios,
        # Backward-compatible name.  The interpretation now explicitly limits
        # this to end-to-end configured compiler-process cost.
        "speed_ratios": ratios,
    }


def performance_report(results: Sequence[dict[str, Any]]) -> dict[str, Any]:
    by_suite: dict[str, Any] = {}
    for suite in ("extracted", "juliet"):
        subset = [result for result in results if result["suite"] == suite]
        if subset:
            paired = paired_performance(subset)
            by_suite[suite] = {
                "baseline": tool_duration_summary(subset, "baseline"),
                "plugin": tool_duration_summary(subset, "plugin"),
                "gcc16_fanalyzer": tool_duration_summary(subset, "gcc16_fanalyzer"),
                "three_way_ok": paired,
                "paired_ok": paired,
            }
    headline = paired_performance(results)
    return {
        "measurement": (
            "end-to-end elapsed wall-clock configured compiler-process cost; plain "
            "GCC, plugin, and native analyzer use identical common compilation "
            "flags and a deterministically rotated execution order; totals sum "
            "per-process durations and are not parallel-run wall time"
        ),
        "headline_scope": "three-way OK translation units",
        "headline_population": "three_way_common_paired_ok",
        "headline_is_configured_process_cost": True,
        "headline": headline,
        "baseline": tool_duration_summary(results, "baseline"),
        "plugin": tool_duration_summary(results, "plugin"),
        "gcc16_fanalyzer": tool_duration_summary(results, "gcc16_fanalyzer"),
        "ok_only": {
            "baseline": tool_duration_summary(results, "baseline", only_ok=True),
            "plugin": tool_duration_summary(results, "plugin", only_ok=True),
            "gcc16_fanalyzer": tool_duration_summary(
                results, "gcc16_fanalyzer", only_ok=True
            ),
        },
        "three_way_ok": headline,
        # Backward-compatible alias; it now means a three-way baseline/plugin/
        # native common pair rather than a two-analyzer pair.
        "paired_ok": headline,
        "by_suite": by_suite,
    }


def warning_distribution(
    results: Sequence[dict[str, Any]], tool: str
) -> dict[str, Any]:
    warning_codes = (
        GCC_RULE_TO_PLUGIN_WARNING.values()
        if tool == "gcc16_fanalyzer"
        else PLUGIN_WARNING_CODES
    )
    warnings: Counter[str] = Counter({code: 0 for code in warning_codes})
    cases_with_warning: Counter[str] = Counter()
    analyzer_rules: Counter[str] = Counter(
        {rule: 0 for rule in GCC_RULE_TO_PLUGIN_WARNING}
        if tool == "gcc16_fanalyzer"
        else {}
    )
    cases_with_analyzer_rule: Counter[str] = Counter()
    for result in results:
        run = result[tool]
        warning_counts = run.get("warning_counts", {})
        warnings.update(warning_counts)
        cases_with_warning.update(
            code for code, count in warning_counts.items() if int(count) > 0
        )
        if tool == "gcc16_fanalyzer":
            rules = run.get("analyzer_rule_ids", {})
            analyzer_rules.update(rules)
            cases_with_analyzer_rule.update(
                rule for rule, count in rules.items() if int(count) > 0
            )
    distribution: dict[str, Any] = {
        "scope": "diagnostics emitted by all attempts, regardless of execution status",
        "mapped_warning_counts": dict(sorted(warnings.items())),
        "cases_with_mapped_warning": dict(sorted(cases_with_warning.items())),
        "total_mapped_warnings": sum(warnings.values()),
    }
    if tool == "gcc16_fanalyzer":
        distribution.update(
            {
                "analyzer_rule_ids": dict(sorted(analyzer_rules.items())),
                "cases_with_analyzer_rule": dict(
                    sorted(cases_with_analyzer_rule.items())
                ),
                "total_analyzer_rule_results": sum(analyzer_rules.values()),
            }
        )
    return distribution


def target_recall(results: Sequence[dict[str, Any]], tool: str) -> dict[str, Any]:
    by_category: dict[str, Any] = {}
    aggregate = {
        "targets": 0,
        "analyzed_ok": 0,
        "not_analyzed": 0,
        "detected": 0,
        "warning_emitted_anywhere": 0,
        "unlocalized_warning_cases": 0,
        "undetected": 0,
        "no_warning_on_ok": 0,
    }
    for category, warning in EXTRACTED_TARGET_WARNING.items():
        subset = [
            result
            for result in results
            if result["suite"] == "extracted" and result["category"] == category
        ]
        analyzed_ok = sum(result[tool]["status"] == "OK" for result in subset)
        detected = sum(
            result[tool]["status"] == "OK"
            and result.get("target") is not None
            and bool(result["target"].get(f"{tool}_detected"))
            for result in subset
        )
        warning_emitted = sum(
            result[tool]["status"] == "OK"
            and result.get("target") is not None
            and bool(result["target"].get(f"{tool}_warning_emitted"))
            for result in subset
        )
        counts = {
            "target_warning": warning,
            "targets": len(subset),
            "analyzed_ok": analyzed_ok,
            "not_analyzed": len(subset) - analyzed_ok,
            "detected": detected,
            "warning_emitted_anywhere": warning_emitted,
            "unlocalized_warning_cases": warning_emitted - detected,
            "undetected": len(subset) - detected,
            "no_warning_on_ok": analyzed_ok - warning_emitted,
            "target_recall": safe_rate(detected, len(subset)),
            "recall_among_ok": safe_rate(detected, analyzed_ok),
            "precision": None,
        }
        by_category[category] = counts
        for key in aggregate:
            aggregate[key] += int(counts[key])
    aggregate.update(
        {
            "target_recall": safe_rate(aggregate["detected"], aggregate["targets"]),
            "recall_among_ok": safe_rate(
                aggregate["detected"], aggregate["analyzed_ok"]
            ),
            "precision": None,
        }
    )
    return {"aggregate": aggregate, "by_category": by_category}


def coverage(results: Sequence[dict[str, Any]], tool: str) -> dict[str, Any]:
    statuses = status_counts(results, tool)
    return {
        "translation_units": len(results),
        "analyzed_ok": statuses["OK"],
        "coverage_rate": safe_rate(statuses["OK"], len(results)),
        "statuses": statuses,
    }


def juliet_report(results: Sequence[dict[str, Any]]) -> dict[str, Any]:
    juliet = [result for result in results if result["suite"] == "juliet"]

    def tool_view(subset: Sequence[dict[str, Any]], tool: str) -> dict[str, Any]:
        configured_cost = tool_duration_summary(subset, tool)
        return {
            "coverage": coverage(subset, tool),
            "warnings": warning_distribution(subset, tool),
            "configured_process_cost": configured_cost,
            # Schema-1 compatibility alias; this is end-to-end process cost.
            "speed": configured_cost,
        }

    by_cwe: dict[str, Any] = {}
    for cwe in sorted({result["group"] for result in juliet}):
        subset = [result for result in juliet if result["group"] == cwe]
        three_way_cwe = paired_performance(subset)
        by_cwe[cwe] = {
            "translation_units": len(subset),
            "baseline": tool_view(subset, "baseline"),
            "plugin": tool_view(subset, "plugin"),
            "gcc16_fanalyzer": tool_view(subset, "gcc16_fanalyzer"),
            "three_way_ok_configured_cost": three_way_cwe,
            "three_way_ok_speed": three_way_cwe,
            "paired_ok_speed": three_way_cwe,
        }
    three_way_ok = paired_performance(juliet)
    return {
        "scope": (
            "translation-unit coverage, emitted warnings, and configured compiler-"
            "process cost only"
        ),
        "ground_truth_warning": (
            "Juliet good/bad programs can span multiple translation units; a "
            "translation-unit filename is not treated as ground truth, so no "
            "precision, recall, or accuracy is reported for Juliet"
        ),
        "translation_units": len(juliet),
        "baseline": tool_view(juliet, "baseline"),
        "plugin": tool_view(juliet, "plugin"),
        "gcc16_fanalyzer": tool_view(juliet, "gcc16_fanalyzer"),
        "three_way_ok_configured_cost": three_way_ok,
        "three_way_ok_speed": three_way_ok,
        # Schema-1 compatibility alias.  It now has the stricter three-way-OK
        # population and configured-cost interpretation.
        "paired_ok_speed": three_way_ok,
        "by_cwe": by_cwe,
    }


def count_text(counts: dict[str, Any]) -> str:
    return (
        ", ".join(
            f"{key}={value}" for key, value in sorted(counts.items()) if int(value) != 0
        )
        or "none"
    )


def log_href(report_path: Path, log_value: str) -> str:
    log = Path(log_value)
    try:
        return log.relative_to(report_path.parent).as_posix()
    except ValueError:
        return log.as_uri()


def display_number(value: Any, digits: int = 2) -> str:
    if value is None:
        return "n/a"
    if isinstance(value, float):
        return f"{value:.{digits}f}"
    return str(value)


def speed_statement(ratio_value: float | None) -> str:
    if ratio_value is None:
        return "No three-way-OK configured-cost multiplier is available."
    if ratio_value == 0:
        return (
            "GCC 16.2 -fanalyzer's summed configured process duration rounded "
            "to zero; an inverse multiplier is unavailable."
        )
    if ratio_value > 1:
        return (
            f"The GCC 16.2 -fanalyzer configured process cost was "
            f"{ratio_value:.2f}× the plugin configured process cost."
        )
    if ratio_value < 1:
        return (
            f"The plugin configured process cost was {1.0 / ratio_value:.2f}× "
            "the GCC 16.2 -fanalyzer configured process cost."
        )
    return "The summed three-way-paired configured process durations are equal."


def write_html_report(path: Path, report: dict[str, Any]) -> None:
    esc = lambda value: html.escape(str(value), quote=True)
    status_rows: list[str] = []
    for status in TOOL_STATUSES:
        status_rows.append(
            "<tr>"
            f"<th>{esc(status)}</th>"
            f"<td>{report['summary']['baseline_statuses'][status]}</td>"
            f"<td>{report['summary']['plugin_statuses'][status]}</td>"
            f"<td>{report['summary']['gcc16_fanalyzer_statuses'][status]}</td>"
            "</tr>"
        )

    performance = report["performance"]["headline"]
    perf_rows: list[str] = []
    for label, key in (
        ("Plain GCC baseline", "baseline"),
        ("Plugin", "plugin"),
        ("GCC 16.2 -fanalyzer", "gcc16_fanalyzer"),
    ):
        item = performance[key]
        perf_rows.append(
            "<tr>"
            f"<th>{esc(label)}</th><td>{item['count']}</td>"
            f"<td>{display_number(item['total_ms'])}</td>"
            f"<td>{display_number(item['mean_ms'])}</td>"
            f"<td>{display_number(item['median_ms'])}</td>"
            f"<td>{display_number(item['p95_ms'])}</td>"
            f"<td>{display_number(item['max_ms'])}</td>"
            "</tr>"
        )

    suite_speed_rows: list[str] = []
    for suite, suite_performance in sorted(report["performance"]["by_suite"].items()):
        paired = suite_performance["three_way_ok"]
        suite_speed_rows.append(
            "<tr>"
            f"<th>{esc(suite)}</th><td>{paired['case_count']}</td>"
            f"<td>{display_number(paired['baseline']['total_ms'])}</td>"
            f"<td>{display_number(paired['plugin']['total_ms'])}</td>"
            f"<td>{display_number(paired['gcc16_fanalyzer']['total_ms'])}</td>"
            f"<td>{display_number(paired['plugin_over_baseline']['summed_duration_ratio'], 4)}</td>"
            f"<td>{display_number(paired['gcc16_fanalyzer_over_baseline']['summed_duration_ratio'], 4)}</td>"
            f"<td>{display_number(paired['configured_cost_ratios']['gcc16_over_plugin_total'])}</td>"
            "</tr>"
        )

    overhead_rows: list[str] = []
    for label, key in (
        ("Plugin", "plugin_over_baseline"),
        ("GCC 16.2 -fanalyzer", "gcc16_fanalyzer_over_baseline"),
    ):
        overhead = performance[key]
        incremental_percent = overhead["incremental_percent_of_baseline"]
        incremental_percent_text = (
            f"{display_number(incremental_percent, 2)}%"
            if incremental_percent is not None
            else "n/a"
        )
        overhead_rows.append(
            "<tr>"
            f"<th>{esc(label)}</th><td>{overhead['pair_count']}</td>"
            f"<td>{display_number(overhead['baseline_total_ms'])}</td>"
            f"<td>{display_number(overhead['analyzer_total_ms'])}</td>"
            f"<td>{display_number(overhead['incremental_total_ms'])}</td>"
            f"<td>{display_number(overhead['summed_duration_ratio'], 4)}</td>"
            f"<td>{incremental_percent_text}</td>"
            f"<td>{overhead['analyzer_higher_cost_pairs']}/"
            f"{overhead['analyzer_lower_cost_pairs']}/"
            f"{overhead['equal_cost_pairs']}</td>"
            "</tr>"
        )

    accuracy_rows: list[str] = []
    accuracy = report["accuracy"]["extracted_positive_targets"]
    for tool, label in (
        ("plugin", "Plugin"),
        ("gcc16_fanalyzer", "GCC 16.2 -fanalyzer"),
    ):
        tool_accuracy = accuracy[tool]
        for category in (*EXTRACTED_TARGET_WARNING.keys(), "aggregate"):
            item = (
                tool_accuracy["aggregate"]
                if category == "aggregate"
                else tool_accuracy["by_category"][category]
            )
            if not item["targets"]:
                continue
            accuracy_rows.append(
                "<tr>"
                f"<td>{esc(label)}</td><td>{esc(category)}</td>"
                f"<td>{item['targets']}</td><td>{item['analyzed_ok']}</td>"
                f"<td>{item['warning_emitted_anywhere']}</td>"
                f"<td>{item['detected']}</td>"
                f"<td>{item['unlocalized_warning_cases']}</td>"
                f"<td>{display_number(item['target_recall'], 4)}</td>"
                "<td>undefined (positive-only)</td></tr>"
            )

    diagnostic_rows: list[str] = []
    plugin_diagnostics = report["diagnostics"]["plugin"]
    gcc_diagnostics = report["diagnostics"]["gcc16_fanalyzer"]
    diagnostic_rows.append(
        "<tr><th>Plugin SA warnings</th>"
        f"<td><code>{esc(count_text(plugin_diagnostics['mapped_warning_counts']))}</code></td>"
        f"<td>{plugin_diagnostics['total_mapped_warnings']}</td></tr>"
    )
    diagnostic_rows.append(
        "<tr><th>GCC mapped SA warnings</th>"
        f"<td><code>{esc(count_text(gcc_diagnostics['mapped_warning_counts']))}</code></td>"
        f"<td>{gcc_diagnostics['total_mapped_warnings']}</td></tr>"
    )
    diagnostic_rows.append(
        "<tr><th>GCC analyzer ruleIds</th>"
        f"<td><code>{esc(count_text(gcc_diagnostics['analyzer_rule_ids']))}</code></td>"
        f"<td>{gcc_diagnostics['total_analyzer_rule_results']}</td></tr>"
    )

    case_rows: list[str] = []
    for result in report["cases"]:
        baseline = result["baseline"]
        plugin = result["plugin"]
        gcc = result["gcc16_fanalyzer"]
        baseline_link = log_href(path, baseline["log"])
        plugin_link = log_href(path, plugin["log"])
        gcc_link = log_href(path, gcc["log"])
        gcc_rules = count_text(gcc.get("analyzer_rule_ids", {}))
        target = result.get("target")
        if target is None:
            plugin_target = gcc_target = "n/a"
        else:
            plugin_target = (
                "localized"
                if target["plugin_detected"]
                else "unlocalized"
                if target["plugin_warning_emitted"]
                else "not emitted"
            )
            gcc_target = (
                "localized"
                if target["gcc16_fanalyzer_detected"]
                else "unlocalized"
                if target["gcc16_fanalyzer_warning_emitted"]
                else "not emitted"
            )
        case_rows.append(
            "<tr>"
            f"<td><code>{esc(' → '.join(result['timed_execution_order']))}</code></td>"
            f"<td class='{esc(baseline['status'].lower())}'>{esc(baseline['status'])}</td>"
            f"<td class='{esc(plugin['status'].lower())}'>{esc(plugin['status'])}</td>"
            f"<td class='{esc(gcc['status'].lower())}'>{esc(gcc['status'])}</td>"
            f"<td>{esc(result['suite'])}</td><td>{esc(result['group'])}</td>"
            f"<td><code>{esc(result['source'])}</code></td>"
            f"<td><code>{esc(count_text(plugin['warning_counts']))}</code></td>"
            f"<td><code>{esc(gcc_rules)}</code></td>"
            f"<td>{esc(plugin_target)}</td><td>{esc(gcc_target)}</td>"
            f"<td>{baseline['duration_ms']:.2f}</td>"
            f"<td>{plugin['duration_ms']:.2f}</td><td>{gcc['duration_ms']:.2f}</td>"
            f"<td><a href='{esc(baseline_link)}'>baseline</a> · "
            f"<a href='{esc(plugin_link)}'>plugin</a> · "
            f"<a href='{esc(gcc_link)}'>GCC</a></td>"
            "</tr>"
        )

    compiler_lines = "<br>".join(
        f"{esc(language)}: {esc(identity['banner'])} [{esc(identity['executable'])}]"
        for language, identity in sorted(report["toolchains"]["drivers"].items())
    )
    ratio_value = performance["configured_cost_ratios"][
        "gcc16_over_plugin_total"
    ]
    plugin_baseline_ratio = performance["plugin_over_baseline"][
        "summed_duration_ratio"
    ]
    gcc_baseline_ratio = performance["gcc16_fanalyzer_over_baseline"][
        "summed_duration_ratio"
    ]
    juliet = report["juliet_translation_units"]
    juliet_text = (
        f"{juliet['translation_units']} selected translation units. "
        f"Plugin coverage: {display_number(juliet['plugin']['coverage']['coverage_rate'], 4)}; "
        f"GCC coverage: {display_number(juliet['gcc16_fanalyzer']['coverage']['coverage_rate'], 4)}."
        if juliet["translation_units"]
        else "Juliet was not selected in this run."
    )
    juliet_rows: list[str] = []
    if juliet["translation_units"]:
        for tool, label in (
            ("plugin", "Plugin"),
            ("gcc16_fanalyzer", "GCC 16.2 -fanalyzer"),
        ):
            item = juliet[tool]
            warning_counts = (
                item["warnings"].get("analyzer_rule_ids", {})
                if tool == "gcc16_fanalyzer"
                else item["warnings"]["mapped_warning_counts"]
            )
            configured_cost = item["configured_process_cost"]
            juliet_rows.append(
                "<tr>"
                f"<th>{esc(label)}</th>"
                f"<td>{item['coverage']['analyzed_ok']}/{item['coverage']['translation_units']} "
                f"({display_number(item['coverage']['coverage_rate'], 4)})</td>"
                f"<td><code>{esc(count_text(warning_counts))}</code></td>"
                f"<td>{display_number(configured_cost['total_ms'])}</td>"
                f"<td>{display_number(configured_cost['mean_ms'])}</td>"
                f"<td>{display_number(configured_cost['median_ms'])}</td>"
                f"<td>{display_number(configured_cost['p95_ms'])}</td>"
                f"<td>{display_number(configured_cost['max_ms'])}</td>"
                "</tr>"
            )

    document = f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>GCC 16.2 corpus analyzer comparison</title>
<style>
body{{font:14px system-ui,sans-serif;background:#f5f7fa;color:#172033;margin:0}}main{{max-width:1500px;margin:32px auto;padding:0 20px}}h1,h2{{margin-top:28px}}.meta,.notice,.card{{background:#fff;border:1px solid #dce2ea;border-radius:9px;padding:14px}}.notice{{border-left:5px solid #b7791f;margin:14px 0}}.cards{{display:flex;gap:12px;flex-wrap:wrap;margin:18px 0}}.card{{min-width:170px}}.number{{font-size:24px;font-weight:700}}table{{width:100%;border-collapse:collapse;background:#fff;border:1px solid #dce2ea}}th,td{{padding:9px;text-align:left;border-bottom:1px solid #e5e9ef;vertical-align:top}}thead th{{position:sticky;top:0;background:#eef2f7}}code{{font-size:12px;overflow-wrap:anywhere}}.ok{{color:#177245;font-weight:700}}.error,.timeout,.crash{{color:#b42336;font-weight:700}}a{{color:#185abc}}
</style></head><body><main>
<h1>GCC 16.2 corpus analyzer comparison</h1>
<div class="meta">Generated: {esc(report['generated_at'])}<br>{compiler_lines}<br>Jobs: {report['configuration']['jobs']}<br>Parallel-run wall duration: {display_number(report['summary']['parallel_run_wall_duration_ms'])} ms<br>Timed-order histogram: <code>{esc(count_text(report['summary']['timed_execution_order_counts']))}</code><br>Plugin SHA-256: <code>{esc(report['plugin_artifact']['sha256'])}</code><br>Runner SHA-256: <code>{esc(report['runner_artifact']['sha256'])}</code><br>Extracted oracle SHA-256: <code>{esc(report['extracted_oracle_artifact']['sha256'])}</code><br>Selected oracle slice SHA-256: <code>{esc(report['extracted_oracle_artifact']['selected']['sha256'])}</code><br>Selected-source-set SHA-256: <code>{esc(report['source_selection_artifact']['sha256'])}</code><br>Input drift check after run: {esc(report['input_provenance_verification']['verified_after_run'])}<br>Every source used independent plain GCC, plugin, and <code>-fanalyzer</code> compiler processes with the same language driver and identical common flags. Cases sorted by stable case id were assigned round-robin across all six permutations.</div>
<div class="cards"><div class="card"><div class="number">{report['summary']['total_cases']}</div>Translation units</div><div class="card"><div class="number">{performance['case_count']}</div>Three-way OK</div><div class="card"><div class="number">{display_number(plugin_baseline_ratio, 2)}×</div>Plugin/baseline configured cost</div><div class="card"><div class="number">{display_number(gcc_baseline_ratio, 2)}×</div>GCC analyzer/baseline configured cost</div><div class="card"><div class="number">{display_number(ratio_value, 2)}×</div>GCC analyzer/plugin configured cost</div></div>
<p><strong>{esc(speed_statement(ratio_value))}</strong></p>
<div class="notice">Timing is an end-to-end configured compiler-process cost, including compiler startup and each mode's required diagnostics/report work; it is not an isolated analyzer-algorithm benchmark. Only translation units where all three modes were OK enter the headline. With <code>--jobs &gt; 1</code>, concurrent compiler processes contend for host resources, so the summed process durations and parallel-run wall time answer different questions.</div>
<h2>Execution status</h2><table><thead><tr><th>Status</th><th>Plain GCC baseline</th><th>Plugin</th><th>GCC 16.2 -fanalyzer</th></tr></thead><tbody>{''.join(status_rows)}</tbody></table>
<h2>Warning and rule distribution</h2><table><thead><tr><th>Tool/view</th><th>Distribution</th><th>Total</th></tr></thead><tbody>{''.join(diagnostic_rows)}</tbody></table>
<h2>Three-way-OK configured compiler-process cost</h2><p>Times are milliseconds. Total is the sum of per-process elapsed times; it is not the parallel job wall time.</p><table><thead><tr><th>Mode</th><th>Count</th><th>Total</th><th>Mean</th><th>Median</th><th>P95</th><th>Max</th></tr></thead><tbody>{''.join(perf_rows)}</tbody></table>
<h2>Incremental configured cost over plain GCC</h2><table><thead><tr><th>Analyzer mode</th><th>Three-way pairs</th><th>Baseline total ms</th><th>Analyzer total ms</th><th>Incremental total ms</th><th>Analyzer/baseline</th><th>Incremental % of baseline</th><th>Higher/lower/equal-cost TUs</th></tr></thead><tbody>{''.join(overhead_rows)}</tbody></table>
<h2>Three-way-OK configured cost by suite</h2><table><thead><tr><th>Suite</th><th>Three-way pairs</th><th>Baseline total ms</th><th>Plugin total ms</th><th>GCC total ms</th><th>Plugin/baseline</th><th>GCC/baseline</th><th>GCC/plugin</th></tr></thead><tbody>{''.join(suite_speed_rows)}</tbody></table>
<h2>Extracted positive targets</h2><div class="notice">All selected extracted models are positive for the warning named by their category. A target is detected only when that rule's primary/trace/codeflow evidence reaches a reviewed historical allocation site; an unrelated same-rule warning elsewhere is shown as unlocalized and receives no credit. This supports target recall only: there are no negative examples here, so precision and false-positive rate are undefined.</div><table><thead><tr><th>Tool</th><th>Category</th><th>Targets</th><th>Analyzed OK</th><th>Warning anywhere</th><th>Localized detection</th><th>Unlocalized warning</th><th>Target recall</th><th>Precision</th></tr></thead><tbody>{''.join(accuracy_rows) or '<tr><td colspan="9">Extracted corpus not selected.</td></tr>'}</tbody></table>
<h2>Juliet translation units</h2><div class="notice">{esc(juliet['ground_truth_warning'])}</div><p>{esc(juliet_text)}</p><table><thead><tr><th>Tool</th><th>Coverage</th><th>Warnings / analyzer ruleIds</th><th>Total ms</th><th>Mean ms</th><th>Median ms</th><th>P95 ms</th><th>Max ms</th></tr></thead><tbody>{''.join(juliet_rows) or '<tr><td colspan="8">Juliet corpus not selected.</td></tr>'}</tbody></table>
<h2>Cases</h2><table><thead><tr><th>Timed order</th><th>Baseline</th><th>Plugin</th><th>GCC</th><th>Suite</th><th>Group</th><th>Source</th><th>Plugin warnings</th><th>GCC analyzer ruleIds</th><th>Plugin target</th><th>GCC target</th><th>Baseline ms</th><th>Plugin ms</th><th>GCC ms</th><th>Evidence</th></tr></thead><tbody>{''.join(case_rows)}</tbody></table>
</main></body></html>"""
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(document, encoding="utf-8")
    temporary.replace(path)


def print_summary(report: dict[str, Any], verbose: bool) -> None:
    if verbose:
        for result in report["cases"]:
            baseline = result["baseline"]
            plugin = result["plugin"]
            gcc = result["gcc16_fanalyzer"]
            print(
                f"{baseline['status']:<7} {plugin['status']:<7} "
                f"{gcc['status']:<7} {result['id']}  "
                f"order[{' > '.join(result['timed_execution_order'])}]  "
                f"plugin[{count_text(plugin['warning_counts'])}]  "
                f"gcc[{count_text(gcc.get('analyzer_rule_ids', {}))}]"
            )

    summary = report["summary"]
    print(f"\nCompared {summary['total_cases']} translation units")
    print(
        "Measured modes: plain GCC baseline, plugin, and GCC 16.2 -fanalyzer; "
        "identical common flags; sorted-index round-robin across six orders."
    )
    print(
        "Timed-order histogram: "
        f"{count_text(summary['timed_execution_order_counts'])}"
    )
    print(f"Plugin SHA-256: {report['plugin_artifact']['sha256']}")
    print(f"Runner SHA-256: {report['runner_artifact']['sha256']}")
    print(
        "Selected-source-set SHA-256: "
        f"{report['source_selection_artifact']['sha256']}"
    )
    print("Post-run source/oracle drift check: verified unchanged")
    print(f"Plain GCC baseline statuses: {count_text(summary['baseline_statuses'])}")
    print(f"Plugin statuses: {count_text(summary['plugin_statuses'])}")
    print(
        "GCC 16.2 -fanalyzer statuses: "
        f"{count_text(summary['gcc16_fanalyzer_statuses'])}"
    )
    extracted = report["accuracy"]["extracted_positive_targets"]
    for tool, label in (
        ("plugin", "Plugin"),
        ("gcc16_fanalyzer", "GCC 16.2 -fanalyzer"),
    ):
        aggregate = extracted[tool]["aggregate"]
        if aggregate["targets"]:
            print(
                f"{label} extracted allocation-site-localized target recall: "
                f"{aggregate['detected']}/{aggregate['targets']} "
                f"({display_number(aggregate['target_recall'], 4)}); "
                f"warning anywhere={aggregate['warning_emitted_anywhere']}, "
                f"unlocalized={aggregate['unlocalized_warning_cases']}; "
                "precision undefined (positive-only)"
            )
    paired = report["performance"]["headline"]
    speed_ratio = paired["configured_cost_ratios"]["gcc16_over_plugin_total"]
    plugin_overhead = paired["plugin_over_baseline"]
    gcc_overhead = paired["gcc16_fanalyzer_over_baseline"]
    print(
        "Three-way common-paired OK configured compiler-process cost: "
        f"{paired['case_count']} TUs; "
        f"plugin/baseline={display_number(plugin_overhead['summed_duration_ratio'], 4)}x "
        f"(increment {display_number(plugin_overhead['incremental_total_ms'])} ms); "
        f"GCC analyzer/baseline={display_number(gcc_overhead['summed_duration_ratio'], 4)}x "
        f"(increment {display_number(gcc_overhead['incremental_total_ms'])} ms)."
    )
    print(speed_statement(speed_ratio))
    print(
        "Timing note: configured end-to-end compiler-process cost; "
        f"jobs={report['configuration']['jobs']} may introduce host contention. "
        "Parallel-run wall time was "
        f"{display_number(summary['parallel_run_wall_duration_ms'])} ms and is not "
        "the sum of per-process durations."
    )


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    try:
        cases, selection = selected_cases(args)
        cases = sorted(cases, key=lambda case: case.case_id)
        selected_sources = source_selection_artifact(cases)
        oracle_path = args.extracted_oracle.expanduser().resolve()
        oracle_sha256 = sha256_file(oracle_path)
        extracted_expectations = load_extracted_oracle(args.extracted_oracle)
        if sha256_file(oracle_path) != oracle_sha256:
            raise ConfigurationError("extracted oracle changed while it was loaded")
        validate_extracted_oracle(cases, extracted_expectations)
        if source_selection_artifact(cases)["sha256"] != selected_sources["sha256"]:
            raise ConfigurationError("selected source set changed during setup")
        selected_oracle = extracted_oracle_selection_artifact(
            cases, extracted_expectations
        )
        compilers, identities = validate_toolchains(args, cases)
        args.output = args.output.expanduser().resolve()
        args.output.mkdir(parents=True, exist_ok=True)
        original_plugin = args.plugin.expanduser().resolve()
        plugin, plugin_sha256 = snapshot_plugin(original_plugin, args.output)
        preflight = preflight_plugin(compilers, plugin, args.output, args.timeout)
        runner_path = Path(__file__).resolve()
        runner_sha256 = sha256_file(runner_path)
    except (ConfigurationError, ExtractedOracleError, OSError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    started = time.monotonic()
    results: list[dict[str, Any]] = []
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {
            executor.submit(
                run_case,
                args,
                case,
                compilers,
                plugin,
                extracted_expectations,
                rotation_index,
            ): (case, rotation_index)
            for rotation_index, case in enumerate(cases)
        }
        for future in as_completed(futures):
            case, rotation_index = futures[future]
            try:
                results.append(future.result())
            except ConfigurationError as error:
                for pending in futures:
                    pending.cancel()
                print(f"error: {error}", file=sys.stderr)
                return 2
            except Exception as error:
                # run_case already isolates all three modes; this is a final guard for
                # non-tool failures such as an unreadable source hash.
                case_dir = case_directory(args.output, case)
                baseline_error = internal_tool_error(case_dir / "baseline", error)
                plugin_error = internal_tool_error(case_dir / "plugin", error)
                gcc_error = internal_tool_error(case_dir / "gcc16-fanalyzer", error)
                gcc_error.update(
                    {
                        "analyzer_rule_ids": {},
                        "all_rule_ids": {},
                        "sarif_parse_error": f"runner error: {error}",
                        "sarif": None,
                    }
                )
                results.append(
                    {
                        "id": case.case_id,
                        "suite": case.suite,
                        "group": case.group,
                        "category": case.category,
                        "source": case.repository_relative_source.as_posix(),
                        "source_sha256": None,
                        "language": case.language,
                        "timed_execution_order": timed_mode_order(
                            case, rotation_index
                        ),
                        "target": None,
                        "baseline": baseline_error,
                        "plugin": plugin_error,
                        "gcc16_fanalyzer": gcc_error,
                    }
                )
    results.sort(key=lambda result: result["id"])
    wall_duration_ms = round((time.monotonic() - started) * 1000, 2)
    try:
        provenance_verification = verify_input_provenance_unchanged(
            cases, selected_sources, oracle_path, oracle_sha256
        )
    except (ConfigurationError, OSError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    baseline_statuses = status_counts(results, "baseline")
    plugin_statuses = status_counts(results, "plugin")
    gcc_statuses = status_counts(results, "gcc16_fanalyzer")
    timed_order_counts = dict(
        sorted(
            Counter(
                " > ".join(result["timed_execution_order"])
                for result in results
            ).items()
        )
    )
    plugin_argument_name = plugin_name(plugin)
    reported_plugin_flags = [
        *COMMON_FLAGS,
        f"-fplugin={plugin}",
        *(
            f"-fplugin-arg-{plugin_argument_name}-{option}"
            for option in PLUGIN_OPTIONS
        ),
        f"-fplugin-arg-{plugin_argument_name}-report=1",
        f"-fplugin-arg-{plugin_argument_name}-reportpath=<per-case-analysis-stem>",
    ]
    report = {
        "schema_version": 2,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "repository": str(REPOSITORY_ROOT),
        "configuration": {
            **selection,
            "jobs": args.jobs,
            "timeout_seconds_per_process": args.timeout,
            # Schema-1 compatibility alias.  A case now has three processes.
            "timeout_seconds_per_tool_per_case": args.timeout,
            "common_flags": list(COMMON_FLAGS),
            "baseline_mode": "plain GCC compile: no -fplugin and no -fanalyzer",
            "plugin_options": list(PLUGIN_OPTIONS),
            # Schema-1 compatibility: these are now exactly COMMON_FLAGS.
            "plugin_compile_flags": list(PLUGIN_COMPILE_FLAGS),
            # Schema-1 compatibility: this remains the complete effective flag list.
            "gcc16_fanalyzer_flags": [*COMMON_FLAGS, *GCC_ANALYZER_FLAGS],
            "gcc16_fanalyzer_specific_flags": list(GCC_ANALYZER_FLAGS),
            "effective_mode_flags": {
                "baseline": list(COMMON_FLAGS),
                "plugin": reported_plugin_flags,
                "gcc16_fanalyzer": [*COMMON_FLAGS, *GCC_ANALYZER_FLAGS],
            },
            "timed_mode_order_policy": (
                "selected cases are sorted by case id, then assigned round-robin "
                "by index across all six permutations of baseline, plugin, and "
                "gcc16_fanalyzer; counts differ by at most one"
            ),
            "extracted_oracle": str(args.extracted_oracle.expanduser().resolve()),
        },
        "toolchains": {
            "required_full_version": "16.2.<patch> (optional vendor suffix)",
            "policy": (
                "plain baseline, plugin, and -fanalyzer use the same GCC 16.2 "
                "driver and identical common flags for each translation unit; "
                "C/C++ drivers must report the same release and target"
            ),
            "drivers": identities,
            "plugin_abi_preflight": preflight,
        },
        "plugin_artifact": {
            "original": str(original_plugin),
            "snapshot": str(plugin),
            "sha256": plugin_sha256,
        },
        "runner_artifact": {
            "path": str(runner_path),
            "sha256": runner_sha256,
        },
        "source_selection_artifact": selected_sources,
        "input_provenance_verification": provenance_verification,
        "extracted_oracle_artifact": {
            "path": str(oracle_path),
            "sha256": oracle_sha256,
            "case_count": len(extracted_expectations),
            "selected": selected_oracle,
            "localization_policy": (
                "target rule evidence must include a reviewed allocation-site line; "
                "same-rule warnings elsewhere receive no target-detection credit"
            ),
        },
        "diagnostic_mapping": {
            warning: rule for rule, warning in GCC_RULE_TO_PLUGIN_WARNING.items()
        },
        "summary": {
            "total_cases": len(results),
            "parallel_run_wall_duration_ms": wall_duration_ms,
            "timed_execution_order_counts": timed_order_counts,
            "baseline_statuses": baseline_statuses,
            "plugin_statuses": plugin_statuses,
            "gcc16_fanalyzer_statuses": gcc_statuses,
        },
        "diagnostics": {
            "plugin": warning_distribution(results, "plugin"),
            "gcc16_fanalyzer": warning_distribution(results, "gcc16_fanalyzer"),
        },
        "accuracy": {
            "extracted_positive_targets": {
                "scope": (
                    "every selected extracted leak/double_free/use_after_free "
                    "model is a positive target for SA001/SA002/SA003 respectively; "
                    "credit requires primary/trace/codeflow evidence at a reviewed "
                    "allocation site from the historical case specification"
                ),
                "warning": (
                    "positive-only corpus: target recall is defined, but precision, "
                    "specificity, false-positive rate, and binary accuracy are not; "
                    "same-rule warnings outside the oracle sites are unlocalized, "
                    "not credited detections"
                ),
                "plugin": target_recall(results, "plugin"),
                "gcc16_fanalyzer": target_recall(results, "gcc16_fanalyzer"),
            }
        },
        "juliet_translation_units": juliet_report(results),
        "performance": performance_report(results),
        "cases": results,
    }

    json_path = args.output / "comparison.json"
    temporary_json = json_path.with_suffix(".json.tmp")
    temporary_json.write_text(
        json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    temporary_json.replace(json_path)
    html_path = args.output / "comparison.html"
    write_html_report(html_path, report)

    print_summary(report, args.verbose)
    print(f"HTML: {html_path}")
    print(f"JSON: {json_path}")
    has_execution_failure = any(
        status_counts_map[status]
        for status_counts_map in (baseline_statuses, plugin_statuses, gcc_statuses)
        for status in ("ERROR", "TIMEOUT", "CRASH")
    )
    return 1 if has_execution_failure else 0


if __name__ == "__main__":
    sys.exit(main())
