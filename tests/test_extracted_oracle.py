from __future__ import annotations

import copy
from pathlib import Path
import unittest

from tests.benchmark_manifest import discover_extracted
from tests.extracted_oracle import (
    DEFAULT_ORACLE,
    ExtractedOracleError,
    evidence_matches_expectation,
    load_extracted_oracle,
    validate_extracted_oracle,
)


class ExtractedOracleTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.cases = discover_extracted()
        cls.oracle = load_extracted_oracle(DEFAULT_ORACLE)

    def test_inventory_and_allocation_sites_are_current(self) -> None:
        self.assertEqual(len(self.cases), 100)
        self.assertEqual(len(self.oracle), 100)
        self.assertEqual(
            sum(len(expectation.allocation_sites) for expectation in self.oracle.values()),
            156,
        )
        validate_extracted_oracle(self.cases, self.oracle)

    def test_every_source_is_represented_exactly_once(self) -> None:
        sources = {
            case.repository_relative_source.as_posix() for case in self.cases
        }
        self.assertEqual(set(self.oracle), sources)

    def test_evidence_matching_uses_file_and_reviewed_line(self) -> None:
        expectation = self.oracle[
            "Test/benchmarks/binutils/double_free_1/binutils_df_1.c"
        ]
        line = next(iter(expectation.target_lines))
        self.assertTrue(
            evidence_matches_expectation(
                [{"file": f"/checkout/{expectation.source}", "line": line}],
                expectation,
            )
        )
        self.assertFalse(
            evidence_matches_expectation(
                [{"file": "/tmp/unrelated.c", "line": line}], expectation
            )
        )
        self.assertFalse(
            evidence_matches_expectation(
                [
                    {
                        "file": f"/tmp/other/{Path(expectation.source).name}",
                        "line": line,
                    }
                ],
                expectation,
            )
        )
        self.assertFalse(
            evidence_matches_expectation(
                [{"file": expectation.source, "line": line + 1}], expectation
            )
        )

    def test_hash_drift_is_a_configuration_error(self) -> None:
        case = self.cases[0]
        source = case.repository_relative_source.as_posix()
        modified = copy.copy(self.oracle)
        expectation = modified[source]
        modified[source] = type(expectation)(
            source=expectation.source,
            source_sha256="0" * 64,
            category=expectation.category,
            target_rule=expectation.target_rule,
            allocation_sites=expectation.allocation_sites,
        )
        with self.assertRaisesRegex(ExtractedOracleError, "fixture hash mismatch"):
            validate_extracted_oracle([case], modified)


if __name__ == "__main__":
    unittest.main()
