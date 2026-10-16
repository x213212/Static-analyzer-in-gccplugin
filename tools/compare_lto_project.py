#!/usr/bin/env python3
"""Compare a real, explicitly supplied source project at GCC's LTO boundary.

This deliberately restrictive first workflow is not a compile-database driver.
All modes separately compile the same C/C++ sources with the same flags, then
link with LTO partitioning disabled. Only the link receives analyzer flags.
No unity source, provider contracts, visibility changes or whole-program
assumptions are injected. The output executable/library is never run.

Use --compile-arg=-Ipath / --compile-arg=-DNAME=value and --link-arg=-lm;
each occurrence supplies exactly one argv token (no shell interpretation).
Supported extras are standard/include/macro options, PIC/PIE, pthread and
basic library/shared/executable link options. Arbitrary -Wl, response files,
compiler wrappers, alternate optimization/pass flags and relocatable links
are intentionally unsupported. Select gcc or g++ explicitly with --cc.

Example:
  python3 tools/compare_lto_project.py --cc /opt/gcc/bin/gcc \
    --plugin Compiler/newplugin.so --output artifacts/project-lto \
    --source app.c --source provider.c --compile-arg=-Iinclude --link-arg=-lm

Each compiler process has a timeout; its whole process group is killed on
expiry, including LTO children. A new output directory is required. Findings
are observations, NOT TP/FP: no accuracy oracle is supplied. Single sequential
timings are exploratory and include compilation plus linking, not a controlled
speed ranking. Headers, libraries and compiler subprograms are not snapshotted;
the report explicitly limits its input-integrity claim to recorded files.
"""

from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import html
import json
import math
import os
from pathlib import Path
import re
import selectors
import shutil
import signal
import subprocess
import sys
import threading
import time
from typing import Any, Sequence


COMMON_FLAGS = (
    "-O1", "-fipa-pta", "-fno-inline", "-fno-tree-dce",
    "-fno-tree-dse", "-fno-tree-fre", "-flto", "-flto-partition=none",
    "-fdiagnostics-color=never",
)
MODES = ("baseline", "plugin", "gcc_fanalyzer")
RULES = ("SA001", "SA002", "SA003", "SA004")
MAX_REPORT_BYTES = 64 * 1024 * 1024
MAX_SOURCES = 4096


class ConfigurationError(ValueError):
    pass


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def validate_extra_args(arguments: Sequence[str], phase: str) -> list[str]:
    """An allowlist prevents analyzer/pass/output overrides through shared args."""
    allowed = {"-pthread", "-fPIC", "-fpic", "-fPIE", "-fpie", "-fno-pie"}
    if phase == "compile":
        allowed |= {"-Wall", "-Wextra", "-Wpedantic", "-pedantic"}
        pairs = {"-I", "-isystem", "-iquote", "-include", "-D", "-U"}
        prefixes = ("-I", "-D", "-U", "-std=")
    elif phase == "link":
        allowed |= {"-shared", "-static", "-pie", "-no-pie",
                    "-static-libgcc", "-static-libstdc++"}
        pairs = {"-L", "-l"}
        prefixes = ("-L", "-l")
    else:
        raise ConfigurationError(f"unknown argument phase: {phase}")
    index = 0
    while index < len(arguments):
        argument = arguments[index]
        if not argument or "\0" in argument or "\n" in argument:
            raise ConfigurationError(f"invalid {phase} argument")
        if argument in pairs:
            index += 1
            if index >= len(arguments) or not arguments[index] or arguments[index][0] in "-@":
                raise ConfigurationError(f"{argument} requires one ordinary operand")
            if "\0" in arguments[index] or "\n" in arguments[index]:
                raise ConfigurationError(f"invalid operand for {argument}")
        elif argument not in allowed and not any(
            argument.startswith(prefix) and len(argument) > len(prefix)
            for prefix in prefixes
        ):
            raise ConfigurationError(
                f"unsupported {phase} argument {argument!r}; this initial LTO "
                "workflow accepts only include/macro/standard, PIC/PIE, pthread "
                "and basic library/link-kind flags; analyzer, output and pass "
                "overrides are not allowed"
            )
        index += 1
    return list(arguments)


