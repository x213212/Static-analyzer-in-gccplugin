"""Reviewed allocation-site oracle for the 100 extracted benchmark cases."""

from __future__ import annotations

from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
from typing import Any, Iterable

try:
    from .benchmark_manifest import REPOSITORY_ROOT, BenchmarkCase
except ImportError:  # Direct execution from tests/.
    from benchmark_manifest import REPOSITORY_ROOT, BenchmarkCase


DEFAULT_ORACLE = Path(__file__).with_name("extracted_oracle.json")


class ExtractedOracleError(ValueError):
    """The checked-in oracle and the selected fixture corpus disagree."""


@dataclass(frozen=True)
class AllocationSite:
    line: int
    original_line: int
    allocator: str
    deallocator: str
    mapping: str
    source_text: str


@dataclass(frozen=True)
class ExtractedExpectation:
    source: str
    source_sha256: str
    category: str
    target_rule: str
    allocation_sites: tuple[AllocationSite, ...]

    @property
    def target_lines(self) -> frozenset[int]:
        return frozenset(site.line for site in self.allocation_sites)


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _string(item: dict[str, Any], key: str, context: str) -> str:
    value = item.get(key)
    if not isinstance(value, str):
        raise ExtractedOracleError(f"{context}.{key} must be a string")
    return value


def _positive_int(item: dict[str, Any], key: str, context: str) -> int:
    value = item.get(key)
    if not isinstance(value, int) or isinstance(value, bool) or value < 1:
        raise ExtractedOracleError(f"{context}.{key} must be a positive integer")
    return value


def load_extracted_oracle(
    path: Path = DEFAULT_ORACLE,
) -> dict[str, ExtractedExpectation]:
    oracle_path = path.expanduser().resolve()
    try:
        document = json.loads(oracle_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise ExtractedOracleError(
            f"cannot read extracted oracle {oracle_path}: {error}"
        ) from error
    if not isinstance(document, dict) or document.get("schema_version") != 1:
        raise ExtractedOracleError("extracted oracle schema_version must be 1")
    raw_cases = document.get("cases")
    if not isinstance(raw_cases, list):
        raise ExtractedOracleError("extracted oracle cases must be an array")

    expectations: dict[str, ExtractedExpectation] = {}
    for index, raw_case in enumerate(raw_cases):
        context = f"cases[{index}]"
        if not isinstance(raw_case, dict):
            raise ExtractedOracleError(f"{context} must be an object")
        source = _string(raw_case, "source", context)
        if source in expectations:
            raise ExtractedOracleError(f"duplicate extracted oracle source: {source}")
        raw_sites = raw_case.get("allocation_sites")
        if not isinstance(raw_sites, list) or not raw_sites:
            raise ExtractedOracleError(
                f"{context}.allocation_sites must be a non-empty array"
            )
        sites: list[AllocationSite] = []
        for site_index, raw_site in enumerate(raw_sites):
            site_context = f"{context}.allocation_sites[{site_index}]"
            if not isinstance(raw_site, dict):
                raise ExtractedOracleError(f"{site_context} must be an object")
            sites.append(
                AllocationSite(
                    line=_positive_int(raw_site, "line", site_context),
                    original_line=_positive_int(
                        raw_site, "original_line", site_context
                    ),
                    allocator=_string(raw_site, "allocator", site_context),
                    deallocator=_string(raw_site, "deallocator", site_context),
                    mapping=_string(raw_site, "mapping", site_context),
                    source_text=_string(raw_site, "source_text", site_context),
                )
            )
        expectations[source] = ExtractedExpectation(
            source=source,
            source_sha256=_string(raw_case, "source_sha256", context),
            category=_string(raw_case, "category", context),
            target_rule=_string(raw_case, "target_rule", context),
            allocation_sites=tuple(sites),
        )
    return expectations


def validate_extracted_oracle(
    cases: Iterable[BenchmarkCase],
    expectations: dict[str, ExtractedExpectation],
) -> None:
    selected = [case for case in cases if case.suite == "extracted"]
    selected_sources = {
        case.repository_relative_source.as_posix() for case in selected
    }
    missing = sorted(selected_sources - expectations.keys())
    if missing:
        raise ExtractedOracleError(
            "selected extracted sources missing from oracle: " + ", ".join(missing)
        )

    for case in selected:
        source_name = case.repository_relative_source.as_posix()
        expectation = expectations[source_name]
        if expectation.category != case.category:
            raise ExtractedOracleError(
                f"oracle category mismatch for {source_name}: "
                f"{expectation.category} != {case.category}"
            )
        source_hash = _sha256(case.source)
        if source_hash != expectation.source_sha256:
            raise ExtractedOracleError(
                f"fixture hash mismatch for {source_name}; review and migrate its "
                "allocation-site oracle before benchmarking"
            )
        lines = case.source.read_text(encoding="utf-8", errors="replace").splitlines()
        for site in expectation.allocation_sites:
            if site.line > len(lines):
                raise ExtractedOracleError(
                    f"oracle line {site.line} is outside {source_name}"
                )
            if lines[site.line - 1].strip() != site.source_text:
                raise ExtractedOracleError(
                    f"oracle text mismatch at {source_name}:{site.line}; "
                    "review fixture drift instead of silently shifting the target"
                )


def evidence_matches_expectation(
    evidence: Iterable[dict[str, Any]], expectation: ExtractedExpectation
) -> bool:
    expected_name = Path(expectation.source).name
    for point in evidence:
        file_value = point.get("file")
        line_value = point.get("line")
        if not isinstance(file_value, str):
            continue
        try:
            line = int(line_value)
        except (TypeError, ValueError):
            continue
        normalized = file_value.replace("\\", "/")
        same_file = (
            normalized == expected_name
            if "/" not in normalized
            else normalized == expectation.source
            or normalized.endswith("/" + expectation.source)
        )
        if same_file and line in expectation.target_lines:
            return True
    return False
