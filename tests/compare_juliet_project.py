#!/usr/bin/env python3
"""Juliet oracle comparison with real provider TUs at the LTO link boundary.

Separate from compare_juliet_oracle.py's isolated-TU raw headline. Each official
bad/good program uses its supplied INCLUDEMAIN and separately compiled io.c;
all three modes compile and link exactly the same sources. No contracts or
visibility assumptions are added. Compiler outputs are never executed.

--reuse-reference re-assesses validated historical baseline/native results with
the current parser, while compiling and linking every plugin input afresh. It
does not reuse old object files or provide a fresh native timing comparison.
"""
from __future__ import annotations

import argparse
import copy
from concurrent.futures import ThreadPoolExecutor, as_completed
import json
import math
from pathlib import Path
import sys
import time
from typing import Any, Sequence

if __package__:
    from . import compare_juliet_oracle as oracle
else:
    import compare_juliet_oracle as oracle

sys.path.insert(0, str(oracle.REPOSITORY_ROOT / "tools"))
import compare_lto_project as project
import project_report_html as presentation


TOOLS = ("plugin", "gcc16_fanalyzer")


def reuse_reference(args: argparse.Namespace, groups: Sequence[oracle.OracleGroup],
                    identity: dict[str, Any], hashes: dict[str, str]) -> tuple[dict, dict]:
    """Validate historical evidence before launching any fresh compilation."""
    path = args.reuse_reference.resolve()
    reference_hash = oracle.sha256_file(path)
    reference = json.loads(path.read_text(encoding="utf-8"))
    fail = lambda message: oracle.ConfigurationError(f"reference cache rejected: {message}")
    if (reference.get("schema_version") != 1 or reference.get("status") != "OK"
            or reference.get("changed_inputs") != []
            or reference.get("benchmark_mode", {}).get("kind") != "lto-project-oracle"):
        raise fail("requires an OK LTO reference with no input drift")
    if reference.get("compiler") != identity:
        raise fail("compiler identity differs")
    recorded = reference.get("recorded_input_hashes", {})
    required = {Path(args.cc), args.manifest}
    required.update(source for group in groups for source in group.sources)
    support_files = {item for item in args.support.rglob("*") if item.is_file()}
    required.update(support_files)
    old_support = {Path(item) for item in recorded if Path(item).is_relative_to(args.support)}
    if old_support != support_files:
        raise fail("support-file inventory differs")
    for item in sorted(required):
        if recorded.get(str(item)) != hashes.get(str(item)) or str(item) not in hashes:
            raise fail(f"missing or changed compiler/manifest/source/support hash: {item}")
    old_groups = reference.get("groups", [])
    indexed = {group["group_id"]: group for group in old_groups}
    if len(indexed) != len(old_groups):
        raise fail("duplicate reference group identity")
    cached: dict[str, Any] = {}
    stderr_hashes: dict[str, str] = {}
    missing_logs: list[str] = []
    for group in groups:
        old = indexed.get(group.group_id, {})
        expected = {
            "group_id": group.group_id, "cwe": group.cwe, "language": "c",
            "tu_scope": group.tu_scope, "sources": [str(item) for item in group.sources],
            "provider_sources": [str(args.support / "io.c")],
            "expected_sites": [site.as_dict() for site in group.expected_sites],
        }
        if any(old.get(key) != value for key, value in expected.items()):
            raise fail(f"selected group/source/oracle identity differs: {group.group_id}")
        cached[group.group_id] = {}
        for variant, macro in (("bad", "OMITGOOD"), ("good", "OMITBAD")):
            cached[group.group_id][variant] = {}
            for tool, mode in (("baseline", "baseline"), ("gcc16_fanalyzer", "gcc_fanalyzer")):
                result = copy.deepcopy(old.get("variants", {}).get(variant, {}).get("project_modes", {}).get(tool, {}))
                link = result.get("link") or {}
                command = link.get("command", [])
                if (result.get("status") != "OK" or len(command) < 2
                        or command[-2] != "-o"):
                    raise fail(f"missing successful {group.group_id}/{variant}/{tool}")
                mode_dir = Path(command[-1]).parent
                if mode_dir.name != mode or mode_dir.parent.name != variant:
                    raise fail("unexpected original output directory")
                compiles, expected_link = project.build_commands(
                    Path(args.cc), [*group.sources, args.support / "io.c"], args.plugin,
                    mode_dir, mode, ["-std=gnu11", "-DINCLUDEMAIN", f"-D{macro}",
                                    f"-I{args.support}", "-pthread"], ["-pthread"],
                )
                phases = result.get("compile", [])
                if (result.get("compile_commands_planned") != compiles
                        or [phase.get("command") for phase in phases] != compiles
                        or result.get("link_command_planned") != expected_link
                        or command != expected_link
                        or result.get("sources_planned") != len(compiles)
                        or result.get("sources_completed") != len(compiles)):
                    raise fail(f"compile/link flags or inputs differ: {group.group_id}/{variant}/{tool}")
                log_stems = [f"compile-{index:04d}" for index in range(len(phases))] + ["link"]
                for phase, stem in zip([*phases, link], log_stems):
                    if (phase.get("status") != "OK" or phase.get("exit_code") != 0
                            or phase.get("timed_out") is not False
                            or phase.get("cwd") != str(oracle.REPOSITORY_ROOT)
                            or phase.get("stderr_path") != str(mode_dir / f"{stem}.stderr.txt")):
                        raise fail("unsuccessful or inconsistent historical process record")
                    stderr = Path(phase["stderr_path"])
                    if stderr.is_file():
                        stderr_hashes[str(stderr)] = oracle.sha256_file(stderr)
                    else:
                        missing_logs.append(str(stderr))
                    phase["historical_duration_ms"] = phase.get("historical_duration_ms", phase.get("duration_ms"))
                    phase["duration_ms"] = None
                    phase["cached"] = True
                if tool == "gcc16_fanalyzer":
                    diagnostics = result.get("native_diagnostics", {})
                    if diagnostics.get("status") != "OK" or not isinstance(diagnostics.get("analyzer_warnings"), list):
                        raise fail("missing native diagnostic payload")
                    stderr = Path(link["stderr_path"])
                    if stderr.is_file():
                        reparsed = project.parse_native_diagnostics(stderr)
                        if (reparsed.get("status") != "OK"
                                or reparsed.get("analyzer_warnings") != diagnostics["analyzer_warnings"]):
                            raise fail(f"native stderr no longer agrees with reference diagnostics: {stderr}")
                        result["native_diagnostics"] = reparsed
                        result["cached_diagnostics_source"] = "reparsed-sarif-stderr"
                    else:
                        result["cached_diagnostics_source"] = "reference-json-native-diagnostics"
                    # Re-score with today's official-rule/evidence mapping,
                    # never copy reference TP/FP or normalized findings.
                    if normalize_mode(result, tool, group)["status"] != "OK":
                        raise fail("current native parser cannot score cached diagnostics")
                result["historical_timing"] = result.get("historical_timing", {
                    key: result.get(key) for key in
                    ("compile_duration_ms", "link_duration_ms", "total_compile_link_ms")
                })
                for key in ("compile_duration_ms", "link_duration_ms", "total_compile_link_ms"):
                    result[key] = None
                result.update(cached=True, timing_complete=False, timing_measured=False,
                              reference_path=str(path), reference_sha256=reference_hash)
                cached[group.group_id][variant][tool] = result
    hashes[str(path)] = reference_hash
    hashes.update(stderr_hashes)
    return cached, {
        "path": str(path), "sha256": reference_hash, "cached": True,
        "modes": ["baseline", "gcc16_fanalyzer"], "selected_groups": len(groups),
        "cached_stderr_hashes": stderr_hashes, "missing_historical_logs": missing_logs,
        "native_accuracy": "historically assessed; re-scored using current native parser and oracle",
        "plugin_objects_reused": False,
        "limits": "Historical output files lack original digests. Present native stderr must match the embedded reference diagnostic payload; absent logs rely on the hashed reference JSON. Current log hashes are checked again after this run. System headers/libraries and environment remain unsnapshotted.",
    }