def plugin_arguments(plugin: Path, report_stem: Path) -> list[str]:
    options = (
        "analysispolicy=proof-only", "memoryleakcheck=1",
        "doublefreecheck=1", "useafterfree=1", "threadmod=0", "smt=1",
        "report=1", "reporthtml=1", f"reportpath={report_stem}",
    )
    return [f"-fplugin={plugin}", *[
        f"-fplugin-arg-{plugin.stem}-{option}" for option in options
    ]]


def build_commands(
    compiler: Path, sources: Sequence[Path], plugin: Path, mode_dir: Path,
    mode: str, compile_args: Sequence[str], link_args: Sequence[str],
) -> tuple[list[list[str]], list[str]]:
    if mode not in MODES:
        raise ConfigurationError(f"unknown mode: {mode}")
    compile_args = validate_extra_args(compile_args, "compile")
    link_args = validate_extra_args(link_args, "link")
    objects = [mode_dir / f"source-{index:04d}.o" for index in range(len(sources))]
    compile_commands = [
        [str(compiler), *COMMON_FLAGS, *compile_args, "-c", str(source), "-o", str(obj)]
        for source, obj in zip(sources, objects)
    ]
    analyzer_args: list[str] = []
    if mode == "plugin":
        analyzer_args = plugin_arguments(plugin, mode_dir / "analysis")
    elif mode == "gcc_fanalyzer":
        analyzer_args = ["-fanalyzer", "-fanalyzer-verbosity=0", "-fdiagnostics-format=sarif-stderr"]
    link_command = [
        str(compiler), *COMMON_FLAGS, *compile_args, *analyzer_args,
        *map(str, objects), *link_args, "-o", str(mode_dir / "project-output"),
    ]
    return compile_commands, link_command


def wait_process_notification(process: subprocess.Popen, timeout: float) -> int:
    """Non-reaping exit notification for Python builds without pidfd_open."""
    if not all(hasattr(os, name) for name in ("waitid", "P_PID", "WEXITED", "WNOWAIT")):
        return process.wait(timeout=timeout)
    deadline = time.monotonic() + timeout
    finished = threading.Event()
    errors: list[OSError] = []

    def observe_exit() -> None:
        try:
            os.waitid(os.P_PID, process.pid, os.WEXITED | os.WNOWAIT)
        except OSError as error:
            errors.append(error)
        finally:
            finished.set()

    observer = threading.Thread(target=observe_exit, daemon=True)
    observer.start()
    if not finished.wait(timeout):
        # The observer never reaps. The caller can kill/reap the whole group;
        # that also releases this observer (possibly with ECHILD).
        raise subprocess.TimeoutExpired(process.args, timeout)
    observer.join()
    if errors:
        # An older kernel or sandbox can reject waitid even when Python
        # exposes it. Do not leave the already-started compiler unmanaged.
        return process.wait(timeout=max(0.0, deadline - time.monotonic()))
    return process.wait()


def wait_process_exit(process: subprocess.Popen, timeout: float) -> int:
    """Wait for a Linux exit notification without timed-wait polling padding.

    Keep the child unreaped until readiness or timeout is observed. In
    particular, the timeout caller can still signal its process group without
    racing a watchdog thread against PID reuse. Other kernels retain Popen's
    non-reaping waitid notification or the bounded portable fallback.
    """
    deadline = time.monotonic() + timeout
    try:
        descriptor = os.pidfd_open(process.pid)
    except (AttributeError, OSError):
        return wait_process_notification(process, timeout)
    try:
        try:
            with selectors.DefaultSelector() as selector:
                selector.register(descriptor, selectors.EVENT_READ)
                if not selector.select(max(0.0, deadline - time.monotonic())):
                    raise subprocess.TimeoutExpired(process.args, timeout)
        except OSError:
            return process.wait(timeout=max(0.0, deadline - time.monotonic()))
        return process.wait()
    finally:
        os.close(descriptor)


