from __future__ import annotations

import json
from pathlib import Path
from types import SimpleNamespace
import tempfile
import unittest
from unittest.mock import patch

from tests import compare_benchmark_analyzers as comparison
from tests.compare_benchmark_analyzers import (
    parse_gcc_sarif,
    parse_plugin_report,
    target_recall,
)


class DiagnosticEvidenceParserTest(unittest.TestCase):
    def test_plugin_report_keeps_allocation_trace(self) -> None:
        document = {
            "summary": {
                "SA001": 0,
                "SA002": 1,
                "SA003": 0,
                "SA004": 0,
            },
            "findings": [
                {
                    "rule_id": "SA002",
                    "message": "double release",
                    "location": {"file": "/tmp/case.c", "line": 9},
                    "trace": [
                        {"role": "allocation", "file": "/tmp/case.c", "line": 4}
                    ],
                }
            ],
        }
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "analysis.json"
            path.write_text(json.dumps(document), encoding="utf-8")
            findings, counts, error = parse_plugin_report(path)
        self.assertIsNone(error)
        self.assertEqual(counts["SA002"], 1)
        self.assertEqual(
            {(point["line"], point["role"]) for point in findings[0]["evidence"]},
            {(9, "primary"), (4, "trace:allocation")},
        )

    def test_gcc_sarif_keeps_codeflow_allocation(self) -> None:
        document = {
            "runs": [
                {
                    "results": [
                        {
                            "ruleId": "-Wanalyzer-double-free",
                            "message": {"text": "double free"},
                            "locations": [
                                {
                                    "physicalLocation": {
                                        "artifactLocation": {"uri": "file:///tmp/case.c"},
                                        "region": {"startLine": 9},
                                    }
                                }
                            ],
                            "codeFlows": [
                                {
                                    "threadFlows": [
                                        {
                                            "locations": [
                                                {
                                                    "location": {
                                                        "physicalLocation": {
                                                            "artifactLocation": {
                                                                "uri": "file:///tmp/case.c"
                                                            },
                                                            "region": {"startLine": 4},
                                                        }
                                                    }
                                                }
                                            ]
                                        }
                                    ]
                                }
                            ],
                        }
                    ]
                }
            ]
        }
        mapped, rules, _, findings, error = parse_gcc_sarif(json.dumps(document))
        self.assertIsNone(error)
        self.assertEqual(mapped["SA002"], 1)
        self.assertEqual(rules["-Wanalyzer-double-free"], 1)
        self.assertEqual(
            {point["line"] for point in findings[0]["evidence"]}, {4, 9}
        )


class LocalizedRecallTest(unittest.TestCase):
    def test_unlocalized_same_rule_warning_gets_no_credit(self) -> None:
        result = {
            "suite": "extracted",
            "category": "double_free",
            "target": {
                "plugin_detected": False,
                "plugin_warning_emitted": True,
            },
            "plugin": {"status": "OK"},
        }
        report = target_recall([result], "plugin")["aggregate"]
        self.assertEqual(report["detected"], 0)
        self.assertEqual(report["warning_emitted_anywhere"], 1)
        self.assertEqual(report["unlocalized_warning_cases"], 1)
        self.assertEqual(report["target_recall"], 0)


