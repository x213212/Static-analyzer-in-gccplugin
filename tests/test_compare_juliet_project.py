from __future__ import annotations

import argparse
from contextlib import redirect_stderr, redirect_stdout
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from tests import compare_juliet_oracle as oracle
from tests import compare_juliet_project as project


def sarif_location(path: Path, line: int) -> dict:
    return {"physicalLocation": {"artifactLocation": {"uri": str(path)},
                                 "region": {"startLine": line}}}


class ProjectFixture:
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.source = self.root / "testcases" / "case.c"
        self.source.parent.mkdir()
        self.source.write_text("int testcase;\n", encoding="utf-8")
        self.provider = self.root / "testcasesupport" / "io.c"
        self.provider.parent.mkdir()
        self.provider.write_text("int support;\n", encoding="utf-8")
        self.group = oracle.OracleGroup(
            group_id="401:case", manifest_index=0, cwe="401",
            cwe_name="CWE401_Memory_Leak", target_rule="SA001",
            sources=(self.source,),
            expected_sites=(oracle.OracleSite("case.c", 17, "bad"),),
            flow_variant="01",
        )

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def plugin_mode(self, findings: list[dict], *, status: str = "OK") -> dict:
        path = self.root / "analysis.json"
        stderr = self.root / "plugin.stderr.txt"
        counts = {rule: sum(item["rule_id"] == rule for item in findings)
                  for rule in ("SA001", "SA002", "SA003", "SA004")}
        document = {
            "schema_version": 2,
            "tool": {"name": "gcc-static-analyzer-plugin", "version": "test"},
            "analysis": {"policy": "proof-only"},
            "source": "source-0000.o",
            "summary": {"total": len(findings), **counts, "inconclusive_total": 0},
            "findings": findings,
            "inconclusive_summary": {"total": 0}, "inconclusive": [],
        }
        path.write_text(json.dumps(document), encoding="utf-8")
        stderr.write_text("".join(
            f'{finding["location"]["file"]}:{finding["location"]["line"]}:1: warning: [{finding["rule_id"]}] finding\n'
            for finding in findings
        ), encoding="utf-8")
        return {"status": status, "total_compile_link_ms": 42.0,
                "plugin_report": document, "plugin_report_path": str(path),
                "link": {"status": status, "stderr_path": str(stderr), "duration_ms": 12.0}}

    def finding(self, primary: Path, *, line: int = 17, rule: str = "SA001", trace=()) -> dict:
        return {"rule_id": rule, "status": "PROVEN", "message": "finding",
                "location": {"file": str(primary), "line": line, "column": 1},
                "trace": [{"role": "evidence", "file": str(path), "line": trace_line, "column": 1}
                          for path, trace_line in trace]}

    def aggregate(self, normalized: dict, variant: str = "bad", tool: str = "plugin") -> dict:
        return oracle.aggregate_tool_variant(
            [{"source": str(self.source), tool: normalized}], tool, variant,
            self.group.target_rule, self.group.expected_sites,
        )

    def native_mode(self, results: list[dict], *, status: str = "OK") -> dict:
        stderr = self.root / "native.stderr.txt"
        stderr.write_text(json.dumps({"version": "2.1.0", "runs": [{"results": results}]}), encoding="utf-8")
        return {"status": status, "total_compile_link_ms": 55.0,
                "native_diagnostics": {"status": "OK", "analyzer_warnings": results},
                "link": {"status": status, "stderr_path": str(stderr), "duration_ms": 15.0}}


