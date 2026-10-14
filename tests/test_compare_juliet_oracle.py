from __future__ import annotations

import argparse
from contextlib import redirect_stderr, redirect_stdout
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from tests import compare_juliet_oracle as juliet


def execution_result(duration_ms: float, status: str = "OK") -> dict[str, object]:
    return {
        "status": status,
        "duration_ms": duration_ms,
        "findings": [],
    }


def aggregate_result(
    duration_ms: float, variant: str, status: str = "OK"
) -> dict[str, object]:
    classification = "FN" if variant == "bad" else "TN"
    if status != "OK":
        classification = "UNASSESSED"
    return {
        "status": status,
        "classification": classification,
        "case_detection_classification": classification,
        "alert_burden_classification": classification,
        "target_finding_count": 0,
        "memory_lifetime_finding_count": 0,
        "matched_target_findings": [],
        "unmatched_target_findings": [],
        "wrong_rule_findings": [],
        "duration_ms": duration_ms,
    }


def group_result(
    baseline: tuple[float, float],
    plugin: tuple[float, float],
    gcc: tuple[float, float],
    *,
    language: str = "c",
    statuses: tuple[str, str] = ("OK", "OK"),
) -> dict[str, object]:
    variants: dict[str, object] = {}
    for index, variant in enumerate(("bad", "good")):
        status = statuses[index]
        unit = {
            "source": "case.c",
            "baseline": execution_result(baseline[index], status),
            "plugin": execution_result(plugin[index], status),
            "gcc16_fanalyzer": execution_result(gcc[index], status),
        }
        variants[variant] = {
            "baseline_status": status,
            "baseline_duration_ms": baseline[index],
            "plugin": aggregate_result(plugin[index], variant, status),
            "gcc16_fanalyzer": aggregate_result(gcc[index], variant, status),
            "translation_units": [unit],
        }
    return {
        "group_id": "401:case_01",
        "cwe": "401",
        "language": language,
        "tu_scope": "single_tu",
        "source_count": 1,
        "sources": ["case.c"],
        "expected_sites": [{"file": "case.c", "line": 3, "name": "leak"}],
        "variants": variants,
    }


def oracle_group(group_id: str, sources: tuple[Path, ...]) -> juliet.OracleGroup:
    return juliet.OracleGroup(
        group_id=group_id,
        manifest_index=0,
        cwe="401",
        cwe_name="CWE401_Memory_Leak",
        target_rule="SA001",
        sources=sources,
        expected_sites=(juliet.OracleSite(sources[0].name, 1, "leak"),),
        flow_variant="01",
    )


