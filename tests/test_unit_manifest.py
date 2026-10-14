from __future__ import annotations

import json
import importlib.util
from pathlib import Path
import re
from argparse import Namespace
import threading
import unittest
from unittest.mock import patch


REPOSITORY_ROOT = Path(__file__).resolve().parents[1]
MANIFEST = REPOSITORY_ROOT / "tests" / "manifest.json"
README = REPOSITORY_ROOT / "README.md"
RUN_SPEC = importlib.util.spec_from_file_location("unit_runner", MANIFEST.parent / "run.py")
RUNNER = importlib.util.module_from_spec(RUN_SPEC)
RUN_SPEC.loader.exec_module(RUNNER)


class UnitManifestTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.cases = json.loads(MANIFEST.read_text(encoding="utf-8"))

    def test_names_are_unique_and_sources_exist(self) -> None:
        names = [str(case["name"]) for case in self.cases]
        self.assertEqual(len(names), len(set(names)))
        for case in self.cases:
            source = MANIFEST.parent / str(case["source"])
            self.assertTrue(source.is_file(), f"missing source for {case['name']}: {source}")

    def test_inconclusive_reason_counts_match_rule_counts(self) -> None:
        for case in self.cases:
            by_rule = {
                str(rule): int(count)
                for rule, count in case.get("expect_inconclusive", {}).items()
            }
            by_reason = {
                str(reason): int(count)
                for reason, count in case.get(
                    "expect_inconclusive_reasons", {}
                ).items()
            }
            self.assertEqual(
                sum(by_rule.values()),
                sum(by_reason.values()),
                f"inconclusive count mismatch in {case['name']}",
            )

    def test_readme_suite_count_matches_manifest(self) -> None:
        readme = README.read_text(encoding="utf-8")
        match = re.search(r"The ([0-9]+)-case suite covers", readme)
        self.assertIsNotNone(match, "README has no assertion-suite count")
        self.assertEqual(int(match.group(1)), len(self.cases))

    def test_parallel_execution_keeps_report_order(self) -> None:
        cases = [{"name": "first"}, {"name": "second"}]
        barrier = threading.Barrier(2, timeout=5)
        observed = []

        def compile_case(args, root, case):
            barrier.wait()
            return dict(case)

        args = Namespace(output=Path("unused-unit-output"), jobs=2, timeout=15)
        with patch.object(RUNNER, "run_case", side_effect=compile_case):
            result = RUNNER.run_cases(args, MANIFEST.parent, cases, observed.append)
        self.assertEqual(result, cases)
        self.assertCountEqual(observed, cases)

    def test_colliding_case_directories_rejected_before_compile(self) -> None:
        args = Namespace(output=Path("unused-unit-output"), jobs=2, timeout=15)
        with patch.object(RUNNER, "run_case") as compile_case:
            with self.assertRaisesRegex(ValueError, "collision"):
                RUNNER.run_cases(args, MANIFEST.parent,
                                 [{"name": "a/b"}, {"name": "a__b"}])
            compile_case.assert_not_called()

    def test_empty_suite_and_invalid_parallelism(self) -> None:
        args = Namespace(output=Path("unused-unit-output"), jobs=1, timeout=15)
        self.assertEqual(RUNNER.run_cases(args, MANIFEST.parent, []), [])
        args.jobs = 0
        with self.assertRaisesRegex(ValueError, "positive"):
            RUNNER.run_cases(args, MANIFEST.parent, [])

    def test_nonfinite_timeout_rejected_before_compile(self) -> None:
        args = Namespace(output=Path("unused-unit-output"), jobs=2, timeout=15)
        with patch.object(RUNNER, "run_case") as compile_case:
            for timeout in (float("nan"), float("inf"), 0, -1):
                args.timeout = timeout
                with self.assertRaisesRegex(ValueError, "finite and positive"):
                    RUNNER.run_cases(args, MANIFEST.parent, [{"name": "first"}])
            compile_case.assert_not_called()

    def test_returned_lifetime_trace_requires_real_callee_evidence(self) -> None:
        self.assertTrue(RUNNER.has_lifetime_trace({"allocation", "previous release"}))
        roles = {"callee allocation origin", "callee releases before return",
                 "callee returns released pointer", "released object returned from callee"}
        self.assertTrue(RUNNER.has_lifetime_trace(roles))
        self.assertFalse(RUNNER.has_lifetime_trace(roles - {"callee allocation origin"}))
        self.assertFalse(RUNNER.has_lifetime_trace(roles - {"callee releases before return"}))


if __name__ == "__main__":
    unittest.main()
