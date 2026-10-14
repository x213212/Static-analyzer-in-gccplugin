"""Fast invariants for benchmark discovery (no compiler invocation)."""

from collections import Counter
import unittest

from tests.benchmark_manifest import discover_extracted, discover_juliet


class ExtractedDiscoveryTest(unittest.TestCase):
    def test_all_five_families_have_twenty_cases(self) -> None:
        cases = discover_extracted()
        self.assertEqual(len(cases), 100)
        self.assertEqual(
            Counter(case.group for case in cases),
            {
                "binutils": 20,
                "git": 20,
                "openssh-portable": 20,
                "openssl": 20,
                "tmux": 20,
            },
        )

    def test_project_stdio_compatibility_files_cannot_shadow_libc(self) -> None:
        self.assertTrue(
            all(not case.include_dirs for case in discover_extracted())
        )


class JulietDiscoveryTest(unittest.TestCase):
    def test_bundled_subset_inventory(self) -> None:
        cases = discover_juliet()
        self.assertEqual(len(cases), 4880)
        self.assertEqual(
            Counter(case.group for case in cases),
            {
                "CWE401_Memory_Leak": 2710,
                "CWE415_Double_Free": 1648,
                "CWE416_Use_After_Free": 522,
            },
        )
        self.assertEqual(
            Counter(case.language for case in cases),
            {"c": 1714, "c++": 3166},
        )


if __name__ == "__main__":
    unittest.main()