class OverheadSummaryTest(unittest.TestCase):
    def test_reports_ratio_of_sums_and_incremental_percent(self) -> None:
        summary = juliet.analyzer_overhead_summary(
            [100.0, 200.0], [150.0, 250.0]
        )
        self.assertEqual(summary["pair_count"], 2)
        self.assertEqual(summary["baseline_total_ms"], 300.0)
        self.assertEqual(summary["analyzer_total_ms"], 400.0)
        self.assertEqual(summary["incremental_total_ms"], 100.0)
        self.assertEqual(summary["summed_duration_ratio"], 1.333333)
        self.assertEqual(summary["incremental_percent_of_baseline"], 33.3333)
        self.assertEqual(summary["incremental_ms"]["mean_ms"], 50.0)

    def test_empty_and_mismatched_pairs_are_explicit(self) -> None:
        empty = juliet.analyzer_overhead_summary([], [])
        self.assertEqual(empty["pair_count"], 0)
        self.assertIsNone(empty["summed_duration_ratio"])
        self.assertIsNone(empty["incremental_percent_of_baseline"])
        with self.assertRaises(ValueError):
            juliet.analyzer_overhead_summary([1.0], [])

    def test_paired_timing_uses_only_three_way_successful_units(self) -> None:
        complete = group_result((100.0, 200.0), (125.0, 250.0), (175.0, 350.0))
        incomplete = group_result(
            (50.0, 60.0),
            (70.0, 80.0),
            (90.0, 100.0),
            statuses=("ERROR", "ERROR"),
        )
        timing = juliet.paired_timing_for([complete, incomplete])
        self.assertEqual(timing["pair_count"], 2)
        self.assertEqual(timing["baseline_compile"]["total_ms"], 300.0)
        self.assertEqual(
            timing["plugin_over_baseline"]["summed_duration_ratio"], 1.25
        )
        self.assertEqual(
            timing["gcc16_fanalyzer_over_baseline"]["summed_duration_ratio"],
            1.75,
        )

    def test_multi_tu_group_is_paired_and_summed_per_translation_unit(self) -> None:
        group = group_result(
            (100.0, 200.0), (125.0, 250.0), (175.0, 350.0)
        )
        group["tu_scope"] = "multi_tu"
        group["source_count"] = 2
        for index, variant in enumerate(("bad", "good")):
            group["variants"][variant]["translation_units"].append(
                {
                    "source": "case_helper.c",
                    "baseline": execution_result(30.0 + 10.0 * index),
                    "plugin": execution_result(50.0 + 10.0 * index),
                    "gcc16_fanalyzer": execution_result(70.0 + 10.0 * index),
                }
            )
        timing = juliet.paired_timing_for([group])
        self.assertEqual(timing["pair_count"], 4)
        self.assertEqual(timing["baseline_compile"]["total_ms"], 370.0)
        self.assertEqual(timing["plugin_compile"]["total_ms"], 485.0)
        self.assertEqual(timing["gcc16_fanalyzer_compile"]["total_ms"], 675.0)