class NormalizeProjectModeTest(ProjectFixture, unittest.TestCase):
    def test_provider_only_plugin_finding_not_credited_or_counted_as_good_fp(self) -> None:
        mode = self.plugin_mode([self.finding(self.provider)])
        normalized = project.normalize_mode(mode, "plugin", self.group)
        self.assertEqual(normalized["status"], "OK")
        self.assertEqual(normalized["findings"], [])
        self.assertEqual(self.aggregate(normalized)["classification"], "FN")
        self.assertEqual(self.aggregate(normalized, "good")["classification"], "TN")

    def test_provider_primary_with_testcase_trace_is_eligible(self) -> None:
        mode = self.plugin_mode([self.finding(self.provider, trace=[(self.source, 17)])])
        normalized = project.normalize_mode(mode, "plugin", self.group)
        self.assertEqual(normalized["status"], "OK")
        self.assertEqual(len(normalized["findings"]), 1)
        self.assertEqual(self.aggregate(normalized)["classification"], "TP")

    def test_testcase_evidence_requires_exact_oracle_line_for_tp(self) -> None:
        mode = self.plugin_mode([self.finding(self.source, line=18)])
        normalized = project.normalize_mode(mode, "plugin", self.group)
        aggregate = self.aggregate(normalized)
        self.assertEqual(aggregate["classification"], "FN")
        self.assertEqual(aggregate["case_detection_classification"], "TP")
        self.assertEqual(len(aggregate["unmatched_target_findings"]), 1)

    def test_native_official_rule_and_trace_map_to_target(self) -> None:
        mode = self.native_mode([
            {"ruleId": "-Wanalyzer-malloc-leak", "level": "warning",
             "message": {"text": "localized message need not mention leak"},
             "locations": [sarif_location(self.provider, 100)],
             "codeFlows": [{"threadFlows": [{"locations": [
                 {"location": sarif_location(self.source, 17)}]}]}]},
        ])
        normalized = project.normalize_mode(mode, "gcc16_fanalyzer", self.group)
        self.assertEqual(normalized["status"], "OK")
        self.assertEqual(normalized["findings"][0]["rule_id"], "SA001")
        self.assertEqual(self.aggregate(normalized, tool="gcc16_fanalyzer")["classification"], "TP")

    def test_provider_only_native_finding_excluded(self) -> None:
        mode = self.native_mode([
            {"ruleId": "-Wanalyzer-malloc-leak", "message": {"text": "provider leak"},
             "locations": [sarif_location(self.provider, 17)]},
        ])
        normalized = project.normalize_mode(mode, "gcc16_fanalyzer", self.group)
        self.assertEqual(normalized["findings"], [])
        self.assertEqual(self.aggregate(normalized, tool="gcc16_fanalyzer")["classification"], "FN")

    def test_failed_project_is_not_tp_even_with_matching_finding(self) -> None:
        mode = self.plugin_mode([self.finding(self.source)], status="ERROR")
        normalized = project.normalize_mode(mode, "plugin", self.group)
        self.assertNotEqual(normalized["status"], "OK")
        self.assertEqual(self.aggregate(normalized)["classification"], "UNASSESSED")

    def test_timeout_and_report_error_are_not_zero_finding_success(self) -> None:
        for status in ("TIMEOUT", "REPORT_ERROR"):
            with self.subTest(status=status):
                mode = self.plugin_mode([], status=status)
                normalized = project.normalize_mode(mode, "plugin", self.group)
                self.assertNotEqual(normalized["status"], "OK")
                self.assertEqual(self.aggregate(normalized)["classification"], "UNASSESSED")

    def test_plugin_parse_failure_errors_instead_of_crediting_stderr(self) -> None:
        mode = self.plugin_mode([self.finding(self.source)])
        Path(mode["plugin_report_path"]).write_text("not JSON", encoding="utf-8")
        normalized = project.normalize_mode(mode, "plugin", self.group)
        self.assertEqual(normalized["status"], "ERROR")
        self.assertEqual(self.aggregate(normalized)["classification"], "UNASSESSED")

    def test_native_parse_failure_errors_instead_of_zero_findings(self) -> None:
        mode = self.native_mode([])
        # The low-level project runner owns SARIF validation. The group layer
        # consumes its validated result rather than reparsing mutable logs.
        mode["native_diagnostics"] = {"status": "UNPARSED", "error": "invalid SARIF", "analyzer_warnings": None}
        normalized = project.normalize_mode(mode, "gcc16_fanalyzer", self.group)
        self.assertEqual(normalized["status"], "ERROR")
        self.assertEqual(self.aggregate(normalized, tool="gcc16_fanalyzer")["classification"], "UNASSESSED")

    def test_duration_is_whole_compile_plus_link(self) -> None:
        normalized = project.normalize_mode(self.plugin_mode([]), "plugin", self.group)
        self.assertEqual(normalized["duration_ms"], 42.0)


