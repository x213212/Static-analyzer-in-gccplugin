from pathlib import Path
import json
import tempfile
import unittest

from tools.audit_realworld_precision import summarize


class PrecisionEvidenceTest(unittest.TestCase):
    def test_unrecorded_budget_is_unknown_and_full_unknown_counts_are_retained(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary) / "object"
            directory.mkdir()
            (directory / "execution.json").write_text(json.dumps({
                "status": "OK", "duration_ms": 10}))
            (directory / "analysis.json").write_text(json.dumps({
                "findings": [{"rule_id": "SA001"}],
                "inconclusive": [{"reason": "opaque"}],
                "inconclusive_summary": {"by_reason": {"opaque": 100}}}))
            result = summarize(Path(temporary))
            self.assertEqual(result["rules"], {"SA001": 1})
            self.assertEqual(result["inconclusive_reasons"], {"opaque": 100})
            self.assertIsNone(result["functions_skipped"])
            self.assertFalse(result["accuracy_scored"])

    def test_failed_process_remains_visible_without_an_analysis_report(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary) / "object"
            directory.mkdir()
            (directory / "execution.json").write_text(json.dumps({
                "status": "TIMEOUT", "duration_ms": 100}))
            result = summarize(Path(temporary))
            self.assertEqual(result["statuses"], {"TIMEOUT": 1})
            self.assertEqual(result["reports"], 0)
            self.assertFalse(result["accuracy_scored"])