class FairRunnerConfigurationTest(unittest.TestCase):
    def benchmark_case(
        self, source: Path, *, case_id: str = "juliet:CWE401/example.c"
    ) -> comparison.BenchmarkCase:
        return comparison.BenchmarkCase(
            case_id=case_id,
            suite="juliet",
            group="CWE401_Memory_Leak",
            category="memory_leak",
            source=source,
            language="c",
            include_dirs=(),
        )

    def test_all_modes_share_common_optimization_flags(self) -> None:
        case = self.benchmark_case(Path(comparison.__file__).resolve())
        baseline = comparison.base_compile_command("/tool/gcc-16", case)
        plugin = [
            *baseline,
            "-fplugin=/tmp/newplugin.so",
            *comparison.plugin_arguments(
                Path("/tmp/newplugin.so"), Path("/tmp/plugin-report")
            ),
        ]
        native = [*baseline, *comparison.GCC_ANALYZER_FLAGS]

        required = {
            "-O1",
            "-fipa-pta",
            "-fno-inline",
            "-fno-tree-dce",
            "-fno-tree-dse",
            "-fno-tree-fre",
        }
        for command in (baseline, plugin, native):
            self.assertTrue(required.issubset(command))
            self.assertNotIn("-O0", command)
        self.assertNotIn("-fanalyzer", baseline)
        self.assertNotIn("-fanalyzer", plugin)
        self.assertIn("-fanalyzer", native)

    def test_plugin_disables_thread_mode_and_html_report(self) -> None:
        arguments = comparison.plugin_arguments(
            Path("/tmp/newplugin.so"), Path("/tmp/plugin-report")
        )
        self.assertIn("-fplugin-arg-newplugin-doublefreecheck=1", arguments)
        self.assertIn("-fplugin-arg-newplugin-threadmod=0", arguments)
        self.assertIn("-fplugin-arg-newplugin-reporthtml=0", arguments)
        self.assertIn("-fplugin-arg-newplugin-report=1", arguments)

    def test_rotation_contains_each_of_the_six_permutations_once(self) -> None:
        orders = comparison.TIMED_MODE_ORDERS
        self.assertEqual(len(orders), 6)
        self.assertEqual(len(set(orders)), 6)
        self.assertTrue(
            all(
                set(order) == {"baseline", "plugin", "gcc16_fanalyzer"}
                for order in orders
            )
        )

    def test_sorted_index_rotation_is_balanced_and_path_independent(self) -> None:
        case = self.benchmark_case(Path(comparison.__file__).resolve())
        orders = [tuple(comparison.timed_mode_order(case, index)) for index in range(60)]
        self.assertTrue(
            all(orders.count(order) == 10 for order in comparison.TIMED_MODE_ORDERS)
        )

        same_id_elsewhere = self.benchmark_case(
            Path(__file__).resolve(), case_id=case.case_id
        )
        self.assertEqual(
            comparison.timed_mode_order(case),
            comparison.timed_mode_order(same_id_elsewhere),
        )

    def test_actual_runners_append_only_their_mode_specific_flags(self) -> None:
        case = self.benchmark_case(Path(comparison.__file__).resolve())
        commands: list[list[str]] = []

        def fake_process(
            command: object, _cwd: object, _timeout: object
        ) -> dict[str, object]:
            commands.append(list(command))  # type: ignore[arg-type]
            return {
                "status": "OK",
                "exit_code": 0,
                "timed_out": False,
                "duration_ms": 1.0,
                "stdout": "",
                "stderr": "",
            }

        with tempfile.TemporaryDirectory() as directory, patch.object(
            comparison, "run_process", side_effect=fake_process
        ), patch.object(
            comparison,
            "parse_plugin_report",
            return_value=(
                [],
                {code: 0 for code in comparison.PLUGIN_WARNING_CODES},
                None,
            ),
        ), patch.object(
            comparison,
            "parse_gcc_sarif",
            return_value=(
                {
                    code: 0
                    for code in comparison.GCC_RULE_TO_PLUGIN_WARNING.values()
                },
                {rule: 0 for rule in comparison.GCC_RULE_TO_PLUGIN_WARNING},
                {},
                [],
                None,
            ),
        ):
            case_dir = Path(directory)
            comparison.run_baseline_case("/tool/gcc-16", case, case_dir, 1.0)
            comparison.run_plugin_case(
                "/tool/gcc-16", Path("/tmp/newplugin.so"), case, case_dir, 1.0
            )
            comparison.run_gcc_analyzer_case(
                "/tool/gcc-16", case, case_dir, 1.0
            )

        self.assertEqual(len(commands), 3)
        baseline, plugin, native = commands
        for command in commands:
            for flag in comparison.COMMON_FLAGS:
                self.assertEqual(command.count(flag), 1)
            self.assertNotIn("-O0", command)
        self.assertFalse(any(flag.startswith("-fplugin=") for flag in baseline))
        self.assertNotIn("-fanalyzer", baseline)
        self.assertTrue(any(flag.startswith("-fplugin=") for flag in plugin))
        self.assertIn("-fplugin-arg-newplugin-threadmod=0", plugin)
        self.assertIn("-fplugin-arg-newplugin-reporthtml=0", plugin)
        self.assertNotIn("-fanalyzer", plugin)
        self.assertIn("-fanalyzer", native)
        self.assertFalse(any(flag.startswith("-fplugin=") for flag in native))

    def test_run_case_obeys_rotation_and_keeps_all_three_results(self) -> None:
        case = self.benchmark_case(Path(comparison.__file__).resolve())
        order = ["gcc16_fanalyzer", "baseline", "plugin"]
        observed: list[str] = []

        def result(duration_ms: float, **extra: object) -> dict[str, object]:
            return {
                "status": "OK",
                "duration_ms": duration_ms,
                "log": "/tmp/compiler.log",
                **extra,
            }

        def baseline_runner(*_args: object) -> dict[str, object]:
            observed.append("baseline")
            return result(10.0)

        def plugin_runner(*_args: object) -> dict[str, object]:
            observed.append("plugin")
            return result(15.0, warning_counts={}, findings=[])

        def native_runner(*_args: object) -> dict[str, object]:
            observed.append("gcc16_fanalyzer")
            return result(25.0, warning_counts={}, findings=[])

        with tempfile.TemporaryDirectory() as directory, patch.object(
            comparison, "timed_mode_order", return_value=order
        ), patch.object(
            comparison, "run_baseline_case", side_effect=baseline_runner
        ), patch.object(
            comparison, "run_plugin_case", side_effect=plugin_runner
        ), patch.object(
            comparison, "run_gcc_analyzer_case", side_effect=native_runner
        ):
            actual = comparison.run_case(
                SimpleNamespace(output=Path(directory), timeout=1.0),
                case,
                {"c": "/tool/gcc-16"},
                Path("/tmp/newplugin.so"),
                {},
            )

        self.assertEqual(observed, order)
        self.assertEqual(actual["timed_execution_order"], order)
        self.assertEqual(actual["baseline"]["duration_ms"], 10.0)
        self.assertEqual(actual["plugin"]["duration_ms"], 15.0)
        self.assertEqual(actual["gcc16_fanalyzer"]["duration_ms"], 25.0)