def run_process(command: Sequence[str], cwd: Path, stem: Path, timeout: float) -> dict[str, Any]:
    stdout_path = stem.with_suffix(".stdout.txt")
    stderr_path = stem.with_suffix(".stderr.txt")
    result: dict[str, Any] = {
        "command": list(command), "cwd": str(cwd), "timeout_seconds": timeout,
        "stdout_path": str(stdout_path), "stderr_path": str(stderr_path),
        "status": "ERROR", "exit_code": None, "timed_out": False,
    }
    started = time.monotonic()
    with stdout_path.open("wb") as stdout, stderr_path.open("wb") as stderr:
        try:
            process = subprocess.Popen(
                list(command), cwd=cwd, stdout=stdout, stderr=stderr,
                start_new_session=True,
            )
            try:
                result["exit_code"] = wait_process_exit(process, timeout)
            except subprocess.TimeoutExpired:
                result["timed_out"] = True
                try:
                    os.killpg(process.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
                result["exit_code"] = process.wait(timeout=5)
            result["status"] = (
                "TIMEOUT" if result["timed_out"] else
                "CRASH" if result["exit_code"] < 0 else
                "OK" if result["exit_code"] == 0 else "ERROR"
            )
        except (OSError, subprocess.TimeoutExpired) as error:
            result["error"] = str(error)
            stderr.write((str(error) + "\n").encode("utf-8", errors="replace"))
            if result["timed_out"]:
                result["status"] = "TIMEOUT"
    result["duration_ms"] = round((time.monotonic() - started) * 1000, 3)
    return result


def bounded_text(path: Path) -> str:
    if path.stat().st_size > MAX_REPORT_BYTES:
        raise ValueError(f"artifact exceeds {MAX_REPORT_BYTES} bytes: {path}")
    return path.read_text(encoding="utf-8", errors="replace")


def parse_native_diagnostics(path: Path) -> dict[str, Any]:
    """Accept GCC SARIF results by official ruleId, never localized messages."""
    try:
        text = bounded_text(path)
        decoder = json.JSONDecoder()
        diagnostics: list[dict[str, Any]] = []
        remaining = text.lstrip()
        if not remaining:
            raise ValueError("no GCC SARIF diagnostic document was emitted")
        while remaining:
            document, consumed = decoder.raw_decode(remaining)
            if not isinstance(document, dict) or document.get("version") != "2.1.0" or not isinstance(document.get("runs"), list) or not document["runs"]:
                raise ValueError("expected a GCC SARIF 2.1.0 document with runs")
            for run in document["runs"]:
                if not isinstance(run, dict) or not isinstance(run.get("results"), list):
                    raise ValueError("expected a SARIF results array")
                if not all(isinstance(item, dict) for item in run["results"]):
                    raise ValueError("invalid SARIF result")
                diagnostics.extend(run["results"])
            remaining = remaining[consumed:].lstrip()
        warnings = [
            item for item in diagnostics
            if isinstance(item.get("ruleId"), str) and
            item["ruleId"].startswith("-Wanalyzer-")
        ]
        return {
            "status": "OK", "diagnostic_path": str(path),
            "format": "sarif-2.1.0",
            "warning_options": dict(sorted(Counter(item["ruleId"] for item in warnings).items())),
            "analyzer_warnings": warnings,
            "diagnostic_count": len(diagnostics),
            "accuracy_scored": False,
        }
    except (OSError, ValueError, RecursionError) as error:
        return {"status": "UNPARSED", "error": str(error),
                "diagnostic_path": str(path), "warning_options": None,
                "analyzer_warnings": None, "accuracy_scored": False}


def load_plugin_report(path: Path) -> dict[str, Any]:
    document = json.loads(bounded_text(path))
    if not isinstance(document, dict) or document.get("schema_version") != 2:
        raise ValueError("expected plugin report schema_version 2")
    if not isinstance(document.get("tool"), dict) or document["tool"].get("name") != "gcc-static-analyzer-plugin":
        raise ValueError("unexpected plugin report tool")
    if not isinstance(document.get("analysis"), dict) or document["analysis"].get("policy") != "proof-only":
        raise ValueError("expected proof-only plugin report")
    summary, findings = document.get("summary"), document.get("findings")
    if not isinstance(summary, dict) or not isinstance(findings, list):
        raise ValueError("missing plugin summary/findings")
    counts: Counter[str] = Counter()
    for finding in findings:
        if not isinstance(finding, dict) or finding.get("rule_id") not in RULES or finding.get("status") != "PROVEN":
            raise ValueError("invalid plugin finding")
        counts[finding["rule_id"]] += 1
    for key, expected in {"total": len(findings), **{rule: counts[rule] for rule in RULES}}.items():
        if type(summary.get(key)) is not int or summary[key] != expected:
            raise ValueError(f"inconsistent plugin summary: {key}")
    inconclusive = document.get("inconclusive_summary")
    if not isinstance(inconclusive, dict) or type(inconclusive.get("total")) is not int or inconclusive["total"] < 0:
        raise ValueError("missing plugin inconclusive summary")
    if summary.get("inconclusive_total") != inconclusive["total"]:
        raise ValueError("inconsistent plugin inconclusive count")
    return document


def run_mode(
    compiler: Path, sources: Sequence[Path], plugin: Path, output: Path,
    mode: str, compile_args: Sequence[str], link_args: Sequence[str],
    cwd: Path, timeout: float,
) -> dict[str, Any]:
    mode_dir = output / mode
    mode_dir.mkdir()
    compile_commands, link_command = build_commands(
        compiler, sources, plugin, mode_dir, mode, compile_args, link_args,
    )
    result: dict[str, Any] = {
        "status": "OK", "compile": [], "link": None,
        "compile_commands_planned": compile_commands,
        "link_command_planned": link_command,
        "sources_planned": len(sources), "sources_completed": 0,
        "total_compile_link_ms": 0.0,
    }
    for index, command in enumerate(compile_commands):
        phase = run_process(command, cwd, mode_dir / f"compile-{index:04d}", timeout)
        result["compile"].append(phase)
        if phase["status"] != "OK":
            result["status"] = phase["status"]
            result["link_skipped_reason"] = "source compilation did not complete successfully"
            break
        result["sources_completed"] += 1
    if result["status"] == "OK":
        result["link"] = run_process(link_command, cwd, mode_dir / "link", timeout)
        result["status"] = result["link"]["status"]
    result["compile_duration_ms"] = round(sum(phase["duration_ms"] for phase in result["compile"]), 3)
    result["link_duration_ms"] = result["link"]["duration_ms"] if result["link"] else None
    result["total_compile_link_ms"] = round(result["compile_duration_ms"] + (result["link_duration_ms"] or 0), 3)
    result["timing_complete"] = result["status"] == "OK"
    if mode == "plugin" and result["status"] == "OK":
        report_path = mode_dir / "analysis.json"
        result["plugin_report_path"] = str(report_path)
        result["plugin_html_path"] = str(mode_dir / "analysis.html")
        try:
            result["plugin_report"] = load_plugin_report(report_path)
            if not (mode_dir / "analysis.html").is_file():
                raise ValueError("plugin HTML report missing")
        except (OSError, ValueError, RecursionError) as error:
            result["status"] = "REPORT_ERROR"
            result["report_error"] = str(error)
    elif mode == "gcc_fanalyzer" and result["link"]:
        result["native_diagnostics"] = parse_native_diagnostics(Path(result["link"]["stderr_path"]))
        if result["status"] == "OK" and result["native_diagnostics"]["status"] != "OK":
            result["status"] = "REPORT_ERROR"
            result["report_error"] = result["native_diagnostics"]["error"]
    return result


def render_html(report: dict[str, Any], output: Path) -> str:
    def escape(value: Any) -> str:
        return html.escape(str(value), quote=True)

    def artifact(path: str, label: str) -> str:
        relative = os.path.relpath(path, output)
        return f'<a href="{escape(relative)}">{escape(label)}</a>'

    rows: list[str] = []
    details: list[str] = []
    for mode in MODES:
        result = report["modes"][mode]
        counts: Any = "—"
        if mode == "plugin" and "plugin_report" in result:
            counts = result["plugin_report"]["summary"]
        elif mode == "gcc_fanalyzer" and "native_diagnostics" in result:
            counts = result["native_diagnostics"]["warning_options"]
        rows.append(
            f'<tr><td>{escape(mode)}</td><td>{escape(result["status"])}</td>'
            f'<td>{result["sources_completed"]}/{result["sources_planned"]}</td>'
            f'<td>{result["compile_duration_ms"]}</td><td>{escape(result["link_duration_ms"])}</td>'
            f'<td>{result["total_compile_link_ms"]}</td><td>{escape(counts)}</td></tr>'
        )
        logs = []
        for phase in [*result["compile"], *([result["link"]] if result["link"] else [])]:
            logs.append('<li>' + artifact(phase["stdout_path"], "stdout") + " · " +
                        artifact(phase["stderr_path"], "stderr") + "<pre>" +
                        escape(json.dumps(phase["command"], ensure_ascii=False)) + "</pre></li>")
        if result.get("plugin_html_path") and Path(result["plugin_html_path"]).is_file():
            logs.append("<li>" + artifact(result["plugin_html_path"], "Plugin source findings and remediation report") + "</li>")
        details.append(f'<details><summary>{escape(mode)} commands and artifacts</summary><ul>{"".join(logs)}</ul></details>')
    return (
        '<!doctype html><html lang="en"><meta charset="utf-8"><title>LTO project comparison</title>'
        '<style>body{font:16px system-ui;max-width:1200px;margin:2rem auto;padding:1rem}'
        'table{border-collapse:collapse;width:100%}th,td{border:1px solid #ccc;padding:.5rem;text-align:left}'
        'pre{white-space:pre-wrap;overflow-wrap:anywhere}details{margin:1rem 0}</style>'
        '<h1>LTO project comparison</h1>'
        f'<p>Status: {escape(report["status"])}. Raw single-TU headline eligible: false.</p>'
        '<p>No accuracy oracle: warning counts are not TP/FP, precision or recall. '
        'All modes receive identical real source/provider inputs and common flags. '
        'One sequential run per mode: exploratory compile-plus-link timing only; '
        'failed or incomplete durations cannot support a speed comparison.</p>'
        '<p><a href="comparison.json">Complete JSON, hashes and limitations</a></p>'
        '<table><thead><tr><th>Mode</th><th>Status</th><th>Sources</th><th>Compile ms</th>'
        '<th>Link ms</th><th>Total ms</th><th>Observed counts, not accuracy</th></tr></thead><tbody>'
        + "".join(rows) + '</tbody></table>' + "".join(details) + '</html>\n'
    )


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    result.add_argument("--cc", required=True)
    result.add_argument("--plugin", required=True, type=Path)
    result.add_argument("--output", required=True, type=Path, help="new directory; existing paths are rejected")
    result.add_argument("--source", action="append", required=True, type=Path)
    result.add_argument("--compile-arg", action="append", default=[])
    result.add_argument("--link-arg", action="append", default=[])
    result.add_argument("--timeout", type=float, default=120.0, help="seconds per compile/link process (default: 120)")
    return result


def main(argv: Sequence[str] | None = None) -> int:
    arguments = parser().parse_args(argv)
    try:
        compile_args = validate_extra_args(arguments.compile_arg, "compile")
        link_args = validate_extra_args(arguments.link_arg, "link")
        if not math.isfinite(arguments.timeout) or arguments.timeout <= 0:
            raise ConfigurationError("--timeout must be finite and positive")
        if len(arguments.source) > MAX_SOURCES:
            raise ConfigurationError(f"at most {MAX_SOURCES} explicit sources are supported")
        found = shutil.which(arguments.cc)
        if found is None:
            raise ConfigurationError(f"compiler executable not found: {arguments.cc}")
        compiler = Path(found).resolve(strict=True)
        plugin = arguments.plugin.expanduser().resolve(strict=True)
        if not plugin.is_file() or not re.fullmatch(r"[A-Za-z0-9_-]+\.so", plugin.name):
            raise ConfigurationError("plugin must be a regular .so file with a simple basename")
        sources = [source.expanduser().resolve(strict=True) for source in arguments.source]
        if len(set(sources)) != len(sources):
            raise ConfigurationError("duplicate source paths are not supported")
        for source in sources:
            if not source.is_file() or source.suffix not in {".c", ".cc", ".cpp", ".cxx", ".C"}:
                raise ConfigurationError(f"expected an explicit C/C++ source file: {source}")
        output = arguments.output.expanduser().resolve()
        if output.exists():
            raise ConfigurationError(f"output path already exists; choose a new directory: {output}")
        initial_files = {str(path): sha256_file(path) for path in [compiler, *sources, Path(__file__).resolve()]}
        plugin_digest = sha256_file(plugin)
        output.mkdir(parents=True)
        snapshot_dir = output / "plugin-snapshot"
        snapshot_dir.mkdir()
        snapshot = snapshot_dir / plugin.name
        shutil.copy2(plugin, snapshot)
        if sha256_file(snapshot) != plugin_digest:
            raise ConfigurationError("plugin changed while its analysis snapshot was copied")
        cwd = Path.cwd().resolve()
        version = run_process([str(compiler), "--version"], cwd, output / "compiler-version", arguments.timeout)
        report: dict[str, Any] = {
            "schema_version": 1, "tool": "gcc-static-analyzer-lto-project-comparison",
            "benchmark_mode": {"name": "lto-project", "raw_headline_eligible": False,
                               "runner_injected_contracts": False, "provider_inputs": "explicit-real-source-tus"},
            "accuracy": {"scored": False, "reason": "no source-site accuracy oracle supplied"},
            "compiler": {"path": str(compiler), "sha256": initial_files[str(compiler)], "version_probe": version},
            "plugin": {"original_path": str(plugin), "snapshot_path": str(snapshot), "sha256": plugin_digest},
            "sources": [{"path": str(source), "sha256": initial_files[str(source)]} for source in sources],
            "common_flags": list(COMMON_FLAGS), "compile_args": compile_args, "link_args": link_args,
            "working_directory": str(cwd), "mode_order": list(MODES), "timeout_seconds_per_phase": arguments.timeout,
            "timing": {"kind": "single-run-exploratory", "scope": "sum of each mode's compile and link process wall times",
                       "controlled_speed_ranking": False, "metadata_probe_and_report_rendering_included": False},
            "limitations": [
                "No accuracy scoring without an oracle; counts are not true/false positives.",
                "LTO-project results are separate from raw single-translation-unit headline results.",
                "Only one sequential measurement per mode; cache, order and system load can affect timing.",
                "Single-partition LTO materializes project GIMPLE; large-project memory and compile costs are not guaranteed.",
                "Only explicit sources, compiler driver, runner and plugin snapshot are hashed; headers, libraries, environment and compiler subprograms are not snapshotted.",
                "C++ linkage/runtime is selected by the supplied gcc/g++ driver, not inferred or rewritten.",
            ],
            "modes": {},
        }
        for mode in MODES:
            print(f"LTO project: {mode}, {len(sources)} source(s)", flush=True)
            report["modes"][mode] = run_mode(
                compiler, sources, snapshot, output, mode, compile_args, link_args, cwd, arguments.timeout,
            )
        changed = []
        for path, expected in {**initial_files, str(snapshot): plugin_digest}.items():
            try:
                if sha256_file(Path(path)) != expected:
                    changed.append(path)
            except OSError:
                changed.append(path)
        report["input_integrity"] = {"verified_recorded_files": not changed, "changed_paths": changed,
                                     "recorded_initial_hashes": {**initial_files, str(snapshot): plugin_digest}}
        report["status"] = (
            "INPUT_DRIFT" if changed else "OK" if version["status"] == "OK" and
            all(result["status"] == "OK" for result in report["modes"].values()) else "INCOMPLETE"
        )
        report["timing"]["all_modes_completed"] = report["status"] == "OK"
        (output / "comparison.json").write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        (output / "comparison.html").write_text(render_html(report, output), encoding="utf-8")
        print(f'{report["status"]}: {output / "comparison.html"} (no accuracy scoring)')
        return 0 if report["status"] == "OK" else 1
    except (OSError, ConfigurationError) as error:
        print(f"LTO project configuration error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
