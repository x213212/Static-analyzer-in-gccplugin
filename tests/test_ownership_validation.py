from __future__ import annotations

import copy
import importlib.util
from pathlib import Path
import sys
import unittest

TOOLS = Path(__file__).resolve().parents[1] / "tools"
sys.path.insert(0, str(TOOLS))


def load(name):
    spec = importlib.util.spec_from_file_location(name, TOOLS / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


FUZZ = load("fuzz_ownership_paths")
TRIAGE = load("triage_juliet_project")


class OwnershipOracleTest(unittest.TestCase):
    def test_contradictory_branch_cannot_supply_a_leak_witness(self):
        tree = (0, True, (0, False, "none", "free"), "free")
        self.assertFalse(FUZZ.oracle(tree)["leak_exists"])

    def test_conditional_callee_is_a_real_leak_but_not_an_independent_witness(self):
        result = FUZZ.oracle("maybe")
        self.assertTrue(result["leak_exists"])
        self.assertFalse(result["witness"]["release"])
        self.assertIsNone(result["independent_witness"])

    def test_ownership_barrier_bypass_has_a_concrete_guard_valuation(self):
        result = FUZZ.oracle((2, True, "maybe", "none"))
        self.assertFalse(result["independent_witness"]["guards"][2])


class TriageTest(unittest.TestCase):
    def report(self):
        source = str((TOOLS / "../tests/example.c").resolve())
        summary = {"inconclusive_summary": {"samples_omitted": 0},
                   "inconclusive": [{"rule_id": "SA001", "reason": "ambiguous-callee-ownership",
                                     "location": {"file": source, "line": 3}}]}
        group = {"group_id": "401:example", "cwe": "401", "sources": [source],
                 "variants": {"bad": {
                     "plugin": {"status": "OK", "target_rule": "SA001",
                                "classification": "FN", "case_detection_classification": "FN"},
                     "gcc16_fanalyzer": {"status": "OK", "classification": "FN",
                                         "case_detection_classification": "TP"},
                     "project_modes": {"plugin": {"plugin_report": summary,
                                                  "plugin_html_path": "example.html"}}}}}
        return {"status": "OK", "changed_inputs": [],
                "benchmark_mode": {"kind": "lto-project-oracle"},
                "plugin_sha256": "a" * 64, "compiler": {}, "groups": [group],
                "common_paired_accuracy": {"plugin": {"case_detection_confusion_matrix": {"FN": 1}}}}

    def test_case_detection_is_separate_from_exact_site(self):
        report = TRIAGE.summarize(self.report())
        self.assertEqual(report["buckets"][0]["native_only"], 1)
        self.assertEqual(report["missed_by_cwe"], {"401": 1})

    def test_provider_with_same_basename_does_not_supply_target_evidence(self):
        document = self.report()
        evidence = document["groups"][0]["variants"]["bad"]["project_modes"]["plugin"]["plugin_report"]
        evidence["inconclusive"][0]["location"]["file"] = "/tmp/provider/example.c"
        self.assertEqual(TRIAGE.summarize(document)["buckets"][0]["reasons"], ["no-target-candidate"])

    def test_truncated_samples_are_not_labelled_no_candidate(self):
        document = self.report()
        evidence = document["groups"][0]["variants"]["bad"]["project_modes"]["plugin"]["plugin_report"]
        evidence["inconclusive"] = []
        evidence["inconclusive_summary"]["samples_omitted"] = 2
        self.assertEqual(TRIAGE.summarize(document)["buckets"][0]["reasons"], ["report-samples-truncated"])

    def test_duplicate_or_incomplete_population_cannot_be_ranked(self):
        document = self.report()
        document["groups"].append(copy.deepcopy(document["groups"][0]))
        with self.assertRaisesRegex(ValueError, "duplicate"):
            TRIAGE.summarize(document)
        document = self.report()
        document["common_paired_accuracy"]["plugin"]["case_detection_confusion_matrix"]["FN"] = 2
        with self.assertRaisesRegex(ValueError, "partition"):
            TRIAGE.summarize(document)


if __name__ == "__main__":
    unittest.main()