class ProjectGroupTest(ProjectFixture, unittest.TestCase):
    def args(self) -> argparse.Namespace:
        return argparse.Namespace(
            output=self.root / "output", support=self.provider.parent,
            cc="/fake/gcc", order={self.group.group_id: 0}, timeout=3,
        )

    def fake_mode(self, compiler, sources, plugin, output, mode, compile_args, link_args, cwd, timeout):
        bad = "-DOMITGOOD" in compile_args
        if mode == "plugin":
            # An unrelated provider-only warning on good must not become FP.
            return self.plugin_mode([self.finding(self.source if bad else self.provider)])
        if mode == "gcc_fanalyzer":
            return self.native_mode([
                {"ruleId": "-Wanalyzer-malloc-leak", "message": {"text": "diagnostic"},
                 "locations": [sarif_location(self.source if bad else self.provider, 17)]},
            ])
        return {"status": "OK", "total_compile_link_ms": 30.0, "link": None}

    def test_group_compiles_real_sources_plus_provider_and_separates_oracle_scores(self) -> None:
        args = self.args()
        with patch.object(project.project, "run_mode", side_effect=self.fake_mode) as run:
            result = project.project_group(args, self.group, self.root / "newplugin.so")
        self.assertEqual(run.call_count, 6)
        self.assertEqual(result["group_id"], self.group.group_id)
        self.assertEqual(result["provider_sources"], [str(self.provider)])
        for call in run.call_args_list:
            compiler, sources, plugin, output, mode, compile_args, link_args, cwd, timeout = call.args
            self.assertEqual(sources, [self.source, self.provider])
            self.assertIn("-DINCLUDEMAIN", compile_args)
            self.assertIn(f"-I{self.provider.parent}", compile_args)
            self.assertNotIn("-include", compile_args)
            self.assertEqual(timeout, 3)
        for tool in project.TOOLS:
            self.assertEqual(result["variants"]["bad"][tool]["classification"], "TP")
            self.assertEqual(result["variants"]["good"][tool]["classification"], "TN")
            self.assertEqual(len(result["variants"]["good"]["provider_only_findings"][tool]), 1)
        self.assertNotEqual(result["variants"]["bad"]["timed_order"], result["variants"]["good"]["timed_order"])
        for variant in ("bad", "good"):
            evidence = Path(result["artifact_directory"]) / variant / "project.json"
            self.assertTrue(evidence.is_file())

    def test_baseline_failure_gates_both_analyzer_classifications(self) -> None:
        def baseline_failure(*args):
            result = self.fake_mode(*args)
            if args[4] == "baseline":
                result["status"] = "ERROR"
            return result
        with patch.object(project.project, "run_mode", side_effect=baseline_failure):
            result = project.project_group(self.args(), self.group, self.root / "newplugin.so")
        for variant in ("bad", "good"):
            for tool in project.TOOLS:
                self.assertEqual(result["variants"][variant][tool]["classification"], "UNASSESSED")
                self.assertNotEqual(result["variants"][variant][tool]["status"], "OK")

    def test_cross_cwe_good_finding_stays_separate_from_target_false_positive(self) -> None:
        def wrong_rule(*args):
            if args[4] == "plugin" and "-DOMITBAD" in args[5]:
                return self.plugin_mode([self.finding(self.source, rule="SA003")])
            return self.fake_mode(*args)
        with patch.object(project.project, "run_mode", side_effect=wrong_rule):
            result = project.project_group(self.args(), self.group, self.root / "newplugin.so")
        good = result["variants"]["good"]["plugin"]
        self.assertEqual(good["classification"], "TN")
        self.assertEqual(good["alert_burden_classification"], "FP")
        self.assertEqual(len(good["wrong_rule_findings"]), 1)

    def test_html_explicitly_labels_lto_mode_and_separate_alert_burden(self) -> None:
        args = self.args()
        with patch.object(project.project, "run_mode", side_effect=self.fake_mode):
            group_result = project.project_group(args, self.group, self.root / "newplugin.so")
        report = {"status": "OK", "common_paired_groups": 1, "selected_groups": 1,
                  "common_paired_accuracy": {tool: oracle.accuracy_for([group_result], tool) for tool in project.TOOLS},
                  "groups": [group_result]}
        rendered = project.render_html(report, args.output)
        self.assertIn("LTO project oracle", rendered)
        self.assertIn("separate from isolated-TU raw", rendered)
        self.assertIn("Provider-only unrelated findings", rendered)
        self.assertIn("Any-lifetime good alerts", rendered)
        self.assertIn("do not establish controlled speed rankings", rendered)