def normalize_mode(result: dict[str, Any], tool: str,
                   group: oracle.OracleGroup) -> dict[str, Any]:
    status = result["status"] if result["status"] in oracle.TOOL_STATUSES else "ERROR"
    normalized: dict[str, Any] = {
        "status": status, "duration_ms": 0.0 if result.get("cached") else result["total_compile_link_ms"],
        "findings": [], "provider_only_findings": [],
    }
    if status != "OK":
        return normalized
    if tool == "plugin":
        findings, metadata = oracle.parse_plugin_report(
            Path(result["plugin_report_path"]),
            project.bounded_text(Path(result["link"]["stderr_path"])),
        )
        if metadata.get("parse_error") or not metadata.get("stderr_json_findings_agree"):
            normalized.update(status="ERROR", parse_metadata=metadata)
            return normalized
    else:
        diagnostics = result.get("native_diagnostics", {})
        if diagnostics.get("status") != "OK":
            normalized.update(status="ERROR", parse_metadata=diagnostics)
            return normalized
        # The project runner has already validated every SARIF document.
        findings, metadata = oracle.parse_gcc_sarif(json.dumps({
            "version": "2.1.0", "runs": [{
                "results": diagnostics["analyzer_warnings"],
            }],
        }))
        if metadata.get("parse_error"):
            normalized.update(status="ERROR", parse_metadata=metadata)
            return normalized
    # A provider's unrelated defect must not give every linked testcase a TP.
    # The oracle discovery rejects ambiguous source basenames. A provider-site
    # finding can still qualify when its trace contains a member of this group.
    names = {source.name for source in group.sources}
    for finding in findings:
        eligible = any(
            oracle.basename(str(point.get("file", ""))) in names
            for point in finding.get("evidence", [])
        )
        normalized["findings" if eligible else "provider_only_findings"].append(finding)
    normalized["parse_metadata"] = metadata
    return normalized