class FairExecutionTest(unittest.TestCase):
    def test_stable_order_contains_each_measured_mode_once(self) -> None:
        source = Path("/tmp/stable-juliet-case.c")
        first = juliet.timed_mode_order("401:case", "bad", source)
        second = juliet.timed_mode_order(
            "401:case", "bad", Path("/different/checkout/stable-juliet-case.c")
        )
        self.assertEqual(first, second)
        self.assertEqual(
            set(first), {"baseline", "plugin", "gcc16_fanalyzer"}
        )
        self.assertEqual(len(first), 3)

        assigned = [
            tuple(juliet.timed_mode_order("401:case", "bad", source, index))
            for index in range(60)
        ]
        self.assertTrue(
            all(assigned.count(order) == 10 for order in juliet.TIMED_MODE_ORDERS)
        )

    def test_global_rotation_starts_are_sorted_and_count_tu_variants(self) -> None:
        first = oracle_group("401:a", (Path("a.c"), Path("a_helper.c")))
        second = oracle_group("401:b", (Path("b.c"),))
        self.assertEqual(
            juliet.group_rotation_start_indices([second, first]),
            {"401:a": 0, "401:b": 4},
        )
        self.assertEqual(
            sorted(juliet.timed_order_histogram(60).values()), [10] * 6
        )

    def test_run_group_assigns_contiguous_indices_to_every_tu_variant(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            sources = (root / "case_b.c", root / "case_a.c")
            for source in sources:
                source.write_text("int value;\n", encoding="utf-8")
            group = oracle_group("401:case", sources)
            observed: list[tuple[str, str, int | None]] = []

            def fake_unit(
                _args,
                _group,
                variant,
                _macro,
                source,
                _compilers,
                _plugin,
                _group_dir,
                rotation_index,
            ):
                observed.append((variant, source.name, rotation_index))
                return {
                    "source": source.name,
                    "baseline": execution_result(1.0),
                    "plugin": execution_result(2.0),
                    "gcc16_fanalyzer": execution_result(3.0),
                }

            args = argparse.Namespace(output=root / "out")
            with patch.object(juliet, "run_unit", side_effect=fake_unit):
                juliet.run_group(
                    args,
                    group,
                    {"c": "/opt/gcc-16.2/bin/gcc"},
                    root / "newplugin.so",
                    10,
                )

        self.assertEqual(
            observed,
            [
                ("bad", "case_a.c", 10),
                ("bad", "case_b.c", 11),
                ("good", "case_a.c", 12),
                ("good", "case_b.c", 13),
            ],
        )

    def test_run_unit_passes_the_identical_common_prefix_to_all_modes(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "case.c"
            source.write_text("int value;\n", encoding="utf-8")
            group = juliet.OracleGroup(
                group_id="401:case",
                manifest_index=0,
                cwe="401",
                cwe_name="CWE401_Memory_Leak",
                target_rule="SA001",
                sources=(source,),
                expected_sites=(juliet.OracleSite("case.c", 1, "leak"),),
                flow_variant="01",
            )
            args = argparse.Namespace(timeout=2.0)
            prefixes: list[list[str]] = []

            def baseline(prefix, *_args):
                prefixes.append(list(prefix))
                return execution_result(10.0)

            def plugin(prefix, *_args):
                prefixes.append(list(prefix))
                return execution_result(20.0)

            def native(prefix, *_args):
                prefixes.append(list(prefix))
                return execution_result(30.0)

            with (
                patch.object(juliet, "run_baseline", side_effect=baseline),
                patch.object(juliet, "run_plugin", side_effect=plugin),
                patch.object(juliet, "run_gcc_analyzer", side_effect=native),
            ):
                result = juliet.run_unit(
                    args,
                    group,
                    "bad",
                    "OMITGOOD",
                    source,
                    {"c": "/opt/gcc-16.2/bin/gcc"},
                    root / "newplugin.so",
                    root / "group",
                )

        self.assertEqual(len(prefixes), 3)
        self.assertEqual(prefixes[0], prefixes[1])
        self.assertEqual(prefixes[1], prefixes[2])
        self.assertEqual(
            set(result["timed_execution_order"]),
            {"baseline", "plugin", "gcc16_fanalyzer"},
        )

    def test_failed_baseline_still_keeps_analyzers_unassessed(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "broken.c"
            source.write_text("syntax error\n", encoding="utf-8")
            group = juliet.OracleGroup(
                group_id="401:broken",
                manifest_index=0,
                cwe="401",
                cwe_name="CWE401_Memory_Leak",
                target_rule="SA001",
                sources=(source,),
                expected_sites=(juliet.OracleSite("broken.c", 1, "leak"),),
                flow_variant=None,
            )
            args = argparse.Namespace(timeout=2.0)
            with (
                patch.object(
                    juliet,
                    "run_baseline",
                    return_value=execution_result(10.0, "BUILD_FAIL"),
                ),
                patch.object(
                    juliet, "run_plugin", return_value=execution_result(20.0)
                ),
                patch.object(
                    juliet,
                    "run_gcc_analyzer",
                    return_value=execution_result(30.0),
                ),
            ):
                result = juliet.run_unit(
                    args,
                    group,
                    "bad",
                    "OMITGOOD",
                    source,
                    {"c": "/opt/gcc-16.2/bin/gcc"},
                    root / "newplugin.so",
                    root / "group",
                )
        self.assertEqual(result["plugin"]["status"], "NOT_RUN")
        self.assertEqual(result["gcc16_fanalyzer"]["status"], "NOT_RUN")
        self.assertEqual(
            result["plugin"]["discarded_execution"]["status"], "OK"
        )

    def test_run_unit_rejects_source_drift_during_three_modes(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "case.c"
            source.write_text("int value;\n", encoding="utf-8")
            group = oracle_group("401:case", (source,))
            args = argparse.Namespace(timeout=2.0)

            def mutate_source(*_args):
                source.write_text("int changed;\n", encoding="utf-8")
                return execution_result(20.0)

            with (
                patch.object(
                    juliet, "run_baseline", return_value=execution_result(10.0)
                ),
                patch.object(juliet, "run_plugin", side_effect=mutate_source),
                patch.object(
                    juliet,
                    "run_gcc_analyzer",
                    return_value=execution_result(30.0),
                ),
            ):
                with self.assertRaisesRegex(
                    juliet.ConfigurationError, "changed during execution"
                ):
                    juliet.run_unit(
                        args,
                        group,
                        "bad",
                        "OMITGOOD",
                        source,
                        {"c": "/opt/gcc-16.2/bin/gcc"},
                        root / "newplugin.so",
                        root / "group",
                        0,
                    )


class ProvenanceTest(unittest.TestCase):
    def test_source_aggregate_is_sorted_path_independent_and_content_sensitive(self) -> None:
        with tempfile.TemporaryDirectory() as first_directory, tempfile.TemporaryDirectory() as second_directory:
            first_root = Path(first_directory)
            second_root = Path(second_directory)
            first_source = first_root / "testcases" / "case.c"
            second_source = second_root / "testcases" / "case.c"
            first_source.parent.mkdir()
            second_source.parent.mkdir()
            content = "int value;\n"
            first_source.write_text(content, encoding="utf-8")
            second_source.write_text(content, encoding="utf-8")
            first_group = oracle_group("401:case", (first_source,))
            second_group = oracle_group("401:case", (second_source,))

            first = juliet.source_selection_artifact([first_group], first_root)
            same_elsewhere = juliet.source_selection_artifact(
                [second_group], second_root
            )
            second_source.write_text("int changed;\n", encoding="utf-8")
            changed = juliet.source_selection_artifact([second_group], second_root)

        self.assertEqual(first["sha256"], same_elsewhere["sha256"])
        self.assertNotEqual(first["sha256"], changed["sha256"])
        self.assertEqual(first["translation_unit_count"], 1)

    def test_verifier_rejects_source_manifest_and_runner_drift(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "testcases" / "case.c"
            source.parent.mkdir()
            manifest = root / "manifest.xml"
            runner = root / "runner.py"
            source_text = "int value;\n"
            manifest_text = "<container/>\n"
            runner_text = "# runner\n"
            source.write_text(source_text, encoding="utf-8")
            manifest.write_text(manifest_text, encoding="utf-8")
            runner.write_text(runner_text, encoding="utf-8")
            group = oracle_group("401:case", (source,))
            sources = juliet.source_selection_artifact([group], root)
            manifest_sha256 = juliet.sha256_file(manifest)
            runner_sha256 = juliet.sha256_file(runner)

            verified = juliet.verify_input_provenance_unchanged(
                [group],
                root,
                sources,
                manifest,
                manifest_sha256,
                runner,
                runner_sha256,
            )
            self.assertEqual(verified["manifest_sha256"], manifest_sha256)

            source.write_text("int changed;\n", encoding="utf-8")
            with self.assertRaisesRegex(juliet.ConfigurationError, "source set"):
                juliet.verify_input_provenance_unchanged(
                    [group], root, sources, manifest, manifest_sha256, runner, runner_sha256
                )
            source.write_text(source_text, encoding="utf-8")

            manifest.write_text("<changed/>\n", encoding="utf-8")
            with self.assertRaisesRegex(juliet.ConfigurationError, "manifest"):
                juliet.verify_input_provenance_unchanged(
                    [group], root, sources, manifest, manifest_sha256, runner, runner_sha256
                )
            manifest.write_text(manifest_text, encoding="utf-8")

            runner.write_text("# changed\n", encoding="utf-8")
            with self.assertRaisesRegex(juliet.ConfigurationError, "runner"):
                juliet.verify_input_provenance_unchanged(
                    [group], root, sources, manifest, manifest_sha256, runner, runner_sha256
                )

    def test_main_returns_two_and_publishes_no_report_after_source_drift(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "testcases" / "case.c"
            source.parent.mkdir()
            source.write_text("int value;\n", encoding="utf-8")
            manifest = root / "manifest.xml"
            manifest.write_text("<container/>\n", encoding="utf-8")
            plugin = root / "newplugin.so"
            output = root / "output"
            group = oracle_group("401:case", (source,))

            def drift_during_group(*_args):
                source.write_text("int changed;\n", encoding="utf-8")
                return group_result(
                    (1.0, 1.0), (2.0, 2.0), (3.0, 3.0)
                )

            with (
                patch.object(
                    juliet,
                    "discover_oracle",
                    return_value=([group], {"manifest": str(manifest)}),
                ),
                patch.object(
                    juliet,
                    "validate_toolchains",
                    return_value=(
                        {"c": "/opt/gcc-16.2/bin/gcc"},
                        {"c": {"banner": "gcc 16.2", "executable": "/opt/gcc"}},
                    ),
                ),
                patch.object(
                    juliet,
                    "snapshot_plugin",
                    return_value=(plugin, "plugin-sha", plugin),
                ),
                patch.object(juliet, "preflight_plugin", return_value={}),
                patch.object(juliet, "run_group", side_effect=drift_during_group),
                redirect_stderr(io.StringIO()),
            ):
                result = juliet.main(
                    [
                        "--manifest",
                        str(manifest),
                        "--language",
                        "c",
                        "--plugin",
                        str(plugin),
                        "--output",
                        str(output),
                        "--jobs",
                        "1",
                    ]
                )
            json_report_exists = (output / "comparison.json").exists()
            html_report_exists = (output / "comparison.html").exists()

        self.assertEqual(result, 2)
        self.assertFalse(json_report_exists)
        self.assertFalse(html_report_exists)


class ContractBenchmarkTest(unittest.TestCase):
    def test_exact_cohort_rejects_duplicates_unknown_ids_and_conflicting_filters(self) -> None:
        groups = [
            oracle_group("401:first", (Path("first.c"),)),
            oracle_group("401:second", (Path("second.c"),)),
        ]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "ids.txt"
            path.write_text("# cohort\n 401:second # comment\n\n401:first\n", encoding="utf-8")
            ids, artifact = juliet.group_id_selection_artifact(path)
            self.assertEqual(ids, ("401:second", "401:first"))
            self.assertEqual(artifact["sha256"], juliet.sha256_file(path))
            self.assertEqual(artifact["group_id_count"], 2)
            self.assertEqual(juliet.select_groups(groups, "c", None, None, ids), groups)
            for contents, error in (
                ("401:first\n401:first # duplicate\n", "duplicate"),
                ("# empty\n\n", "no group ids"),
            ):
                with self.subTest(contents=contents):
                    path.write_text(contents, encoding="utf-8")
                    with self.assertRaisesRegex(juliet.ConfigurationError, error):
                        juliet.group_id_selection_artifact(path)
        for ids, limit, error in (
            (("401:first", "401:unknown"), None, "unknown"),
            (("401:first", "401:first"), None, "duplicate"),
            (("401:first", "401:second"), 1, "excluded ids"),
        ):
            with self.subTest(ids=ids, limit=limit):
                with self.assertRaisesRegex(juliet.ConfigurationError, error):
                    juliet.select_groups(groups, "c", None, limit, ids)

    def test_only_plugin_compiler_receives_contract_header(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "case.c"
            source.write_text("int value;\n", encoding="utf-8")
            header = root / "contract with spaces.h"
            header.write_text("/* trusted semantics */\n", encoding="utf-8")
            commands: list[list[str]] = []

            def capture_command(command, *_args):
                commands.append(list(command))
                return {}

            with (
                patch.object(juliet, "run_process", side_effect=capture_command),
                patch.object(
                    juliet, "final_process_result",
                    side_effect=lambda *_args: (execution_result(1.0), ""),
                ),
                patch.object(juliet, "parse_plugin_report", return_value=([], {
                    "stderr_json_findings_agree": True,
                })),
                patch.object(juliet, "parse_gcc_sarif", return_value=([], {})),
            ):
                result = juliet.run_unit(
                    argparse.Namespace(timeout=2.0, plugin_contract_header=header),
                    oracle_group("401:case", (source,)), "bad", "OMITGOOD", source,
                    {"c": "/opt/gcc-16.2/bin/gcc"}, root / "newplugin.so",
                    root / "output", 0,
                )
            self.assertEqual(result["plugin"]["status"], "OK")
            self.assertEqual(len(commands), 3)
            plugin_commands = [command for command in commands if any(
                argument.startswith("-fplugin=") for argument in command
            )]
            self.assertEqual(len(plugin_commands), 1)
            included = plugin_commands[0].index("-include")
            self.assertEqual(plugin_commands[0][included + 1], str(header.resolve()))
            for command in commands:
                if command is not plugin_commands[0]:
                    self.assertNotIn("-include", command)
                    self.assertNotIn(str(header.resolve()), command)

    def run_fake_benchmark(self, *, assisted=True, drift=None, stage="after"):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "testcases" / "case.c"
            source.parent.mkdir()
            source.write_text("int value;\n", encoding="utf-8")
            manifest = root / "manifest.xml"
            manifest.write_text("<container/>\n", encoding="utf-8")
            header = root / "contracts.h"
            header.write_text("/* contract */\n", encoding="utf-8")
            cohort = root / "ids.txt"
            cohort.write_text("401:case_01\n", encoding="utf-8")
            plugin = root / "newplugin.so"
            output = root / "output"
            group = oracle_group("401:case_01", (source,))
            drift_paths = {"header": header, "cohort": cohort}

            def mutate_input():
                if drift is not None:
                    drift_paths[drift].write_text("changed\n", encoding="utf-8")

            def preflight(*_args):
                if stage == "before":
                    mutate_input()
                return {}

            def run_group(*_args):
                if stage == "after":
                    mutate_input()
                return group_result((1.0, 1.0), (2.0, 2.0), (3.0, 3.0))

            stdout, stderr = io.StringIO(), io.StringIO()
            with (
                patch.object(juliet, "discover_oracle", return_value=(
                    [group], {"manifest": str(manifest)},
                )),
                patch.object(juliet, "validate_toolchains", return_value=(
                    {"c": "/opt/gcc-16.2/bin/gcc"},
                    {"c": {"banner": "gcc 16.2", "executable": "/opt/gcc"}},
                )),
                patch.object(juliet, "snapshot_plugin", return_value=(plugin, "plugin-sha", plugin)),
                patch.object(juliet, "preflight_plugin", side_effect=preflight),
                patch.object(juliet, "run_group", side_effect=run_group) as run_mock,
                redirect_stdout(stdout), redirect_stderr(stderr),
            ):
                arguments = [
                    "--manifest", str(manifest), "--language", "c",
                    "--plugin", str(plugin), "--output", str(output), "--jobs", "1",
                ]
                if assisted:
                    arguments += [
                        "--plugin-contract-header", str(header),
                        "--group-id-file", str(cohort),
                    ]
                status = juliet.main(arguments)
            json_path = output / "comparison.json"
            html_path = output / "comparison.html"
            return {
                "status": status, "stdout": stdout.getvalue(), "stderr": stderr.getvalue(),
                "report": json.loads(json_path.read_text()) if json_path.exists() else None,
                "html": html_path.read_text() if html_path.exists() else None,
                "run_group_calls": run_mock.call_count,
            }

    def test_assisted_mode_is_labeled_in_json_html_console_with_verified_hashes(self) -> None:
        result = self.run_fake_benchmark()
        self.assertEqual(result["status"], 0, result["stderr"])
        report = result["report"]
        for key in ("benchmark_mode", "configuration", "accuracy", "performance"):
            self.assertFalse(report[key]["raw_headline_eligible"], key)
            self.assertEqual(report[key]["kind"], "contract-assisted", key)
        verification = report["input_provenance_verification"]
        self.assertTrue(verification["verified_before_run"])
        self.assertTrue(verification["verified_after_run"])
        for key, digest_key in (
            ("plugin_contract_header_artifact", "plugin_contract_header_sha256"),
            ("group_id_selection_artifact", "group_id_file_sha256"),
        ):
            artifact = report[key]
            self.assertEqual(artifact["sha256"], verification[digest_key])
            self.assertIn(artifact["sha256"], result["html"])
            self.assertIn(artifact["sha256"], result["stdout"])
        self.assertEqual(report["plugin_contract_header_artifact"]["applies_to"], ["plugin"])
        self.assertEqual(report["group_id_selection_artifact"]["group_ids"], ["401:case_01"])
        for rendered in (result["html"], result["stdout"]):
            self.assertIn("NOT a raw benchmark headline", rendered)
            self.assertIn("Plugin (contract-assisted)", rendered)
            self.assertNotIn("Fair headline:", rendered)
        self.assertNotIn("HEADLINE: paired C", result["html"])

    def test_raw_mode_retains_unassisted_headline_and_no_injected_arguments(self) -> None:
        result = self.run_fake_benchmark(assisted=False)
        self.assertEqual(result["status"], 0, result["stderr"])
        report = result["report"]
        self.assertTrue(report["benchmark_mode"]["raw_headline_eligible"])
        self.assertEqual(report["benchmark_mode"]["kind"], "raw")
        self.assertIsNone(report["plugin_contract_header_artifact"])
        self.assertIsNone(report["group_id_selection_artifact"])
        self.assertEqual(report["configuration"]["plugin_only_extra_flags"], [])
        self.assertIn("Fair headline:", result["stdout"])
        self.assertIn("HEADLINE: paired C", result["html"])

    def test_input_drift_before_or_after_run_never_publishes_comparison(self) -> None:
        for drift, label in (("header", "plugin contract header"), ("cohort", "group-id file")):
            for stage in ("before", "after"):
                with self.subTest(drift=drift, stage=stage):
                    result = self.run_fake_benchmark(drift=drift, stage=stage)
                    self.assertEqual(result["status"], 2)
                    self.assertIn(label, result["stderr"])
                    self.assertIn("input drift", result["stderr"])
                    self.assertIsNone(result["report"])
                    self.assertIsNone(result["html"])
                    self.assertEqual(result["run_group_calls"], 0 if stage == "before" else 1)


class HtmlOverheadRenderingTest(unittest.TestCase):
    def test_html_renders_baseline_overhead_ratios(self) -> None:
        groups = [
            group_result(
                (100.0, 200.0), (150.0, 300.0), (200.0, 400.0)
            )
        ]
        report = {
            "generated_at": "2026-09-05T00:00:00+00:00",
            "configuration": {"jobs": 4},
            "toolchains": {
                "drivers": {
                    "c": {
                        "banner": "gcc (GCC) 16.2.0",
                        "executable": "/opt/gcc-16.2/bin/gcc",
                    }
                }
            },
            "plugin_artifact": {"sha256": "abc123"},
            "runner_artifact": {"sha256": "runner123"},
            "manifest_artifact": {"sha256": "manifest123"},
            "source_selection_artifact": {"sha256": "sources123"},
            "input_provenance_verification": {
                "verified_before_run": True,
                "verified_after_run": True,
            },
            "summary": {
                "parallel_run_wall_duration_ms": 42.0,
                "timed_execution_order_counts": {
                    "baseline > plugin > gcc16_fanalyzer": 2
                },
            },
            "oracle": {
                "inventory": {"manifest": "/tmp/manifest.xml"},
                "selected": {
                    "groups": 1,
                    "translation_units": 1,
                    "by_language": {"c": {"groups": 1}},
                },
            },
            "coverage": {
                "plugin": juliet.coverage_for(groups, "plugin"),
                "gcc16_fanalyzer": juliet.coverage_for(
                    groups, "gcc16_fanalyzer"
                ),
            },
            "accuracy": juliet.accuracy_report(groups),
            "performance": juliet.performance_report(groups),
            "groups": groups,
        }
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "comparison.html"
            juliet.write_html_report(path, report)
            rendered = path.read_text(encoding="utf-8")
        self.assertIn("Incremental analyzer overhead over plain GCC", rendered)
        self.assertIn("Plugin / baseline wall time", rendered)
        self.assertIn("1.50×", rendered)
        self.assertIn("2.00×", rendered)
        self.assertIn("All three measured modes rotate", rendered)
        self.assertIn("Timed-order histogram", rendered)
        self.assertIn("baseline &gt; plugin &gt; gcc16_fanalyzer=2", rendered)
        self.assertIn("runner123", rendered)
        self.assertIn("manifest123", rendered)
        self.assertIn("sources123", rendered)


if __name__ == "__main__":
    unittest.main()