class ThreeWayPerformanceTest(unittest.TestCase):
    @staticmethod
    def result(
        baseline: tuple[str, float],
        plugin: tuple[str, float],
        native: tuple[str, float],
        *,
        suite: str = "juliet",
    ) -> dict[str, object]:
        return {
            "suite": suite,
            "baseline": {"status": baseline[0], "duration_ms": baseline[1]},
            "plugin": {"status": plugin[0], "duration_ms": plugin[1]},
            "gcc16_fanalyzer": {"status": native[0], "duration_ms": native[1]},
        }

    def test_headline_uses_only_three_way_ok_and_reports_overhead(self) -> None:
        results = [
            self.result(("OK", 10.0), ("OK", 15.0), ("OK", 25.0)),
            self.result(("ERROR", 8.0), ("OK", 9.0), ("OK", 11.0)),
        ]
        report = comparison.performance_report(results)
        headline = report["headline"]

        self.assertEqual(headline["case_count"], 1)
        self.assertEqual(headline["baseline"]["total_ms"], 10.0)
        self.assertEqual(
            headline["plugin_over_baseline"]["incremental_total_ms"], 5.0
        )
        self.assertEqual(
            headline["plugin_over_baseline"]["summed_duration_ratio"], 1.5
        )
        self.assertEqual(
            headline["gcc16_fanalyzer_over_baseline"]["incremental_total_ms"],
            15.0,
        )
        self.assertEqual(report["plugin"]["count"], 2)
        self.assertIs(report["paired_ok"], report["three_way_ok"])
        self.assertIn("configured compiler-process cost", report["measurement"])

    def test_overhead_vectors_must_be_paired(self) -> None:
        with self.assertRaises(ValueError):
            comparison.analyzer_overhead_summary([1.0], [2.0, 3.0])

    def test_direct_ratio_is_not_described_as_algorithmic_speed(self) -> None:
        statement = comparison.speed_statement(2.0)
        self.assertIn("configured process cost", statement)
        self.assertNotIn("faster", statement.lower())