def project_group(args: argparse.Namespace, group: oracle.OracleGroup,
                  plugin: Path) -> dict[str, Any]:
    directory = oracle.group_directory(args.output, group)
    directory.mkdir(parents=True)
    result: dict[str, Any] = {
        "group_id": group.group_id, "cwe": group.cwe,
        "language": "c", "tu_scope": group.tu_scope,
        "sources": [str(source) for source in group.sources],
        "provider_sources": [str(args.support / "io.c")],
        "expected_sites": [site.as_dict() for site in group.expected_sites],
        "artifact_directory": str(directory), "variants": {},
    }
    for variant_index, (variant, macro) in enumerate((("bad", "OMITGOOD"), ("good", "OMITBAD"))):
        variant_dir = directory / variant
        variant_dir.mkdir()
        order = oracle.TIMED_MODE_ORDERS[(args.order[group.group_id] * 2 + variant_index) % 6]
        reference = getattr(args, "reference_groups", {}).get(group.group_id)
        modes: dict[str, Any] = copy.deepcopy(reference[variant]) if reference else {}
        if reference:
            order = ("plugin",)
        for tool in order:
            mode = "gcc_fanalyzer" if tool == "gcc16_fanalyzer" else tool
            modes[tool] = project.run_mode(
                Path(args.cc), [*group.sources, args.support / "io.c"],
                plugin, variant_dir, mode,
                ["-std=gnu11", "-DINCLUDEMAIN", f"-D{macro}",
                 f"-I{args.support}", "-pthread"], ["-pthread"],
                oracle.REPOSITORY_ROOT, args.timeout,
            )
        unit: dict[str, Any] = {"source": str(group.sources[0])}
        for tool in TOOLS:
            unit[tool] = normalize_mode(modes[tool], tool, group)
            if modes["baseline"]["status"] != "OK":
                unit[tool]["status"] = "ERROR"
        record: dict[str, Any] = {
            "timed_order": list(order), "project_modes": modes,
            "provider_only_findings": {
                tool: unit[tool]["provider_only_findings"] for tool in TOOLS
            },
        }
        for tool in TOOLS:
            record[tool] = oracle.aggregate_tool_variant(
                [unit], tool, variant, group.target_rule, group.expected_sites,
            )
            if modes[tool].get("cached"):
                record[tool].update(cached=True, duration_ms=None, timing_measured=False)
        result["variants"][variant] = record
        (variant_dir / "project.json").write_text(
            json.dumps(record, indent=2) + "\n", encoding="utf-8",
        )
    return result