class ProjectConfigurationTest(ProjectFixture, unittest.TestCase):
    def invoke(self, extra=()):
        output = self.root / "output"
        with patch.object(project.oracle, "discover_oracle", return_value=([self.group], {})), \
                patch.object(project.oracle, "resolve_executable") as compiler, \
                redirect_stdout(io.StringIO()), redirect_stderr(io.StringIO()):
            status = project.main(["--manifest", str(self.root / "manifest.xml"),
                                   "--output", str(output), *extra])
        compiler.assert_not_called()
        self.assertFalse(output.exists())
        return status

    def test_duplicate_ids_rejected_before_compiler_probe(self) -> None:
        ids = self.root / "ids.txt"
        ids.write_text("401:case\n401:case\n", encoding="utf-8")
        self.assertEqual(self.invoke(["--group-id-file", str(ids)]), 2)

    def test_unknown_ids_rejected_before_compiler_probe(self) -> None:
        ids = self.root / "ids.txt"
        ids.write_text("401:not-present\n", encoding="utf-8")
        self.assertEqual(self.invoke(["--group-id-file", str(ids)]), 2)

    def test_filter_cannot_silently_drop_requested_exact_ids(self) -> None:
        ids = self.root / "ids.txt"
        ids.write_text("401:case\n", encoding="utf-8")
        self.assertEqual(self.invoke(["--group-id-file", str(ids), "--match", "not-present"]), 2)

    def test_empty_selection_and_invalid_resource_bounds_rejected(self) -> None:
        for args in (["--match", "not-present"], ["--jobs", "0"], ["--timeout", "nan"], ["--limit", "0"]):
            with self.subTest(args=args):
                self.assertEqual(self.invoke(args), 2)


class ProjectMainTest(ProjectFixture, unittest.TestCase):
    def run_main(self, *, worker_failure: bool = False, mutate_provider: bool = False):
        manifest = self.root / "manifest.xml"
        manifest.write_text("<testcases/>\n", encoding="utf-8")
        compiler = self.root / "gcc"
        compiler.write_text("fake compiler identity\n", encoding="utf-8")
        plugin = self.root / "newplugin.so"
        plugin.write_bytes(b"fake plugin snapshot")
        output = self.root / "output"

        def fake_mode(*args):
            result = ProjectGroupTest.fake_mode(self, *args)
            if mutate_provider:
                self.provider.write_text("int provider_changed;\n", encoding="utf-8")
            return result

        with patch.object(project.oracle, "discover_oracle", return_value=([self.group], {})), \
                patch.object(project.oracle, "resolve_executable", return_value=str(compiler)), \
                patch.object(project.oracle, "compiler_identity", return_value={"version": "16.2.0"}), \
                patch.object(project.project, "run_mode", side_effect=RuntimeError("worker interrupted") if worker_failure else fake_mode), \
                redirect_stdout(io.StringIO()), redirect_stderr(io.StringIO()):
            status = project.main([
                "--manifest", str(manifest), "--cc", str(compiler),
                "--plugin", str(plugin), "--output", str(output), "--jobs", "1",
            ])
        return status, json.loads((output / "comparison.json").read_text(encoding="utf-8"))

    def test_full_report_keeps_project_mode_provenance_and_paired_population(self) -> None:
        status, report = self.run_main()
        self.assertEqual(status, 0)
        self.assertEqual(report["status"], "OK")
        self.assertEqual(report["benchmark_mode"]["kind"], "lto-project-oracle")
        self.assertFalse(report["benchmark_mode"]["raw_headline_eligible"])
        self.assertFalse(report["benchmark_mode"]["runner_injected_contracts"])
        self.assertEqual(report["selected_groups"], 1)
        self.assertEqual(report["common_paired_groups"], 1)
        self.assertIn(str(self.provider), report["recorded_input_hashes"])
        self.assertEqual(report["changed_inputs"], [])
        for tool in project.TOOLS:
            self.assertEqual(report["common_paired_accuracy"][tool]["case_detection_confusion_matrix"],
                             {"TP": 1, "FP": 0, "TN": 1, "FN": 0})

    def test_worker_exception_preserves_failed_group_as_unassessed(self) -> None:
        status, report = self.run_main(worker_failure=True)
        self.assertEqual(status, 1)
        self.assertEqual(report["status"], "INCOMPLETE")
        self.assertEqual(report["selected_groups"], 1)
        self.assertEqual(report["common_paired_groups"], 0)
        self.assertEqual(len(report["groups"]), 1)
        self.assertEqual(report["groups"][0]["group_id"], self.group.group_id)
        for tool in project.TOOLS:
            score = report["all_selected_accuracy"][tool]
            self.assertEqual(score["case_detection_confusion_matrix"], {"TP": 0, "FP": 0, "TN": 0, "FN": 0})
            self.assertEqual(score["unassessed_positive_variants"], 1)
            self.assertEqual(score["unassessed_negative_variants"], 1)

    def test_provider_input_drift_invalidates_even_successful_executions(self) -> None:
        status, report = self.run_main(mutate_provider=True)
        self.assertEqual(status, 1)
        self.assertEqual(report["status"], "INPUT_DRIFT")
        self.assertIn(str(self.provider), report["changed_inputs"])


if __name__ == "__main__":
    unittest.main()
