"""Discovery rules for the repository's legacy benchmark corpora.

This module intentionally contains no expected analyzer results.  The extracted
projects and Juliet are build/robustness corpora; semantic expectations belong
in the small, reviewed unit-test manifest.
"""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


REPOSITORY_ROOT = Path(__file__).resolve().parents[1]
BENCHMARK_ROOT = REPOSITORY_ROOT / "Test" / "benchmarks"
JULIET_ROOT = (
    BENCHMARK_ROOT / "Juliet_Test_Suite_v1.3_for_C_Cpp" / "C"
)

EXTRACTED_FAMILIES = (
    "binutils",
    "git",
    "openssh-portable",
    "openssl",
    "tmux",
)

JULIET_CWES = (
    "CWE401_Memory_Leak",
    "CWE415_Double_Free",
    "CWE416_Use_After_Free",
)


@dataclass(frozen=True)
class BenchmarkCase:
    """One independently compilable benchmark translation unit."""

    case_id: str
    suite: str
    group: str
    category: str
    source: Path
    language: str
    include_dirs: tuple[Path, ...]

    @property
    def repository_relative_source(self) -> Path:
        return self.source.relative_to(REPOSITORY_ROOT)


def _language_for(source: Path) -> str:
    if source.suffix == ".c":
        return "c"
    if source.suffix in {".cc", ".cpp", ".cxx"}:
        return "c++"
    raise ValueError(f"unsupported benchmark source extension: {source}")


def discover_extracted(
    families: Iterable[str] = EXTRACTED_FAMILIES,
) -> list[BenchmarkCase]:
    """Discover the 100 single-file, real-project extracts."""

    requested = tuple(families)
    unknown = sorted(set(requested) - set(EXTRACTED_FAMILIES))
    if unknown:
        raise ValueError(f"unknown extracted benchmark families: {unknown}")

    cases: list[BenchmarkCase] = []
    for family in requested:
        family_root = BENCHMARK_ROOT / family

        for source in sorted(family_root.glob("*/*.c")):
            category = source.parent.name.rsplit("_", 1)[0]
            relative = source.relative_to(BENCHMARK_ROOT)
            cases.append(
                BenchmarkCase(
                    case_id=f"extracted:{relative.as_posix()}",
                    suite="extracted",
                    group=family,
                    category=category,
                    source=source,
                    language=_language_for(source),
                    # The extracts use explicit ../project.h paths.  Adding
                    # the family directory with -I would make its compatibility
                    # file named stdio.h shadow the host's real <stdio.h>.
                    include_dirs=(),
                )
            )
    return cases


def discover_juliet(cwes: Iterable[str] = JULIET_CWES) -> list[BenchmarkCase]:
    """Discover every C/C++ translation unit in the bundled Juliet subset."""

    requested = tuple(cwes)
    unknown = sorted(set(requested) - set(JULIET_CWES))
    if unknown:
        raise ValueError(f"unknown Juliet CWEs: {unknown}")

    support_dir = JULIET_ROOT / "testcasesupport"
    cases: list[BenchmarkCase] = []
    for cwe in requested:
        cwe_root = JULIET_ROOT / "testcases" / cwe
        sources = sorted(
            source
            for source in cwe_root.rglob("*")
            if source.is_file() and source.suffix in {".c", ".cpp"}
        )
        for source in sources:
            relative = source.relative_to(JULIET_ROOT / "testcases")
            cases.append(
                BenchmarkCase(
                    case_id=f"juliet:{relative.as_posix()}",
                    suite="juliet",
                    group=cwe,
                    category=cwe.split("_", 1)[1].lower(),
                    source=source,
                    language=_language_for(source),
                    include_dirs=(support_dir, source.parent),
                )
            )
    return cases


def discover_cases(
    suites: Iterable[str],
    families: Iterable[str] = EXTRACTED_FAMILIES,
    cwes: Iterable[str] = JULIET_CWES,
) -> list[BenchmarkCase]:
    requested = set(suites)
    if "all" in requested:
        requested = {"extracted", "juliet"}

    unknown = sorted(requested - {"extracted", "juliet"})
    if unknown:
        raise ValueError(f"unknown benchmark suites: {unknown}")

    cases: list[BenchmarkCase] = []
    if "extracted" in requested:
        cases.extend(discover_extracted(families))
    if "juliet" in requested:
        cases.extend(discover_juliet(cwes))
    return sorted(cases, key=lambda case: case.case_id)