def render_html(report: dict[str, Any], output: Path) -> str:
    return presentation.render_html(report, output)


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=oracle.DEFAULT_MANIFEST)
    parser.add_argument("--cc", default=oracle.default_compiler("c"))
    parser.add_argument("--plugin", type=Path, default=oracle.REPOSITORY_ROOT / "Compiler/newplugin.so")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--group-id-file", type=Path)
    parser.add_argument("--match")
    parser.add_argument("--limit", type=int)
    parser.add_argument("--cwe", action="append", choices=tuple(oracle.CWE_CONFIGURATION))
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--timeout", type=float, default=30)
    parser.add_argument("--reuse-reference", type=Path,
                        help="reuse validated historical baseline/native accuracy from comparison.json; re-score with current parser and compile/link plugin inputs afresh; timings are plugin-only")
    args = parser.parse_args(argv)
    try:
        if not 1 <= args.jobs <= 32 or not math.isfinite(args.timeout) or args.timeout <= 0:
            raise oracle.ConfigurationError("jobs must be 1..32 and timeout finite and positive")
        if args.limit is not None and args.limit < 1:
            raise oracle.ConfigurationError("limit must be positive")
        args.output = args.output.resolve()
        if args.output.exists():
            raise oracle.ConfigurationError("output must be a new directory")
        args.manifest = args.manifest.resolve()
        args.support = args.manifest.parent / "testcasesupport"
        groups, inventory = oracle.discover_oracle(args.manifest, args.cwe or list(oracle.CWE_CONFIGURATION))
        ids = oracle.group_id_selection_artifact(args.group_id_file)[0] if args.group_id_file else None
        groups = sorted(oracle.select_groups(groups, "c", args.match, args.limit, ids), key=lambda item: item.group_id)
        args.order = {group.group_id: index for index, group in enumerate(groups)}
        args.cc = oracle.resolve_executable(args.cc)
        identity = oracle.compiler_identity(args.cc)
        if not str(identity["version"]).startswith("16.2"):
            raise oracle.ConfigurationError("this comparison requires GCC 16.2")
        source_set = oracle.source_selection_artifact(groups, args.manifest.parent)
        inputs = {Path(args.cc), args.manifest, Path(__file__).resolve(),
                  Path(project.__file__).resolve(), Path(oracle.__file__).resolve()}
        inputs.update(source for group in groups for source in group.sources)
        inputs.update(path for path in args.support.rglob("*") if path.is_file())
        if args.group_id_file:
            inputs.add(args.group_id_file.resolve())
        hashes = {str(path): oracle.sha256_file(path) for path in sorted(inputs)}
        args.reference_groups = {}
        reference_metadata = None
        if args.reuse_reference:
            args.reference_groups, reference_metadata = reuse_reference(args, groups, identity, hashes)
        args.output.mkdir(parents=True)
        plugin, plugin_hash, _ = oracle.snapshot_plugin(args.plugin, args.output)
        hashes[str(plugin)] = plugin_hash
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f"configuration error: {error}", file=sys.stderr)
        return 2
    started = time.monotonic()
    results = []
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        pending = {pool.submit(project_group, args, group, plugin): group for group in groups}
        for future in as_completed(pending):
            group = pending[future]
            try:
                results.append(future.result())
            except Exception as error:
                # Preserve a failed group instead of silently reducing coverage.
                failed = oracle.failed_group(group, args.output, error)
                results.append(failed)
            if len(results) % 25 == 0 or len(results) == len(groups):
                print(f"LTO oracle completed {len(results)}/{len(groups)} groups", flush=True)
    results.sort(key=lambda item: item["group_id"])
    changed = []
    for path, digest in hashes.items():
        try:
            if oracle.sha256_file(Path(path)) != digest:
                changed.append(path)
        except OSError:
            changed.append(path)
    paired = [group for group in results if all(
        group["variants"][variant][tool]["status"] == "OK"
        for variant in ("bad", "good") for tool in TOOLS
    )]
    report = {
        "schema_version": 1, "benchmark_mode": {
            "kind": "lto-project-oracle", "raw_headline_eligible": False,
            "runner_injected_contracts": False,
        },
        "status": "INPUT_DRIFT" if changed else "OK" if len(paired) == len(groups) else "INCOMPLETE",
        "selected_groups": len(groups), "common_paired_groups": len(paired),
        "compiler": identity, "plugin_sha256": plugin_hash,
        "oracle_inventory": inventory, "selected_source_set": source_set,
        "recorded_input_hashes": hashes, "changed_inputs": changed,
        "input_integrity_limits": "System headers/libraries, compiler subprograms and environment are not snapshotted.",
        "all_selected_accuracy": {tool: oracle.accuracy_for(results, tool) for tool in TOOLS},
        "common_paired_accuracy": {tool: oracle.accuracy_for(paired, tool) for tool in TOOLS},
        "common_paired_by_cwe": {
            cwe: {tool: oracle.accuracy_for([group for group in paired if group["cwe"] == cwe], tool) for tool in TOOLS}
            for cwe in sorted({group["cwe"] for group in results})
        },
        "timing": {"kind": "parallel-exploratory-compile-plus-link", "jobs": args.jobs,
                   "parallel_wall_ms": round((time.monotonic() - started) * 1000, 3),
                   "mode_orders": [list(order) for order in oracle.TIMED_MODE_ORDERS]},
        "groups": results,
    }
    if reference_metadata is not None:
        report["reference_cache"] = reference_metadata
        report["timing"].update(kind="cached-native-plugin-only", measured_modes=["plugin"],
                                historical_modes=["baseline", "gcc16_fanalyzer"],
                                speed_comparison_available=False, mode_orders=[["plugin"]])
    (args.output / "comparison.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    (args.output / "comparison.html").write_text(render_html(report, args.output), encoding="utf-8")
    for tool in TOOLS:
        print(tool, report["common_paired_accuracy"][tool]["case_detection_confusion_matrix"])
    print(report["status"], args.output / "comparison.html")
    return 0 if report["status"] == "OK" else 1


if __name__ == "__main__":
    raise SystemExit(main())