class ProvenanceAndReportTest(unittest.TestCase):
    def test_selected_source_hash_is_order_independent_and_content_sensitive(self) -> None:
        test_root = Path(comparison.__file__).resolve().parent
        with tempfile.TemporaryDirectory(dir=test_root) as directory:
            root = Path(directory)
            first_source = root / "first.c"
            second_source = root / "second.c"
            first_source.write_text("int first(void) { return 1; }\n", encoding="utf-8")
            second_source.write_text("int second(void) { return 2; }\n", encoding="utf-8")
            first = FairRunnerConfigurationTest().benchmark_case(
                first_source, case_id="juliet:first.c"
            )
            second = FairRunnerConfigurationTest().benchmark_case(
                second_source, case_id="juliet:second.c"
            )

            forward = comparison.source_selection_artifact([first, second])
            reverse = comparison.source_selection_artifact([second, first])
            self.assertEqual(forward, reverse)
            first_source.write_text("int first(void) { return 3; }\n", encoding="utf-8")
            changed = comparison.source_selection_artifact([first, second])

        self.assertNotEqual(forward["sha256"], changed["sha256"])
        self.assertEqual(forward["case_count"], 2)
        self.assertEqual(len(forward["sources"]), 2)

    def test_post_run_provenance_check_rejects_source_or_oracle_drift(self) -> None:
        test_root = Path(comparison.__file__).resolve().parent
        with tempfile.TemporaryDirectory(dir=test_root) as directory:
            root = Path(directory)
            source = root / "case.c"
            oracle = root / "oracle.json"
            source.write_text("int value;\n", encoding="utf-8")
            oracle.write_text("{}\n", encoding="utf-8")
            case = FairRunnerConfigurationTest().benchmark_case(source)
            initial_sources = comparison.source_selection_artifact([case])
            initial_oracle = comparison.sha256_file(oracle)

            verified = comparison.verify_input_provenance_unchanged(
                [case], initial_sources, oracle, initial_oracle
            )
            self.assertTrue(verified["verified_after_run"])
            self.assertEqual(
                verified["selected_source_set_sha256"],
                initial_sources["sha256"],
            )

            source.write_text("int changed;\n", encoding="utf-8")
            with self.assertRaisesRegex(
                comparison.ConfigurationError, "selected source set"
            ):
                comparison.verify_input_provenance_unchanged(
                    [case], initial_sources, oracle, initial_oracle
                )

            source.write_text("int value;\n", encoding="utf-8")
            oracle.write_text('{"changed": true}\n', encoding="utf-8")
            with self.assertRaisesRegex(
                comparison.ConfigurationError, "extracted oracle"
            ):
                comparison.verify_input_provenance_unchanged(
                    [case], initial_sources, oracle, initial_oracle
                )

    def test_html_uses_three_way_configured_cost_language(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            log = str(root / "compiler.log")
            result = {
                "id": "juliet:sample.c",
                "suite": "juliet",
                "group": "CWE401_Memory_Leak",
                "category": "memory_leak",
                "source": "sample.c",
                "timed_execution_order": [
                    "baseline",
                    "plugin",
                    "gcc16_fanalyzer",
                ],
                "target": None,
                "baseline": {"status": "OK", "duration_ms": 10.0, "log": log},
                "plugin": {
                    "status": "OK",
                    "duration_ms": 15.0,
                    "log": log,
                    "warning_counts": {code: 0 for code in comparison.PLUGIN_WARNING_CODES},
                },
                "gcc16_fanalyzer": {
                    "status": "OK",
                    "duration_ms": 25.0,
                    "log": log,
                    "warning_counts": {
                        code: 0
                        for code in comparison.GCC_RULE_TO_PLUGIN_WARNING.values()
                    },
                    "analyzer_rule_ids": {
                        rule: 0 for rule in comparison.GCC_RULE_TO_PLUGIN_WARNING
                    },
                },
            }
            results = [result]
            report = {
                "generated_at": "2026-09-05T00:00:00+00:00",
                "configuration": {"jobs": 1},
                "summary": {
                    "total_cases": 1,
                    "parallel_run_wall_duration_ms": 50.0,
                    "timed_execution_order_counts": {
                        "baseline > plugin > gcc16_fanalyzer": 1
                    },
                    "baseline_statuses": comparison.status_counts(results, "baseline"),
                    "plugin_statuses": comparison.status_counts(results, "plugin"),
                    "gcc16_fanalyzer_statuses": comparison.status_counts(
                        results, "gcc16_fanalyzer"
                    ),
                },
                "toolchains": {
                    "drivers": {
                        "c": {
                            "banner": "gcc 16.2.0",
                            "executable": "/tool/gcc-16",
                        }
                    }
                },
                "plugin_artifact": {"sha256": "plugin-sha"},
                "runner_artifact": {"sha256": "runner-sha"},
                "extracted_oracle_artifact": {
                    "sha256": "oracle-sha",
                    "selected": {"sha256": "selected-oracle-sha"},
                },
                "source_selection_artifact": {"sha256": "sources-sha"},
                "input_provenance_verification": {"verified_after_run": True},
                "diagnostics": {
                    "plugin": comparison.warning_distribution(results, "plugin"),
                    "gcc16_fanalyzer": comparison.warning_distribution(
                        results, "gcc16_fanalyzer"
                    ),
                },
                "accuracy": {
                    "extracted_positive_targets": {
                        "plugin": comparison.target_recall(results, "plugin"),
                        "gcc16_fanalyzer": comparison.target_recall(
                            results, "gcc16_fanalyzer"
                        ),
                    }
                },
                "juliet_translation_units": comparison.juliet_report(results),
                "performance": comparison.performance_report(results),
                "cases": results,
            }
            output = root / "comparison.html"
            comparison.write_html_report(output, report)
            document = output.read_text(encoding="utf-8")

        self.assertIn("Three-way-OK configured compiler-process cost", document)
        self.assertIn("Plain GCC baseline", document)
        self.assertIn("Runner SHA-256", document)
        self.assertNotIn("Paired-OK process speed", document)

    def test_main_json_records_three_way_schema_and_provenance(self) -> None:
        case = FairRunnerConfigurationTest().benchmark_case(
            Path(comparison.__file__).resolve()
        )
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            output = root / "report"
            plugin_path = root / "newplugin.so"
            oracle_path = root / "oracle.json"
            plugin_path.write_bytes(b"test plugin artifact")
            oracle_path.write_text('{"schema_version": 1}\n', encoding="utf-8")
            expected_plugin_sha256 = comparison.sha256_file(plugin_path)
            log = str(root / "compiler.log")
            case_result = {
                "id": case.case_id,
                "suite": "juliet",
                "group": case.group,
                "category": case.category,
                "source": case.repository_relative_source.as_posix(),
                "source_sha256": comparison.sha256_file(case.source),
                "language": "c",
                "timed_execution_order": list(comparison.TIMED_MODE_ORDERS[0]),
                "target": None,
                "baseline": {"status": "OK", "duration_ms": 10.0, "log": log},
                "plugin": {
                    "status": "OK",
                    "duration_ms": 15.0,
                    "log": log,
                    "warning_counts": {
                        code: 0 for code in comparison.PLUGIN_WARNING_CODES
                    },
                },
                "gcc16_fanalyzer": {
                    "status": "OK",
                    "duration_ms": 25.0,
                    "log": log,
                    "warning_counts": {
                        code: 0
                        for code in comparison.GCC_RULE_TO_PLUGIN_WARNING.values()
                    },
                    "analyzer_rule_ids": {
                        rule: 0 for rule in comparison.GCC_RULE_TO_PLUGIN_WARNING
                    },
                },
            }
            selection = {
                "suites": ["juliet"],
                "families": [],
                "cwes": [case.group],
                "match": None,
                "limit": 1,
            }
            identities = {
                "c": {
                    "banner": "gcc 16.2.0",
                    "executable": "/tool/gcc-16",
                    "version": "16.2.0",
                    "target": "test-target",
                }
            }
            with patch.object(
                comparison, "selected_cases", return_value=([case], selection)
            ), patch.object(
                comparison, "load_extracted_oracle", return_value={}
            ), patch.object(
                comparison, "validate_extracted_oracle"
            ), patch.object(
                comparison,
                "validate_toolchains",
                return_value=({"c": "/tool/gcc-16"}, identities),
            ), patch.object(
                comparison, "preflight_plugin", return_value={"c": {"status": "OK"}}
            ), patch.object(
                comparison, "run_case", return_value=case_result
            ), patch("builtins.print"):
                exit_code = comparison.main(
                    [
                        "--suite",
                        "juliet",
                        "--limit",
                        "1",
                        "--jobs",
                        "1",
                        "--plugin",
                        str(plugin_path),
                        "--extracted-oracle",
                        str(oracle_path),
                        "--output",
                        str(output),
                    ]
                )

            report = json.loads(
                (output / "comparison.json").read_text(encoding="utf-8")
            )
            document = (output / "comparison.html").read_text(encoding="utf-8")

        self.assertEqual(exit_code, 0)
        self.assertEqual(report["schema_version"], 2)
        self.assertEqual(report["summary"]["baseline_statuses"]["OK"], 1)
        self.assertEqual(
            report["summary"]["timed_execution_order_counts"],
            {"baseline > plugin > gcc16_fanalyzer": 1},
        )
        self.assertEqual(report["performance"]["headline"]["case_count"], 1)
        self.assertIn(
            "plugin_over_baseline", report["performance"]["headline"]
        )
        self.assertEqual(
            report["configuration"]["common_flags"],
            list(comparison.COMMON_FLAGS),
        )
        self.assertTrue(
            report["input_provenance_verification"]["verified_after_run"]
        )
        self.assertEqual(
            report["runner_artifact"]["sha256"],
            comparison.sha256_file(Path(comparison.__file__).resolve()),
        )
        self.assertEqual(
            report["plugin_artifact"]["sha256"],
            expected_plugin_sha256,
        )
        self.assertEqual(report["source_selection_artifact"]["case_count"], 1)
        self.assertIn("Timed-order histogram", document)
        self.assertIn("Incremental configured cost over plain GCC", document)
        self.assertIn(expected_plugin_sha256, document)
        self.assertIn(report["source_selection_artifact"]["sha256"], document)
        self.assertIn("round-robin across all six permutations", document)


if __name__ == "__main__":
    unittest.main()
