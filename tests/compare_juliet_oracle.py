#!/usr/bin/env python3
"""Ground-truth Juliet comparison for the GCC plugin and GCC 16.2.

The unit of accuracy is a Juliet ``<testcase>`` group, not a source file.  Each
group is compiled twice: with ``OMITGOOD`` (the positive/bad program) and with
``OMITBAD`` (the negative/good program).  Multi-translation-unit groups are
compiled one translation unit at a time and then aggregated.  This reflects
the analyzers' real per-translation-unit behavior; it does not silently turn
multi-file tests into an amalgamated program.

The bundled Juliet ``manifest.xml`` is the oracle.  Primary accuracy credits a
bad program when its normalized target rule appears anywhere in the testcase
group; exact manifest file/line matching is retained as a separate localization
score because CWE-401 commonly labels a non-executable "No deallocation"
comment.  The fair native-vs-plugin headline is the common-paired C subset;
GCC 16.2 documents its analyzer as suitable only for C in this release.
"""

from __future__ import annotations

import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass
from datetime import datetime, timezone
import hashlib
import html
import json
import math
import os
from pathlib import Path
import re
import shlex
import shutil
import signal
import statistics
import subprocess
import sys
import time
from typing import Any, Iterable, Sequence
from urllib.parse import unquote, urlparse
import xml.etree.ElementTree as ET


REPOSITORY_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_JULIET_ROOT = (
    REPOSITORY_ROOT
    / "Test"
    / "benchmarks"
    / "Juliet_Test_Suite_v1.3_for_C_Cpp"
    / "C"
)
DEFAULT_MANIFEST = DEFAULT_JULIET_ROOT / "manifest.xml"

CWE_CONFIGURATION = {
    "401": {
        "name": "CWE401_Memory_Leak",
        "label": "CWE-401 Memory Leak",
        "target_rule": "SA001",
    },
    "415": {
        "name": "CWE415_Double_Free",
        "label": "CWE-415 Double Free",
        "target_rule": "SA002",
    },
    "416": {
        "name": "CWE416_Use_After_Free",
        "label": "CWE-416 Use After Free",
        "target_rule": "SA003",
    },
}
NAME_TO_CWE = {
    str(configuration["name"]): cwe
    for cwe, configuration in CWE_CONFIGURATION.items()
}
GCC_RULE_TO_PLUGIN_RULE = {
    "-Wanalyzer-malloc-leak": "SA001",
    "-Wanalyzer-double-free": "SA002",
    "-Wanalyzer-use-after-free": "SA003",
}
MEMORY_LIFETIME_RULES = frozenset(CWE_CONFIGURATION[cwe]["target_rule"] for cwe in CWE_CONFIGURATION)

TOOL_STATUSES = (
    "OK",
    "BUILD_FAIL",
    "ERROR",
    "CRASH",
    "TIMEOUT",
    "NOT_RUN",
)
STATUS_PRIORITY = {
    "OK": 0,
    "NOT_RUN": 1,
    "ERROR": 2,
    "BUILD_FAIL": 3,
    "CRASH": 4,
    "TIMEOUT": 5,
}

CRASH_RE = re.compile(
    r"internal compiler error|segmentation fault|please submit a full bug report|"
    r"fatal signal|killed signal terminated program|stack dump|bus error",
    re.IGNORECASE,
)
PLUGIN_DIAGNOSTIC_RE = re.compile(
    r"^(.*?):(\d+):(\d+):\s+(?:fatal\s+)?warning:\s+\[(SA\d{3})\]",
    re.MULTILINE,
)
PLUGIN_LOCATIONLESS_DIAGNOSTIC_RE = re.compile(
    r"^(?:cc1|cc1plus):\s+(?:fatal\s+)?warning:\s+\[(SA\d{3})\]",
    re.MULTILINE,
)

COMMON_FLAGS = (
    "-fdiagnostics-color=never",
    "-O1",
    "-fipa-pta",
    "-fno-inline",
    "-fno-tree-dce",
    "-fno-tree-dse",
    "-fno-tree-fre",
    "-pthread",
)
PLUGIN_OPTIONS = (
    "contextsentive=1",
    "debugoutput=0",
    "legacyengine=0",
    "memoryleakcheck=1",
    "doublefreecheck=1",
    "report=1",
    "reporthtml=0",
    "smt=1",
    "threadmod=0",
    "traceallfucntion=1",
    "useafterfree=1",
)
GCC_ANALYZER_FLAGS = (
    "-fanalyzer",
    "-fanalyzer-verbosity=0",
    "-fdiagnostics-format=sarif-stderr",
)

# Rotate all three measured modes, rather than always measuring the plain
# compile first. This spreads filesystem-cache, thermal, and process-order
# effects across baseline, plugin, and native analyzer runs. Formal runs assign
# sorted TU variants consecutive indices, strictly counterbalancing all orders.
TIMED_MODE_ORDERS = (
    ("baseline", "plugin", "gcc16_fanalyzer"),
    ("baseline", "gcc16_fanalyzer", "plugin"),
    ("plugin", "baseline", "gcc16_fanalyzer"),
    ("plugin", "gcc16_fanalyzer", "baseline"),
    ("gcc16_fanalyzer", "baseline", "plugin"),
    ("gcc16_fanalyzer", "plugin", "baseline"),
)


class ConfigurationError(ValueError):
    """A configuration/oracle error that must stop the benchmark."""


@dataclass(frozen=True)
class OracleSite:
    file: str
    line: int
    name: str

    def as_dict(self) -> dict[str, Any]:
        return {"file": self.file, "line": self.line, "name": self.name}


@dataclass(frozen=True)
class OracleGroup:
    group_id: str
    manifest_index: int
    cwe: str
    cwe_name: str
    target_rule: str
    sources: tuple[Path, ...]
    expected_sites: tuple[OracleSite, ...]
    flow_variant: str | None

    @property
    def tu_scope(self) -> str:
        return "single_tu" if len(self.sources) == 1 else "multi_tu"

    @property
    def searchable_text(self) -> str:
        members = " ".join(str(source) for source in self.sources)
        return f"{self.group_id} {self.cwe} {self.cwe_name} {members}"


def default_compiler(language: str) -> str:
    environment = "GCC16_CC" if language == "c" else "GCC16_CXX"
    configured = os.environ.get(environment)
    if configured:
        return configured
    local_name = "gcc" if language == "c" else "g++"
    local = Path("/root/toolchains/gcc-16.2/bin") / local_name
    return str(local) if local.is_file() else f"{local_name}-16"


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Compare a GCC 16.2 plugin and GCC 16.2 -fanalyzer against "
            "Juliet manifest flaw sites"
        )
    )
    parser.add_argument(
        "--manifest",
        type=Path,
        default=DEFAULT_MANIFEST,
        help=f"Juliet manifest.xml (default: {DEFAULT_MANIFEST})",
    )
    parser.add_argument(
        "--cwe",
        action="append",
        choices=tuple(CWE_CONFIGURATION),
        help="CWE number to include; repeatable (default: 401, 415, 416)",
    )
    parser.add_argument(
        "--language",
        choices=("c", "c++"),
        help=(
            "only run testcase groups in this language; use 'c' for the fair "
            "GCC 16.2 native-vs-plugin headline (default: both languages)"
        ),
    )
    parser.add_argument(
        "--match",
        help="select groups whose id, CWE, or member path contains this text",
    )
    parser.add_argument(
        "--limit",
        type=int,
        help="run at most this many testcase groups after deterministic filtering",
    )
    parser.add_argument("--cc", default=default_compiler("c"))
    parser.add_argument("--cxx", default=default_compiler("c++"))
    parser.add_argument(
        "--plugin",
        type=Path,
        default=REPOSITORY_ROOT / "Compiler" / "newplugin.so",
    )
    parser.add_argument(
        "--plugin-contract-header",
        type=Path,
        help=(
            "explicitly inject this trusted contract header into plugin testcase "
            "compiles only; marks the report contract-assisted and ineligible as "
            "a raw benchmark headline"
        ),
    )
    parser.add_argument(
        "--group-id-file",
        type=Path,
        help=(
            "select exact Juliet group_ids listed one per line; blank lines and "
            "# comments are ignored"
        ),
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=REPOSITORY_ROOT / "artifacts" / "juliet-oracle-comparison",
    )
    parser.add_argument("--jobs", type=int, default=min(8, os.cpu_count() or 1))
    parser.add_argument("--timeout", type=float, default=60.0)
    parser.add_argument("--verbose", action="store_true")
    return parser.parse_args(argv)


def language_for(source: Path) -> str:
    if source.suffix == ".c":
        return "c"
    if source.suffix in {".cc", ".cpp", ".cxx"}:
        return "c++"
    raise ConfigurationError(f"unsupported Juliet source extension: {source}")


def group_language(group: OracleGroup) -> str:
    languages = {language_for(source) for source in group.sources}
    return next(iter(languages)) if len(languages) == 1 else "mixed"


def repository_path(path: Path) -> str:
    try:
        return path.resolve().relative_to(REPOSITORY_ROOT).as_posix()
    except ValueError:
        return str(path.resolve())


def flow_variant_for(name: str) -> str | None:
    match = re.search(r"_(\d{2})(?:[a-z]|_(?:bad|good[^.]*))?\.(?:c|cpp)$", name)
    return match.group(1) if match else None


def discover_oracle(
    manifest_path: Path, selected_cwes: Sequence[str]
) -> tuple[list[OracleGroup], dict[str, Any]]:
    manifest = manifest_path.expanduser().resolve()
    if not manifest.is_file():
        raise ConfigurationError(f"Juliet manifest not found: {manifest}")
    testcases_root = manifest.parent / "testcases"
    if not testcases_root.is_dir():
        raise ConfigurationError(
            f"Juliet testcases directory not found beside manifest: {testcases_root}"
        )
    support_dir = manifest.parent / "testcasesupport"
    if not support_dir.is_dir():
        raise ConfigurationError(
            f"Juliet testcasesupport directory not found beside manifest: {support_dir}"
        )

    source_by_name: dict[str, Path] = {}
    generated_mains: list[Path] = []
    for cwe in selected_cwes:
        cwe_name = str(CWE_CONFIGURATION[cwe]["name"])
        cwe_root = testcases_root / cwe_name
        if not cwe_root.is_dir():
            raise ConfigurationError(f"Juliet CWE directory not found: {cwe_root}")
        for source in sorted(cwe_root.rglob("*")):
            if not source.is_file() or source.suffix not in {".c", ".cpp"}:
                continue
            if source.name in {"main.cpp", "main_linux.cpp", "main.c"}:
                generated_mains.append(source)
                continue
            previous = source_by_name.get(source.name)
            if previous is not None and previous != source:
                raise ConfigurationError(
                    "manifest uses basenames but the selected corpus contains a "
                    f"duplicate basename: {previous} and {source}"
                )
            source_by_name[source.name] = source.resolve()

    try:
        document = ET.parse(manifest)
    except (ET.ParseError, OSError) as error:
        raise ConfigurationError(f"cannot parse Juliet manifest {manifest}: {error}") from error

    groups: list[OracleGroup] = []
    used_sources: set[Path] = set()
    seen_group_ids: set[str] = set()
    for manifest_index, testcase in enumerate(document.getroot().findall("testcase")):
        file_elements = list(testcase.findall("file"))
        names = [Path(element.attrib.get("path", "")).name for element in file_elements]
        group_cwes = {
            cwe
            for cwe in selected_cwes
            if any(name.startswith(str(CWE_CONFIGURATION[cwe]["name"])) for name in names)
        }
        if not group_cwes:
            continue
        if len(group_cwes) != 1:
            raise ConfigurationError(
                f"manifest testcase {manifest_index} mixes selected CWEs: {sorted(group_cwes)}"
            )
        cwe = next(iter(group_cwes))
        cwe_name = str(CWE_CONFIGURATION[cwe]["name"])
        source_elements = [
            element
            for element in file_elements
            if Path(element.attrib.get("path", "")).suffix in {".c", ".cpp"}
            and Path(element.attrib.get("path", "")).name.startswith(cwe_name)
        ]
        if not source_elements:
            raise ConfigurationError(
                f"manifest testcase {manifest_index} has no C/C++ source members"
            )

        members: list[Path] = []
        sites: list[OracleSite] = []
        for element in source_elements:
            basename = Path(element.attrib["path"]).name
            source = source_by_name.get(basename)
            if source is None:
                raise ConfigurationError(
                    f"manifest source {basename!r} is missing under {testcases_root / cwe_name}"
                )
            members.append(source)
            used_sources.add(source)
            for flaw in element.findall("flaw"):
                try:
                    line = int(flaw.attrib["line"])
                except (KeyError, ValueError) as error:
                    raise ConfigurationError(
                        f"invalid flaw line for manifest source {basename}"
                    ) from error
                if line < 1:
                    raise ConfigurationError(
                        f"invalid non-positive flaw line for manifest source {basename}: {line}"
                    )
                sites.append(
                    OracleSite(
                        file=basename,
                        line=line,
                        name=flaw.attrib.get("name", str(CWE_CONFIGURATION[cwe]["label"])),
                    )
                )
        if not sites:
            raise ConfigurationError(
                f"selected manifest testcase {manifest_index} has no flaw site"
            )

        first_name = Path(source_elements[0].attrib["path"]).stem
        group_id = f"{cwe}:{first_name}"
        if group_id in seen_group_ids:
            group_id = f"{group_id}:manifest-{manifest_index}"
        seen_group_ids.add(group_id)
        groups.append(
            OracleGroup(
                group_id=group_id,
                manifest_index=manifest_index,
                cwe=cwe,
                cwe_name=cwe_name,
                target_rule=str(CWE_CONFIGURATION[cwe]["target_rule"]),
                sources=tuple(members),
                expected_sites=tuple(sites),
                flow_variant=flow_variant_for(source_elements[0].attrib["path"]),
            )
        )

    unused_sources = sorted(set(source_by_name.values()) - used_sources)
    if unused_sources:
        preview = ", ".join(repository_path(path) for path in unused_sources[:5])
        raise ConfigurationError(
            f"{len(unused_sources)} selected testcase sources are not represented in "
            f"manifest.xml (first: {preview})"
        )

    groups.sort(key=lambda group: group.group_id)
    inventory = {
        "manifest": str(manifest),
        "testcases_root": str(testcases_root.resolve()),
        "testcasesupport": str(support_dir.resolve()),
        "selected_cwes": list(selected_cwes),
        "groups": len(groups),
        "translation_units": sum(len(group.sources) for group in groups),
        "flaw_sites": sum(len(group.expected_sites) for group in groups),
        "single_tu_groups": sum(group.tu_scope == "single_tu" for group in groups),
        "multi_tu_groups": sum(group.tu_scope == "multi_tu" for group in groups),
        "generated_mains_excluded": [repository_path(path) for path in generated_mains],
        "generated_mains_excluded_count": len(generated_mains),
    }
    return groups, inventory


def select_groups(
    groups: Sequence[OracleGroup],
    language: str | None,
    match: str | None,
    limit: int | None,
    exact_group_ids: Sequence[str] | None = None,
) -> list[OracleGroup]:
    selected = list(groups)
    if exact_group_ids is not None:
        if len(set(exact_group_ids)) != len(exact_group_ids):
            raise ConfigurationError("exact group-id selection contains duplicate ids")
        available = {group.group_id for group in selected}
        missing = [group_id for group_id in exact_group_ids if group_id not in available]
        if missing:
            preview = ", ".join(repr(group_id) for group_id in missing[:5])
            suffix = "" if len(missing) <= 5 else f" (and {len(missing) - 5} more)"
            raise ConfigurationError(
                f"group-id file contains {len(missing)} unknown group_id(s): "
                f"{preview}{suffix}"
            )
        requested = set(exact_group_ids)
        selected = [group for group in selected if group.group_id in requested]
    if language:
        selected = [group for group in selected if group_language(group) == language]
    if match:
        needle = match.casefold()
        selected = [group for group in selected if needle in group.searchable_text.casefold()]
    if limit is not None:
        selected = selected[:limit]
    if exact_group_ids is not None and len(selected) != len(exact_group_ids):
        raise ConfigurationError(
            "--language, --match, or --limit excluded ids from the exact group-id "
            "selection; remove conflicting filters or provide the intended cohort"
        )
    if not selected:
        raise ConfigurationError("no Juliet testcase groups matched the selection")
    return selected


def group_id_selection_artifact(path: Path) -> tuple[tuple[str, ...], dict[str, Any]]:
    """Read and fingerprint an exact group-id selection file.

    Text after ``#`` is a comment.  Rejecting duplicate ids is intentional: a
    duplicate is usually a cohort-construction error that would otherwise be
    silently hidden by set-based filtering.
    """

    resolved = path.expanduser().resolve()
    if not resolved.is_file():
        raise ConfigurationError(f"group-id file not found: {resolved}")
    try:
        content = resolved.read_bytes()
        lines = content.decode("utf-8").splitlines()
    except (OSError, UnicodeDecodeError) as error:
        raise ConfigurationError(f"cannot read group-id file {resolved}: {error}") from error

    group_ids: list[str] = []
    first_line: dict[str, int] = {}
    for line_number, raw_line in enumerate(lines, 1):
        group_id = raw_line.partition("#")[0].strip()
        if not group_id:
            continue
        if group_id in first_line:
            raise ConfigurationError(
                f"duplicate group_id {group_id!r} in {resolved}: lines "
                f"{first_line[group_id]} and {line_number}"
            )
        first_line[group_id] = line_number
        group_ids.append(group_id)
    if not group_ids:
        raise ConfigurationError(f"group-id file contains no group ids: {resolved}")
    return tuple(group_ids), {
        "path": str(resolved),
        "sha256": hashlib.sha256(content).hexdigest(),
        "group_id_count": len(group_ids),
        "group_ids": group_ids,
        "policy": (
            "UTF-8, one exact Juliet group_id per line; surrounding whitespace, "
            "blank lines, and text beginning with # are ignored; duplicates and "
            "unknown ids are fatal"
        ),
    }


def contract_header_artifact(path: Path) -> dict[str, Any]:
    resolved = path.expanduser().resolve()
    if not resolved.is_file():
        raise ConfigurationError(f"plugin contract header not found: {resolved}")
    return {
        "path": str(resolved),
        "sha256": sha256_file(resolved),
        "applies_to": ["plugin"],
        "policy": (
            "explicit trusted semantics supplied by the benchmark operator; "
            "forced include in plugin testcase compiles only, excluded from "
            "baseline and native GCC analyzer compiles; contract-assisted "
            "results are NOT a raw benchmark headline"
        ),
    }


def benchmark_mode(contract_assisted: bool) -> dict[str, Any]:
    return {
        "kind": "contract-assisted" if contract_assisted else "raw",
        "raw_headline_eligible": not contract_assisted,
        "label": (
            "Contract-assisted comparison — NOT a raw benchmark headline"
            if contract_assisted
            else "Raw native-vs-plugin comparison"
        ),
    }


def resolve_executable(candidate: str) -> str:
    resolved = shutil.which(candidate)
    if resolved is None:
        path = Path(candidate).expanduser()
        if path.is_file() and os.access(path, os.X_OK):
            resolved = str(path)
    if resolved is None:
        raise ConfigurationError(f"compiler not found or not executable: {candidate}")
    return str(Path(resolved).resolve())


def probe(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    try:
        return subprocess.run(
            list(command),
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=15.0,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        raise ConfigurationError(f"compiler probe failed: {shlex.join(command)}: {error}") from error


def compiler_identity(executable: str) -> dict[str, str]:
    version_probe = probe([executable, "-dumpfullversion", "-dumpversion"])
    if version_probe.returncode != 0:
        detail = version_probe.stderr.strip() or f"exit {version_probe.returncode}"
        raise ConfigurationError(f"cannot determine compiler version: {detail}")
    words = version_probe.stdout.strip().split()
    if not words:
        raise ConfigurationError(f"compiler version probe returned no output: {executable}")
    version = words[0]
    if re.fullmatch(
        r"16\.2\.\d+(?:\.\d+)*(?:[-+~][A-Za-z0-9._+~-]+)?", version
    ) is None:
        raise ConfigurationError(
            f"GCC 16.2.x is required, but {executable} reports {version}"
        )
    banner_probe = probe([executable, "--version"])
    banner_lines = (banner_probe.stdout or banner_probe.stderr).splitlines()
    target_probe = probe([executable, "-dumpmachine"])
    if target_probe.returncode != 0 or not target_probe.stdout.strip():
        raise ConfigurationError(f"cannot determine compiler target: {executable}")
    return {
        "executable": executable,
        "version": version,
        "target": target_probe.stdout.strip(),
        "banner": banner_lines[0] if banner_lines else "unknown",
    }


def validate_toolchains(
    args: argparse.Namespace, groups: Sequence[OracleGroup]
) -> tuple[dict[str, str], dict[str, dict[str, str]]]:
    languages = {language_for(source) for group in groups for source in group.sources}
    requested = {"c": args.cc, "c++": args.cxx}
    compilers: dict[str, str] = {}
    identities: dict[str, dict[str, str]] = {}
    for language in sorted(languages):
        compilers[language] = resolve_executable(requested[language])
        identities[language] = compiler_identity(compilers[language])
    if len(identities) > 1:
        versions = {identity["version"] for identity in identities.values()}
        targets = {identity["target"] for identity in identities.values()}
        if len(versions) != 1:
            raise ConfigurationError(
                f"C and C++ drivers are not the same GCC 16.2.x release: {sorted(versions)}"
            )
        if len(targets) != 1:
            raise ConfigurationError(
                f"C and C++ drivers target different platforms: {sorted(targets)}"
            )
    return compilers, identities


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def source_selection_artifact(
    groups: Sequence[OracleGroup], corpus_root: Path
) -> dict[str, Any]:
    """Fingerprint every source TU in the selected testcase-group population.

    The aggregate deliberately uses paths relative to the Juliet corpus, rather
    than checkout-specific absolute paths. Group and source ordering are also
    canonicalized so callers get the same fingerprint for the same selection.
    """

    root = corpus_root.expanduser().resolve()
    entries: list[dict[str, str]] = []
    for group in sorted(groups, key=lambda item: item.group_id):
        for source in sorted(group.sources, key=lambda item: item.name):
            resolved = source.resolve()
            try:
                relative_source = resolved.relative_to(root).as_posix()
            except ValueError as error:
                raise ConfigurationError(
                    f"selected Juliet source is outside corpus root {root}: {resolved}"
                ) from error
            entries.append(
                {
                    "group_id": group.group_id,
                    "source": relative_source,
                    "sha256": sha256_file(resolved),
                }
            )
    canonical = json.dumps(
        entries, sort_keys=True, separators=(",", ":"), ensure_ascii=False
    ).encode("utf-8")
    return {
        "translation_unit_count": len(entries),
        "sha256": hashlib.sha256(canonical).hexdigest(),
        "policy": (
            "SHA-256 of canonical sorted testcase-group id, corpus-relative "
            "source path, and source SHA-256"
        ),
        "sources": entries,
    }


def verify_input_provenance_unchanged(
    groups: Sequence[OracleGroup],
    corpus_root: Path,
    initial_sources: dict[str, Any],
    manifest_path: Path,
    initial_manifest_sha256: str,
    runner_path: Path,
    initial_runner_sha256: str,
    *,
    plugin_contract_header: Path | None = None,
    initial_plugin_contract_header_sha256: str | None = None,
    group_id_file: Path | None = None,
    initial_group_id_file_sha256: str | None = None,
) -> dict[str, Any]:
    """Fail closed if any formal benchmark input changed after it was cached."""

    final_sources = source_selection_artifact(groups, corpus_root)
    final_manifest_sha256 = sha256_file(manifest_path)
    final_runner_sha256 = sha256_file(runner_path)
    changed: list[str] = []
    if final_sources["sha256"] != initial_sources["sha256"]:
        changed.append("selected source set")
    if final_manifest_sha256 != initial_manifest_sha256:
        changed.append("Juliet manifest")
    if final_runner_sha256 != initial_runner_sha256:
        changed.append("benchmark runner")

    final_plugin_contract_header_sha256: str | None = None
    if plugin_contract_header is not None:
        if initial_plugin_contract_header_sha256 is None:
            raise ConfigurationError(
                "plugin contract header provenance is missing its initial SHA-256"
            )
        try:
            final_plugin_contract_header_sha256 = sha256_file(plugin_contract_header)
        except OSError:
            changed.append("plugin contract header")
        else:
            if (
                final_plugin_contract_header_sha256
                != initial_plugin_contract_header_sha256
            ):
                changed.append("plugin contract header")
    elif initial_plugin_contract_header_sha256 is not None:
        raise ConfigurationError(
            "plugin contract header SHA-256 was supplied without a header path"
        )

    final_group_id_file_sha256: str | None = None
    if group_id_file is not None:
        if initial_group_id_file_sha256 is None:
            raise ConfigurationError(
                "group-id file provenance is missing its initial SHA-256"
            )
        try:
            final_group_id_file_sha256 = sha256_file(group_id_file)
        except OSError:
            changed.append("group-id file")
        else:
            if final_group_id_file_sha256 != initial_group_id_file_sha256:
                changed.append("group-id file")
    elif initial_group_id_file_sha256 is not None:
        raise ConfigurationError(
            "group-id file SHA-256 was supplied without a file path"
        )

    if changed:
        raise ConfigurationError(
            "benchmark input drift detected ("
            + ", ".join(changed)
            + "); no valid comparison report was written"
        )
    verified = {
        "selected_source_set_sha256": final_sources["sha256"],
        "manifest_sha256": final_manifest_sha256,
        "runner_sha256": final_runner_sha256,
    }
    if final_plugin_contract_header_sha256 is not None:
        verified["plugin_contract_header_sha256"] = (
            final_plugin_contract_header_sha256
        )
    if final_group_id_file_sha256 is not None:
        verified["group_id_file_sha256"] = final_group_id_file_sha256
    return verified


def snapshot_plugin(plugin: Path, output: Path) -> tuple[Path, str, Path]:
    original = plugin.expanduser().resolve()
    if not original.is_file():
        raise ConfigurationError(f"plugin does not exist: {original}")
    digest = sha256_file(original)
    snapshot_dir = output / "plugin-snapshot" / digest[:12]
    snapshot_dir.mkdir(parents=True, exist_ok=True)
    snapshot = snapshot_dir / original.name
    if not snapshot.exists() or sha256_file(snapshot) != digest:
        temporary = snapshot.with_name(f".{snapshot.name}.{os.getpid()}.tmp")
        shutil.copy2(original, temporary)
        temporary.replace(snapshot)
    if sha256_file(snapshot) != digest:
        raise ConfigurationError(f"plugin snapshot hash mismatch: {snapshot}")
    return snapshot.resolve(), digest, original


def standard_for(language: str) -> str:
    return "-std=gnu11" if language == "c" else "-std=gnu++14"


def plugin_arguments(plugin: Path, report_stem: Path) -> list[str]:
    name = plugin.stem
    arguments = [f"-fplugin-arg-{name}-{option}" for option in PLUGIN_OPTIONS]
    arguments.append(f"-fplugin-arg-{name}-reportpath={report_stem.resolve()}")
    return arguments


def process_status(returncode: int | None, stderr: str, timed_out: bool) -> str:
    if timed_out:
        return "TIMEOUT"
    if returncode is not None and returncode < 0:
        return "CRASH"
    if CRASH_RE.search(stderr):
        return "CRASH"
    if returncode != 0:
        return "ERROR"
    return "OK"


def run_process(command: Sequence[str], cwd: Path, timeout: float) -> dict[str, Any]:
    started = time.monotonic()
    try:
        process = subprocess.Popen(
            list(command),
            cwd=cwd,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            start_new_session=True,
        )
    except OSError as error:
        return {
            "status": "ERROR",
            "exit_code": None,
            "timed_out": False,
            "duration_ms": round((time.monotonic() - started) * 1000, 2),
            "stdout": "",
            "stderr": f"failed to start compiler: {error}\n",
        }

    timed_out = False
    try:
        stdout, stderr = process.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        timed_out = True
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        stdout, stderr = process.communicate()
    return {
        "status": process_status(process.returncode, stderr, timed_out),
        "exit_code": process.returncode,
        "timed_out": timed_out,
        "duration_ms": round((time.monotonic() - started) * 1000, 2),
        "stdout": stdout,
        "stderr": stderr,
    }


def write_log(
    path: Path, command: Sequence[str], stdout: str, stderr: str
) -> None:
    content = "$ " + shlex.join(command) + "\n"
    if stdout:
        content += "\n[stdout]\n" + stdout
    if stderr:
        content += "\n[stderr]\n" + stderr
    path.write_text(content, encoding="utf-8", errors="replace")


def final_process_result(
    execution: dict[str, Any], command: Sequence[str], log: Path
) -> tuple[dict[str, Any], str]:
    stdout = str(execution.pop("stdout"))
    stderr = str(execution.pop("stderr"))
    write_log(log, command, stdout, stderr)
    return (
        {
            **execution,
            "command": list(command),
            "log": str(log.resolve()),
            "diagnostic": stderr.strip()[:500] or None,
        },
        stderr,
    )


def base_command(
    compiler: str, language: str, source: Path, variant_macro: str
) -> list[str]:
    support = DEFAULT_JULIET_ROOT / "testcasesupport"
    # If a non-default manifest is selected, the caller replaces the support
    # path by setting JULIET_ROOT in each command through source ancestry.  The
    # bundled layout is the benchmark contract and is validated up front.
    manifest_support = next(
        (
            parent / "testcasesupport"
            for parent in source.parents
            if (parent / "testcasesupport").is_dir()
        ),
        support,
    )
    return [
        compiler,
        standard_for(language),
        *COMMON_FLAGS,
        f"-D{variant_macro}",
        "-I",
        str(manifest_support.resolve()),
        "-I",
        str(source.parent.resolve()),
    ]


def site(file_value: Any, line_value: Any, role: str) -> dict[str, Any] | None:
    if not isinstance(file_value, str):
        return None
    try:
        line = int(line_value)
    except (TypeError, ValueError):
        return None
    if line < 1:
        return None
    return {"file": file_value, "line": line, "role": role}


def normalize_artifact_uri(value: Any) -> str:
    if not isinstance(value, str):
        return ""
    parsed = urlparse(value)
    if parsed.scheme == "file":
        return unquote(parsed.path)
    return unquote(value)


def deduplicate_evidence(evidence: Iterable[dict[str, Any]]) -> list[dict[str, Any]]:
    result: list[dict[str, Any]] = []
    seen: set[tuple[str, int, str]] = set()
    for point in evidence:
        key = (str(point.get("file", "")), int(point.get("line", 0)), str(point.get("role", "")))
        if key in seen:
            continue
        seen.add(key)
        result.append(point)
    return result


def parse_plugin_diagnostics(stderr: str) -> list[dict[str, Any]]:
    findings: list[dict[str, Any]] = []
    for match in PLUGIN_DIAGNOSTIC_RE.finditer(stderr):
        primary = site(match.group(1), match.group(2), "diagnostic-primary")
        if primary is None:
            continue
        findings.append(
            {
                "rule_id": match.group(4),
                "native_rule_id": match.group(4),
                "message": None,
                "primary": primary,
                "evidence": [primary],
                "origin": "stderr-diagnostic",
            }
        )
    for match in PLUGIN_LOCATIONLESS_DIAGNOSTIC_RE.finditer(stderr):
        findings.append(
            {
                "rule_id": match.group(1),
                "native_rule_id": match.group(1),
                "message": None,
                "primary": None,
                "evidence": [],
                "origin": "stderr-diagnostic-locationless",
            }
        )
    return findings


def parse_plugin_report(
    report_path: Path, stderr: str
) -> tuple[list[dict[str, Any]], dict[str, Any]]:
    diagnostic_findings = parse_plugin_diagnostics(stderr)
    metadata: dict[str, Any] = {
        "path": str(report_path.resolve()),
        "diagnostic_finding_count": len(diagnostic_findings),
        "parse_error": None,
    }
    if not report_path.is_file():
        metadata["parse_error"] = "plugin JSON report was not created"
        return diagnostic_findings, metadata
    try:
        document = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        metadata["parse_error"] = f"invalid plugin JSON report: {error}"
        return diagnostic_findings, metadata
    raw_findings = document.get("findings") if isinstance(document, dict) else None
    if not isinstance(raw_findings, list):
        metadata["parse_error"] = "plugin JSON report has no findings array"
        return diagnostic_findings, metadata

    findings: list[dict[str, Any]] = []
    for raw in raw_findings:
        if not isinstance(raw, dict) or not isinstance(raw.get("rule_id"), str):
            continue
        location = raw.get("location", {})
        primary = (
            site(location.get("file"), location.get("line"), "primary")
            if isinstance(location, dict)
            else None
        )
        evidence: list[dict[str, Any]] = []
        if primary is not None:
            evidence.append(primary)
        trace = raw.get("trace", [])
        if isinstance(trace, list):
            for point in trace:
                if not isinstance(point, dict):
                    continue
                trace_site = site(
                    point.get("file"),
                    point.get("line"),
                    f"trace:{point.get('role', 'unspecified')}",
                )
                if trace_site is not None:
                    evidence.append(trace_site)
        findings.append(
            {
                "rule_id": raw["rule_id"],
                "native_rule_id": raw["rule_id"],
                "message": raw.get("message"),
                "primary": primary,
                "evidence": deduplicate_evidence(evidence),
                "origin": "plugin-json",
            }
        )
    metadata["json_finding_count"] = len(findings)
    def signature(finding: dict[str, Any]) -> tuple[Any, str, int]:
        primary = finding.get("primary") or {}
        return (
            finding.get("rule_id"),
            basename(str(primary.get("file", ""))),
            int(primary.get("line", 0)),
        )

    stderr_signature = Counter(signature(finding) for finding in diagnostic_findings)
    json_signature = Counter(signature(finding) for finding in findings)
    metadata["stderr_json_count_agree"] = len(diagnostic_findings) == len(findings)
    metadata["stderr_json_findings_agree"] = stderr_signature == json_signature
    metadata["stderr_finding_signature"] = {
        f"{rule}@{file}:{line}": count
        for (rule, file, line), count in sorted(stderr_signature.items())
    }
    metadata["json_finding_signature"] = {
        f"{rule}@{file}:{line}": count
        for (rule, file, line), count in sorted(json_signature.items())
    }
    metadata["schema_version"] = document.get("schema_version") if isinstance(document, dict) else None
    metadata["tool"] = document.get("tool") if isinstance(document, dict) else None
    metadata["analysis"] = document.get("analysis") if isinstance(document, dict) else None
    return findings, metadata


def iter_sarif_results(document: Any) -> Iterable[dict[str, Any]]:
    if not isinstance(document, dict):
        return
    runs = document.get("runs", [])
    if not isinstance(runs, list):
        return
    for run in runs:
        if not isinstance(run, dict):
            continue
        results = run.get("results", [])
        if not isinstance(results, list):
            continue
        for result in results:
            if isinstance(result, dict):
                yield result


def sarif_location(
    location: Any, role: str
) -> dict[str, Any] | None:
    if not isinstance(location, dict):
        return None
    physical = location.get("physicalLocation")
    if not isinstance(physical, dict):
        return None
    artifact = physical.get("artifactLocation")
    region = physical.get("region")
    if not isinstance(artifact, dict) or not isinstance(region, dict):
        return None
    return site(
        normalize_artifact_uri(artifact.get("uri")),
        region.get("startLine"),
        role,
    )


def parse_gcc_sarif(text: str) -> tuple[list[dict[str, Any]], dict[str, Any]]:
    metadata: dict[str, Any] = {"parse_error": None, "all_analyzer_rule_ids": {}}
    try:
        document = json.loads(text)
    except json.JSONDecodeError as error:
        metadata["parse_error"] = f"invalid SARIF JSON: {error}"
        return [], metadata
    if not isinstance(document, dict) or not isinstance(document.get("runs"), list):
        metadata["parse_error"] = "SARIF root has no runs array"
        return [], metadata

    rule_counts: Counter[str] = Counter()
    findings: list[dict[str, Any]] = []
    for raw in iter_sarif_results(document):
        native_rule = raw.get("ruleId")
        if not isinstance(native_rule, str) or not native_rule.startswith("-Wanalyzer-"):
            continue
        rule_counts[native_rule] += 1
        normalized_rule = GCC_RULE_TO_PLUGIN_RULE.get(native_rule)
        evidence: list[dict[str, Any]] = []
        primary: dict[str, Any] | None = None
        locations = raw.get("locations", [])
        if isinstance(locations, list):
            for index, location in enumerate(locations):
                point = sarif_location(location, "primary" if index == 0 else "related")
                if point is not None:
                    if primary is None:
                        primary = point
                    evidence.append(point)
        related_locations = raw.get("relatedLocations", [])
        if isinstance(related_locations, list):
            for location in related_locations:
                point = sarif_location(location, "related")
                if point is not None:
                    evidence.append(point)
        code_flows = raw.get("codeFlows", [])
        if isinstance(code_flows, list):
            for code_flow in code_flows:
                if not isinstance(code_flow, dict):
                    continue
                thread_flows = code_flow.get("threadFlows", [])
                if not isinstance(thread_flows, list):
                    continue
                for thread_flow in thread_flows:
                    if not isinstance(thread_flow, dict):
                        continue
                    flow_locations = thread_flow.get("locations", [])
                    if not isinstance(flow_locations, list):
                        continue
                    for flow_location in flow_locations:
                        if not isinstance(flow_location, dict):
                            continue
                        point = sarif_location(flow_location.get("location"), "codeflow")
                        if point is not None:
                            evidence.append(point)
        message = raw.get("message")
        findings.append(
            {
                "rule_id": normalized_rule,
                "native_rule_id": native_rule,
                "message": message.get("text") if isinstance(message, dict) else None,
                "primary": primary,
                "evidence": deduplicate_evidence(evidence),
                "origin": "gcc-sarif",
                "mapped_to_target_vocabulary": normalized_rule is not None,
            }
        )
    metadata["all_analyzer_rule_ids"] = dict(sorted(rule_counts.items()))
    metadata["finding_count"] = len(findings)
    return findings, metadata


def placeholder_tool(status: str, reason: str) -> dict[str, Any]:
    return {
        "status": status,
        "exit_code": None,
        "timed_out": status == "TIMEOUT",
        "duration_ms": 0.0,
        "command": [],
        "log": None,
        "diagnostic": reason,
        "findings": [],
        "parse": {"parse_error": "not run"},
        "not_run_reason": reason,
    }


def run_baseline(
    command_prefix: Sequence[str], source: Path, run_dir: Path, timeout: float
) -> dict[str, Any]:
    object_file = run_dir / "baseline.o"
    log = run_dir / "baseline.log"
    object_file.unlink(missing_ok=True)
    command = [*command_prefix, "-c", str(source.resolve()), "-o", str(object_file.resolve())]
    execution = run_process(command, run_dir, timeout)
    result, _ = final_process_result(execution, command, log)
    if result["status"] == "ERROR":
        result["status"] = "BUILD_FAIL"
    object_file.unlink(missing_ok=True)
    return result


def run_plugin(
    command_prefix: Sequence[str],
    plugin: Path,
    source: Path,
    run_dir: Path,
    timeout: float,
    plugin_contract_header: Path | None = None,
) -> dict[str, Any]:
    tool_dir = run_dir / "plugin"
    tool_dir.mkdir(parents=True, exist_ok=True)
    object_file = tool_dir / "case.o"
    log = tool_dir / "compiler.log"
    report_stem = tool_dir / "analysis"
    report_json = report_stem.with_suffix(".json")
    report_html = report_stem.with_suffix(".html")
    for stale in (object_file, report_json, report_html):
        stale.unlink(missing_ok=True)
    contract_arguments = (
        ["-include", str(plugin_contract_header.resolve())]
        if plugin_contract_header is not None
        else []
    )
    command = [
        *command_prefix,
        *contract_arguments,
        f"-fplugin={plugin}",
        *plugin_arguments(plugin, report_stem),
        "-c",
        str(source.resolve()),
        "-o",
        str(object_file.resolve()),
    ]
    execution = run_process(command, tool_dir, timeout)
    result, stderr = final_process_result(execution, command, log)
    findings, parse_metadata = parse_plugin_report(report_json, stderr)
    if result["status"] == "OK" and parse_metadata.get("parse_error"):
        result["status"] = "ERROR"
        result["diagnostic"] = str(parse_metadata["parse_error"])
    elif result["status"] == "OK" and not parse_metadata.get(
        "stderr_json_findings_agree", False
    ):
        result["status"] = "ERROR"
        result["diagnostic"] = (
            "plugin JSON findings disagree with emitted GCC diagnostics; "
            "see parse metadata and compiler log"
        )
    result.update(
        {
            "findings": findings,
            "parse": parse_metadata,
            "report_json": str(report_json.resolve()),
            "report_html": str(report_html.resolve()),
        }
    )
    object_file.unlink(missing_ok=True)
    return result


def run_gcc_analyzer(
    command_prefix: Sequence[str], source: Path, run_dir: Path, timeout: float
) -> dict[str, Any]:
    tool_dir = run_dir / "gcc16-fanalyzer"
    tool_dir.mkdir(parents=True, exist_ok=True)
    object_file = tool_dir / "case.o"
    log = tool_dir / "compiler.log"
    sarif_path = tool_dir / "diagnostics.sarif"
    for stale in (object_file, sarif_path):
        stale.unlink(missing_ok=True)
    command = [
        *command_prefix,
        *GCC_ANALYZER_FLAGS,
        "-c",
        str(source.resolve()),
        "-o",
        str(object_file.resolve()),
    ]
    execution = run_process(command, tool_dir, timeout)
    result, stderr = final_process_result(execution, command, log)
    sarif_path.write_text(stderr, encoding="utf-8", errors="replace")
    findings, parse_metadata = parse_gcc_sarif(stderr)
    if result["status"] == "OK" and parse_metadata.get("parse_error"):
        result["status"] = "ERROR"
        result["diagnostic"] = str(parse_metadata["parse_error"])
    result.update(
        {
            "findings": findings,
            "parse": parse_metadata,
            "sarif": str(sarif_path.resolve()),
        }
    )
    object_file.unlink(missing_ok=True)
    return result


def group_directory(output: Path, group: OracleGroup) -> Path:
    digest = hashlib.sha256(group.group_id.encode("utf-8")).hexdigest()[:12]
    readable = re.sub(r"[^A-Za-z0-9_.-]+", "_", group.group_id)[:100]
    return output / "groups" / group.cwe / f"{readable}-{digest}"


def unit_directory(group_dir: Path, variant: str, source: Path) -> Path:
    digest = hashlib.sha256(str(source).encode("utf-8")).hexdigest()[:10]
    return group_dir / variant / f"{source.stem[:90]}-{digest}"


def timed_mode_order(
    group_id: str,
    variant: str,
    source: Path,
    rotation_index: int | None = None,
) -> list[str]:
    """Return one deterministic, path-independent measured-mode permutation.

    The formal runner supplies a global sequential TU-variant index, yielding
    exact round-robin counterbalancing. The stable basename fallback preserves
    compatibility for direct/library callers without consulting an absolute path.
    """

    if rotation_index is None:
        rotation_index = int.from_bytes(
            hashlib.sha256(
                f"{group_id}\0{variant}\0{source.name}".encode("utf-8")
            ).digest()[:8],
            "big",
        )
    return list(TIMED_MODE_ORDERS[rotation_index % len(TIMED_MODE_ORDERS)])


def group_rotation_start_indices(
    groups: Sequence[OracleGroup],
) -> dict[str, int]:
    """Assign contiguous global TU-variant indices to sorted selected groups."""

    starts: dict[str, int] = {}
    next_index = 0
    for group in sorted(groups, key=lambda item: item.group_id):
        if group.group_id in starts:
            raise ConfigurationError(f"duplicate selected group id: {group.group_id}")
        starts[group.group_id] = next_index
        next_index += 2 * len(group.sources)
    return starts


def timed_order_histogram(tu_variant_count: int) -> dict[str, int]:
    """Return scheduled counts for the six round-robin measured-mode orders."""

    counts = Counter(
        " > ".join(TIMED_MODE_ORDERS[index % len(TIMED_MODE_ORDERS)])
        for index in range(tu_variant_count)
    )
    return {
        " > ".join(order): counts[" > ".join(order)]
        for order in TIMED_MODE_ORDERS
    }


def run_unit(
    args: argparse.Namespace,
    group: OracleGroup,
    variant: str,
    variant_macro: str,
    source: Path,
    compilers: dict[str, str],
    plugin: Path,
    group_dir: Path,
    rotation_index: int | None = None,
) -> dict[str, Any]:
    run_dir = unit_directory(group_dir, variant, source)
    run_dir.mkdir(parents=True, exist_ok=True)
    language = language_for(source)
    prefix = base_command(compilers[language], language, source, variant_macro)
    source_sha256 = sha256_file(source)
    execution_order = timed_mode_order(
        group.group_id, variant, source, rotation_index
    )
    tool_results: dict[str, dict[str, Any]] = {}
    for tool in execution_order:
        if tool == "baseline":
            tool_results[tool] = run_baseline(prefix, source, run_dir, args.timeout)
        elif tool == "plugin":
            tool_results[tool] = run_plugin(
                prefix,
                plugin,
                source,
                run_dir,
                args.timeout,
                getattr(args, "plugin_contract_header", None),
            )
        else:
            tool_results[tool] = run_gcc_analyzer(
                prefix, source, run_dir, args.timeout
            )

    baseline = tool_results["baseline"]
    if baseline["status"] != "OK":
        # Keep the baseline gate used by the accuracy model.  The analyzers may
        # already have run because the timing rotation put them first; retain
        # those executions for audit, but do not credit an invalid fixture.
        reason = f"unassessed because baseline status is {baseline['status']}"
        plugin_result = placeholder_tool("NOT_RUN", reason)
        plugin_result["discarded_execution"] = tool_results["plugin"]
        gcc_result = placeholder_tool("NOT_RUN", reason)
        gcc_result["discarded_execution"] = tool_results["gcc16_fanalyzer"]
    else:
        plugin_result = tool_results["plugin"]
        gcc_result = tool_results["gcc16_fanalyzer"]
    if sha256_file(source) != source_sha256:
        raise ConfigurationError(
            f"selected source changed during execution: {repository_path(source)}"
        )
    return {
        "source": repository_path(source),
        "source_basename": source.name,
        "source_sha256": source_sha256,
        "language": language,
        "variant": variant,
        "macro": variant_macro,
        "timed_execution_order": execution_order,
        "timed_rotation_index": rotation_index,
        "baseline": baseline,
        "plugin": plugin_result,
        "gcc16_fanalyzer": gcc_result,
    }


def aggregate_status(statuses: Iterable[str]) -> str:
    values = list(statuses)
    if not values:
        return "NOT_RUN"
    return max(values, key=lambda status: STATUS_PRIORITY.get(status, 99))


def basename(value: str) -> str:
    normalized = value.replace("\\", "/")
    return normalized.rsplit("/", 1)[-1]


def matching_sites(
    finding: dict[str, Any], expected_sites: Sequence[OracleSite]
) -> list[dict[str, Any]]:
    matched: list[dict[str, Any]] = []
    for expected in expected_sites:
        for evidence in finding.get("evidence", []):
            if (
                basename(str(evidence.get("file", ""))) == expected.file
                and int(evidence.get("line", 0)) == expected.line
            ):
                matched.append(
                    {
                        "expected": expected.as_dict(),
                        "evidence": evidence,
                    }
                )
                break
    return matched


def aggregate_tool_variant(
    units: Sequence[dict[str, Any]],
    tool: str,
    variant: str,
    target_rule: str,
    expected_sites: Sequence[OracleSite],
) -> dict[str, Any]:
    statuses = Counter(str(unit[tool]["status"]) for unit in units)
    status = aggregate_status(unit[tool]["status"] for unit in units)
    findings: list[dict[str, Any]] = []
    for unit in units:
        for original in unit[tool].get("findings", []):
            finding = dict(original)
            finding["translation_unit"] = unit["source"]
            finding["matched_expected_sites"] = matching_sites(finding, expected_sites)
            findings.append(finding)
    target_findings = [finding for finding in findings if finding.get("rule_id") == target_rule]
    memory_lifetime_findings = [
        finding
        for finding in findings
        if finding.get("rule_id") in MEMORY_LIFETIME_RULES
    ]
    matched = [finding for finding in target_findings if finding["matched_expected_sites"]]
    unmatched = [finding for finding in target_findings if not finding["matched_expected_sites"]]
    wrong_rule = [
        finding
        for finding in findings
        if finding.get("rule_id") is not None and finding.get("rule_id") != target_rule
    ]
    unmapped = [finding for finding in findings if finding.get("rule_id") is None]

    if status != "OK":
        classification = "UNASSESSED"
        case_detection_classification = "UNASSESSED"
        alert_burden_classification = "UNASSESSED"
    elif variant == "bad":
        classification = "TP" if matched else "FN"
        case_detection_classification = "TP" if target_findings else "FN"
        alert_burden_classification = case_detection_classification
    else:
        classification = "FP" if target_findings else "TN"
        case_detection_classification = classification
        # A warning for another memory-lifetime CWE is not evidence that this
        # testcase's target CWE was detected.  It is still an alert on a
        # manifest-defined good variant and therefore matters when comparing
        # false-positive/triage burden.  Report this stricter view separately.
        alert_burden_classification = (
            "FP" if memory_lifetime_findings else "TN"
        )

    return {
        "status": status,
        "translation_unit_statuses": {
            status_name: statuses[status_name] for status_name in TOOL_STATUSES
        },
        "classification": classification,
        "case_detection_classification": case_detection_classification,
        "alert_burden_classification": alert_burden_classification,
        "target_rule": target_rule,
        "mapped_finding_count": sum(finding.get("rule_id") is not None for finding in findings),
        "target_finding_count": len(target_findings),
        "memory_lifetime_finding_count": len(memory_lifetime_findings),
        "matched_target_findings": matched,
        "unmatched_target_findings": unmatched,
        "wrong_rule_findings": wrong_rule,
        "unmapped_analyzer_findings": unmapped,
        "duration_ms": round(sum(float(unit[tool]["duration_ms"]) for unit in units), 2),
    }


def run_group(
    args: argparse.Namespace,
    group: OracleGroup,
    compilers: dict[str, str],
    plugin: Path,
    rotation_start_index: int | None = None,
) -> dict[str, Any]:
    group_dir = group_directory(args.output, group)
    group_dir.mkdir(parents=True, exist_ok=True)
    variants: dict[str, Any] = {}
    ordered_sources = sorted(group.sources, key=lambda source: source.name)
    for variant_index, (variant, macro) in enumerate(
        (("bad", "OMITGOOD"), ("good", "OMITBAD"))
    ):
        units = [
            run_unit(
                args,
                group,
                variant,
                macro,
                source,
                compilers,
                plugin,
                group_dir,
                (
                    rotation_start_index
                    + variant_index * len(ordered_sources)
                    + source_index
                    if rotation_start_index is not None
                    else None
                ),
            )
            for source_index, source in enumerate(ordered_sources)
        ]
        variants[variant] = {
            "macro": macro,
            "baseline_status": aggregate_status(
                unit["baseline"]["status"] for unit in units
            ),
            "baseline_duration_ms": round(
                sum(float(unit["baseline"]["duration_ms"]) for unit in units), 2
            ),
            "plugin": aggregate_tool_variant(
                units, "plugin", variant, group.target_rule, group.expected_sites
            ),
            "gcc16_fanalyzer": aggregate_tool_variant(
                units,
                "gcc16_fanalyzer",
                variant,
                group.target_rule,
                group.expected_sites,
            ),
            "translation_units": units,
        }
    result_language = group_language(group)
    return {
        "group_id": group.group_id,
        "manifest_index": group.manifest_index,
        "cwe": group.cwe,
        "cwe_name": group.cwe_name,
        "target_rule": group.target_rule,
        "flow_variant": group.flow_variant,
        "tu_scope": group.tu_scope,
        "language": result_language,
        "source_count": len(group.sources),
        "sources": [repository_path(source) for source in group.sources],
        "expected_sites": [site.as_dict() for site in group.expected_sites],
        "label_source": "Juliet manifest.xml <flaw file,line>",
        "variants": variants,
        "artifact_directory": str(group_dir.resolve()),
    }


def failed_group(group: OracleGroup, output: Path, error: BaseException) -> dict[str, Any]:
    group_dir = group_directory(output, group)
    group_dir.mkdir(parents=True, exist_ok=True)
    log = group_dir / "runner-error.log"
    log.write_text(f"{type(error).__name__}: {error}\n", encoding="utf-8")
    variants: dict[str, Any] = {}
    for variant, macro in (("bad", "OMITGOOD"), ("good", "OMITBAD")):
        tool = {
            "status": "ERROR",
            "translation_unit_statuses": {
                status: (len(group.sources) if status == "ERROR" else 0)
                for status in TOOL_STATUSES
            },
            "classification": "UNASSESSED",
            "case_detection_classification": "UNASSESSED",
            "alert_burden_classification": "UNASSESSED",
            "target_rule": group.target_rule,
            "mapped_finding_count": 0,
            "target_finding_count": 0,
            "memory_lifetime_finding_count": 0,
            "matched_target_findings": [],
            "unmatched_target_findings": [],
            "wrong_rule_findings": [],
            "unmapped_analyzer_findings": [],
            "duration_ms": 0.0,
            "runner_error_log": str(log.resolve()),
        }
        variants[variant] = {
            "macro": macro,
            "baseline_status": "ERROR",
            "baseline_duration_ms": 0.0,
            "plugin": dict(tool),
            "gcc16_fanalyzer": dict(tool),
            "translation_units": [],
        }
    result_language = group_language(group)
    return {
        "group_id": group.group_id,
        "manifest_index": group.manifest_index,
        "cwe": group.cwe,
        "cwe_name": group.cwe_name,
        "target_rule": group.target_rule,
        "flow_variant": group.flow_variant,
        "tu_scope": group.tu_scope,
        "language": result_language,
        "source_count": len(group.sources),
        "sources": [repository_path(source) for source in group.sources],
        "expected_sites": [site.as_dict() for site in group.expected_sites],
        "label_source": "Juliet manifest.xml <flaw file,line>",
        "variants": variants,
        "artifact_directory": str(group_dir.resolve()),
        "runner_error": f"{type(error).__name__}: {error}",
    }


def safe_rate(numerator: int | float, denominator: int | float) -> float | None:
    return round(float(numerator) / float(denominator), 6) if denominator else None


def metric_values(tp: int, fp: int, tn: int, fn: int) -> dict[str, float | None]:
    precision = safe_rate(tp, tp + fp)
    recall = safe_rate(tp, tp + fn)
    specificity = safe_rate(tn, tn + fp)
    false_positive_rate = safe_rate(fp, fp + tn)
    balanced_accuracy = (
        round((recall + specificity) / 2.0, 6)
        if recall is not None and specificity is not None
        else None
    )
    denominator = math.sqrt((tp + fp) * (tp + fn) * (tn + fp) * (tn + fn))
    mcc = round((tp * tn - fp * fn) / denominator, 6) if denominator else None
    return {
        "precision": precision,
        "recall": recall,
        "f1": safe_rate(2 * tp, 2 * tp + fp + fn),
        "specificity": specificity,
        "false_positive_rate": false_positive_rate,
        "balanced_accuracy": balanced_accuracy,
        "mcc": mcc,
    }


def accuracy_for(groups: Sequence[dict[str, Any]], tool: str) -> dict[str, Any]:
    confusion = Counter()
    case_detection_confusion = Counter()
    alert_burden_confusion = Counter()
    statuses = Counter()
    matched_findings = 0
    unmatched_findings = 0
    wrong_rule_findings = 0
    for group in groups:
        for variant in ("bad", "good"):
            result = group["variants"][variant][tool]
            statuses[result["status"]] += 1
            classification = result["classification"]
            if classification in {"TP", "FP", "TN", "FN"}:
                confusion[classification] += 1
            else:
                confusion[f"UNASSESSED_{variant.upper()}"] += 1
            if result["status"] == "OK":
                if variant == "bad":
                    detected = "TP" if result["target_finding_count"] else "FN"
                    case_detection_confusion[detected] += 1
                    alert_burden_confusion[detected] += 1
                else:
                    case_detection_confusion[
                        "FP" if result["target_finding_count"] else "TN"
                    ] += 1
                    alert_burden_confusion[
                        "FP"
                        if result.get("memory_lifetime_finding_count", 0)
                        else "TN"
                    ] += 1
            matched_findings += len(result["matched_target_findings"])
            unmatched_findings += len(result["unmatched_target_findings"])
            wrong_rule_findings += len(result["wrong_rule_findings"])
    tp, fp, tn, fn = (
        confusion["TP"],
        confusion["FP"],
        confusion["TN"],
        confusion["FN"],
    )
    detection_tp, detection_fp, detection_tn, detection_fn = (
        case_detection_confusion["TP"],
        case_detection_confusion["FP"],
        case_detection_confusion["TN"],
        case_detection_confusion["FN"],
    )
    burden_tp, burden_fp, burden_tn, burden_fn = (
        alert_burden_confusion["TP"],
        alert_burden_confusion["FP"],
        alert_burden_confusion["TN"],
        alert_burden_confusion["FN"],
    )
    paired_complete = sum(
        group["variants"]["bad"][tool]["status"] == "OK"
        and group["variants"]["good"][tool]["status"] == "OK"
        for group in groups
    )
    return {
        "selected_testcase_groups": len(groups),
        # The primary accuracy view is target-CWE detection at testcase-group
        # granularity.  Exact manifest-line matching is localization evidence,
        # not the headline metric: CWE-401 often labels a comment saying that
        # no deallocation occurs rather than an executable warning location.
        "evaluated_confusion_matrix": {
            "TP": detection_tp,
            "FP": detection_fp,
            "TN": detection_tn,
            "FN": detection_fn,
        },
        "metrics_on_evaluated_variants": metric_values(
            detection_tp, detection_fp, detection_tn, detection_fn
        ),
        "exact_site_confusion_matrix": {"TP": tp, "FP": fp, "TN": tn, "FN": fn},
        "exact_site_metrics_on_evaluated_variants": metric_values(tp, fp, tn, fn),
        "case_detection_confusion_matrix": {
            "TP": detection_tp,
            "FP": detection_fp,
            "TN": detection_tn,
            "FN": detection_fn,
        },
        "case_detection_metrics_on_evaluated_variants": metric_values(
            detection_tp, detection_fp, detection_tn, detection_fn
        ),
        "alert_burden_confusion_matrix": {
            "TP": burden_tp,
            "FP": burden_fp,
            "TN": burden_tn,
            "FN": burden_fn,
        },
        "alert_burden_metrics_on_evaluated_variants": metric_values(
            burden_tp, burden_fp, burden_tn, burden_fn
        ),
        "positive_variants_evaluated": tp + fn,
        "negative_variants_evaluated": tn + fp,
        "unassessed_positive_variants": confusion["UNASSESSED_BAD"],
        "unassessed_negative_variants": confusion["UNASSESSED_GOOD"],
        "end_to_end_site_recall": safe_rate(tp, len(groups)),
        "end_to_end_case_detection_recall": safe_rate(detection_tp, len(groups)),
        "paired_complete_groups": paired_complete,
        "paired_complete_group_coverage": safe_rate(paired_complete, len(groups)),
        "variant_statuses": {status: statuses[status] for status in TOOL_STATUSES},
        "finding_audit": {
            "matched_target_findings": matched_findings,
            "unmatched_target_findings": unmatched_findings,
            "wrong_rule_findings_unscored": wrong_rule_findings,
        },
    }


def accuracy_report(
    groups: Sequence[dict[str, Any]], *, contract_assisted: bool = False
) -> dict[str, Any]:
    def view(subset: Sequence[dict[str, Any]]) -> dict[str, Any]:
        return {
            "plugin": accuracy_for(subset, "plugin"),
            "gcc16_fanalyzer": accuracy_for(subset, "gcc16_fanalyzer"),
        }

    languages = sorted({str(group["language"]) for group in groups})
    by_language = {
        language: view(
            [group for group in groups if group["language"] == language]
        )
        for language in languages
    }
    common_groups = [group for group in groups if commonly_evaluated(group)]
    common_by_language = {
        language: view(
            [group for group in common_groups if group["language"] == language]
        )
        for language in languages
    }

    # GCC 16.2's own manual says -fanalyzer is only suitable for C in this
    # release.  Keep C++ results as useful experimental evidence, but never let
    # the larger C++ subset decide the native-vs-plugin headline.
    selected_c = [group for group in groups if group["language"] == "c"]
    supported_c = [group for group in common_groups if group["language"] == "c"]
    headline_groups = supported_c

    by_cwe = {
        cwe: view([group for group in groups if group["cwe"] == cwe])
        for cwe in sorted({group["cwe"] for group in groups})
    }
    by_scope = {
        scope: view([group for group in groups if group["tu_scope"] == scope])
        for scope in ("single_tu", "multi_tu")
        if any(group["tu_scope"] == scope for group in groups)
    }
    return {
        "unit": "Juliet testcase-group variant",
        **benchmark_mode(contract_assisted),
        "headline_scope": (
            (
                "contract-assisted common-paired C; NOT a raw benchmark headline"
                if contract_assisted
                else "common-paired C testcase groups (GCC 16.2 documented support)"
            )
            if selected_c
            else "unavailable: selection contains no C testcase groups"
        ),
        "headline_available": bool(supported_c),
        "headline_group_count": len(headline_groups),
        "headline": view(headline_groups),
        "headline_by_cwe": {
            cwe: view([group for group in headline_groups if group["cwe"] == cwe])
            for cwe in sorted({group["cwe"] for group in headline_groups})
        },
        "headline_by_tu_scope": {
            scope: view(
                [group for group in headline_groups if group["tu_scope"] == scope]
            )
            for scope in ("single_tu", "multi_tu")
            if any(group["tu_scope"] == scope for group in headline_groups)
        },
        "matching_policy": (
            "primary: bad is TP if it emits the target CWE rule anywhere in the "
            "testcase group; good is FP if it emits that target CWE rule"
        ),
        "exact_site_localization_policy": (
            "secondary localization only: target-rule primary or trace evidence "
            "must exactly match the manifest flaw basename and line; CWE-401 "
            "manifest lines are often non-executable comments"
        ),
        "wrong_rule_policy": (
            "target-CWE precision keeps different rules unscored; the separate "
            "alert-burden matrix counts any SA001-SA003 warning on a good variant "
            "as an alert requiring triage"
        ),
        "common_pair_policy": (
            "headline includes only groups whose bad and good variants completed "
            "successfully for both tools; coverage and end-to-end recall retain all"
        ),
        "overall": view(groups),
        "overall_common_paired": view(common_groups),
        "by_language": by_language,
        "common_paired_by_language": common_by_language,
        "by_cwe": by_cwe,
        "by_tu_scope": by_scope,
    }


def coverage_for(groups: Sequence[dict[str, Any]], tool: str) -> dict[str, Any]:
    variant_statuses: Counter[str] = Counter()
    unit_statuses: Counter[str] = Counter()
    baseline_statuses: Counter[str] = Counter()
    observed_units = 0
    for group in groups:
        for variant in ("bad", "good"):
            variant_result = group["variants"][variant]
            variant_statuses[variant_result[tool]["status"]] += 1
            for unit in variant_result["translation_units"]:
                observed_units += 1
                unit_statuses[unit[tool]["status"]] += 1
                baseline_statuses[unit["baseline"]["status"]] += 1
    total_variants = 2 * len(groups)
    total_units = 2 * sum(int(group["source_count"]) for group in groups)
    missing_units = total_units - observed_units
    if missing_units > 0:
        unit_statuses["ERROR"] += missing_units
        baseline_statuses["ERROR"] += missing_units
    return {
        "group_variants": {
            "total": total_variants,
            "analyzed_ok": variant_statuses["OK"],
            "coverage_rate": safe_rate(variant_statuses["OK"], total_variants),
            "statuses": {status: variant_statuses[status] for status in TOOL_STATUSES},
        },
        "translation_unit_variants": {
            "total": total_units,
            "analyzed_ok": unit_statuses["OK"],
            "coverage_rate": safe_rate(unit_statuses["OK"], total_units),
            "statuses": {status: unit_statuses[status] for status in TOOL_STATUSES},
        },
        "shared_baseline_translation_units": {
            "total": total_units,
            "compiled_ok": baseline_statuses["OK"],
            "coverage_rate": safe_rate(baseline_statuses["OK"], total_units),
            "statuses": {status: baseline_statuses[status] for status in TOOL_STATUSES},
        },
    }


def duration_summary(values: Sequence[float]) -> dict[str, float | int | None]:
    if not values:
        return {
            "count": 0,
            "total_ms": 0.0,
            "mean_ms": None,
            "median_ms": None,
            "p95_ms": None,
            "max_ms": None,
        }
    ordered = sorted(values)
    p95_index = min(len(ordered) - 1, max(0, math.ceil(len(ordered) * 0.95) - 1))
    return {
        "count": len(values),
        "total_ms": round(sum(values), 2),
        "mean_ms": round(statistics.fmean(values), 2),
        "median_ms": round(statistics.median(values), 2),
        "p95_ms": round(ordered[p95_index], 2),
        "max_ms": round(ordered[-1], 2),
    }


def ratio_summary(values: Sequence[float]) -> dict[str, float | int | None]:
    if not values:
        return {
            "count": 0,
            "mean": None,
            "median": None,
            "p95": None,
            "min": None,
            "max": None,
        }
    ordered = sorted(values)
    p95_index = min(len(ordered) - 1, max(0, math.ceil(len(ordered) * 0.95) - 1))
    return {
        "count": len(values),
        "mean": round(statistics.fmean(values), 6),
        "median": round(statistics.median(values), 6),
        "p95": round(ordered[p95_index], 6),
        "min": round(ordered[0], 6),
        "max": round(ordered[-1], 6),
    }


def analyzer_overhead_summary(
    baseline_values: Sequence[float], analyzer_values: Sequence[float]
) -> dict[str, Any]:
    """Summarize analyzer end-to-end compile cost over a paired baseline.

    Total ratios are ratios of sums, rather than the mean of per-TU ratios, so
    tiny translation units cannot dominate the aggregate.  Negative deltas are
    retained: with one process per mode they are measurement noise, not silently
    clamped proof that analysis was free.
    """

    if len(baseline_values) != len(analyzer_values):
        raise ValueError("baseline and analyzer timing vectors must be paired")
    deltas = [
        analyzer - baseline
        for baseline, analyzer in zip(baseline_values, analyzer_values)
    ]
    ratios = [
        analyzer / baseline
        for baseline, analyzer in zip(baseline_values, analyzer_values)
        if baseline > 0
    ]
    baseline_total = sum(baseline_values)
    analyzer_total = sum(analyzer_values)
    added_total = analyzer_total - baseline_total
    return {
        "pair_count": len(deltas),
        "baseline_total_ms": round(baseline_total, 2),
        "analyzer_total_ms": round(analyzer_total, 2),
        "incremental_total_ms": round(added_total, 2),
        "summed_duration_ratio": safe_rate(analyzer_total, baseline_total),
        "incremental_percent_of_baseline": (
            round(100.0 * added_total / baseline_total, 4)
            if baseline_total
            else None
        ),
        "incremental_ms": duration_summary(deltas),
        "per_pair_duration_ratio": ratio_summary(ratios),
        "analyzer_slower_pairs": sum(delta > 0 for delta in deltas),
        "analyzer_faster_pairs": sum(delta < 0 for delta in deltas),
        "ties": sum(delta == 0 for delta in deltas),
    }


def paired_timing_for(groups: Sequence[dict[str, Any]]) -> dict[str, Any]:
    pairs: list[tuple[float, float, float]] = []
    for group in groups:
        for variant in ("bad", "good"):
            for unit in group["variants"][variant]["translation_units"]:
                if (
                    unit["baseline"]["status"] == "OK"
                    and unit["plugin"]["status"] == "OK"
                    and unit["gcc16_fanalyzer"]["status"] == "OK"
                ):
                    pairs.append(
                        (
                            float(unit["baseline"]["duration_ms"]),
                            float(unit["plugin"]["duration_ms"]),
                            float(unit["gcc16_fanalyzer"]["duration_ms"]),
                        )
                    )
    baseline = [pair[0] for pair in pairs]
    plugin = [pair[1] for pair in pairs]
    gcc = [pair[2] for pair in pairs]
    plugin_incremental = [pair[1] - pair[0] for pair in pairs]
    gcc_incremental = [pair[2] - pair[0] for pair in pairs]
    per_pair_ratios = [pair[2] / pair[1] for pair in pairs if pair[1] != 0]
    return {
        "unit": "translation-unit variant for which baseline and both analyzers were OK",
        "pair_count": len(pairs),
        "baseline_compile": duration_summary(baseline),
        "plugin_compile": duration_summary(plugin),
        "gcc16_fanalyzer_compile": duration_summary(gcc),
        "plugin_incremental_over_baseline_ms": duration_summary(plugin_incremental),
        "gcc16_fanalyzer_incremental_over_baseline_ms": duration_summary(gcc_incremental),
        "plugin_over_baseline": analyzer_overhead_summary(baseline, plugin),
        "gcc16_fanalyzer_over_baseline": analyzer_overhead_summary(baseline, gcc),
        "gcc16_over_plugin": {
            "interpretation": "greater than 1 means the plugin compile was faster",
            "summed_duration_ratio": safe_rate(sum(gcc), sum(plugin)),
            "per_pair_ratio": ratio_summary(per_pair_ratios),
            "plugin_faster_pairs": sum(plugin_value < gcc_value for _, plugin_value, gcc_value in pairs),
            "gcc16_fanalyzer_faster_pairs": sum(gcc_value < plugin_value for _, plugin_value, gcc_value in pairs),
            "ties": sum(gcc_value == plugin_value for _, plugin_value, gcc_value in pairs),
        },
    }


def commonly_evaluated(group: dict[str, Any]) -> bool:
    """Return whether both variants completed with both analyzers.

    Accuracy and the supported-language timing headline deliberately share this
    predicate.  That prevents a tool failure from changing either headline's
    population and keeps a partially completed multi-TU group out of both.
    """

    return all(
        group["variants"][variant][tool]["status"] == "OK"
        for variant in ("bad", "good")
        for tool in ("plugin", "gcc16_fanalyzer")
    )


def performance_report(
    groups: Sequence[dict[str, Any]], *, contract_assisted: bool = False
) -> dict[str, Any]:
    languages = sorted({str(group["language"]) for group in groups})
    common_groups = [group for group in groups if commonly_evaluated(group)]
    selected_c = [group for group in groups if group["language"] == "c"]
    headline_groups = [
        group for group in common_groups if group["language"] == "c"
    ]
    return {
        **benchmark_mode(contract_assisted),
        "measurement": (
            "paired elapsed wall-clock compile-only processes; baseline, plugin, "
            "and native analyzer use the same GCC 16.2 language driver and common "
            "optimization flags, and their order is deterministically rotated per "
            "translation-unit variant; parallel benchmark wall time is reported "
            "separately"
            + (
                "; plugin timings include trusted-header preprocessing, which "
                "is absent from baseline and native analyzer compiles"
                if contract_assisted else ""
            )
        ),
        "headline_scope": (
            (
                "contract-assisted common-paired C; NOT a raw benchmark headline"
                if contract_assisted
                else "common-paired C testcase groups (GCC 16.2 documented support)"
            )
            if selected_c
            else "unavailable: selection contains no C testcase groups"
        ),
        "headline_available": bool(headline_groups),
        "headline_group_count": len(headline_groups),
        "headline": paired_timing_for(headline_groups),
        "overall": paired_timing_for(groups),
        "by_language": {
            language: paired_timing_for(
                [group for group in groups if group["language"] == language]
            )
            for language in languages
        },
        "common_paired_by_language": {
            language: paired_timing_for(
                [group for group in common_groups if group["language"] == language]
            )
            for language in languages
        },
        "by_cwe": {
            cwe: paired_timing_for([group for group in groups if group["cwe"] == cwe])
            for cwe in sorted({group["cwe"] for group in groups})
        },
        "by_tu_scope": {
            scope: paired_timing_for(
                [group for group in groups if group["tu_scope"] == scope]
            )
            for scope in ("single_tu", "multi_tu")
            if any(group["tu_scope"] == scope for group in groups)
        },
    }


def preflight_plugin(
    compilers: dict[str, str], plugin: Path, output: Path, timeout: float
) -> dict[str, Any]:
    preflight_dir = output / "preflight"
    preflight_dir.mkdir(parents=True, exist_ok=True)
    checks: dict[str, Any] = {}
    for language, compiler in sorted(compilers.items()):
        suffix = ".c" if language == "c" else ".cpp"
        label = "c" if language == "c" else "cxx"
        source = preflight_dir / f"plugin_abi_probe{suffix}"
        source.write_text("int juliet_plugin_abi_probe(void) { return 0; }\n", encoding="utf-8")
        run_dir = preflight_dir / label
        run_dir.mkdir(parents=True, exist_ok=True)
        prefix = [compiler, standard_for(language), *COMMON_FLAGS]
        result = run_plugin(prefix, plugin, source, run_dir, min(timeout, 30.0))
        checks[language] = result
        if result["status"] != "OK":
            raise ConfigurationError(
                "plugin ABI/load/report preflight failed with GCC 16.2.x "
                f"{language} driver ({result['status']}): {result.get('diagnostic')}; "
                f"log: {result.get('log')}"
            )
    return checks


def fmt(value: Any, digits: int = 4) -> str:
    if value is None:
        return "n/a"
    if isinstance(value, float):
        return f"{value:.{digits}f}"
    return str(value)


def href_for(report_path: Path, value: str | None) -> str | None:
    if not value:
        return None
    target = Path(value)
    try:
        return target.relative_to(report_path.parent).as_posix()
    except ValueError:
        return target.as_uri()


def metric_row(label: str, tool_label: str, data: dict[str, Any]) -> str:
    matrix = data["evaluated_confusion_matrix"]
    metrics = data["metrics_on_evaluated_variants"]
    return (
        "<tr>"
        f"<td>{html.escape(label)}</td><td>{html.escape(tool_label)}</td>"
        f"<td>{matrix['TP']}</td><td>{matrix['FP']}</td>"
        f"<td>{matrix['TN']}</td><td>{matrix['FN']}</td>"
        f"<td>{fmt(metrics['precision'])}</td><td>{fmt(metrics['recall'])}</td>"
        f"<td>{fmt(metrics['f1'])}</td><td>{fmt(metrics['specificity'])}</td>"
        f"<td>{fmt(metrics['false_positive_rate'])}</td>"
        f"<td>{fmt(metrics['balanced_accuracy'])}</td><td>{fmt(metrics['mcc'])}</td>"
        f"<td>{fmt(data['end_to_end_site_recall'])}</td>"
        f"<td>{fmt(data['end_to_end_case_detection_recall'])}</td>"
        f"<td>{data['unassessed_positive_variants']}/"
        f"{data['unassessed_negative_variants']}</td></tr>"
    )


def write_html_report(path: Path, report: dict[str, Any]) -> None:
    esc = lambda value: html.escape(str(value), quote=True)
    mode = report.get("benchmark_mode", benchmark_mode(False))
    assisted = mode["kind"] == "contract-assisted"
    paired_label = "CONTRACT-ASSISTED: paired C" if assisted else "HEADLINE: paired C"
    plugin_label = "Plugin (contract-assisted)" if assisted else "Plugin"
    metric_rows: list[str] = []
    sections: list[tuple[str, dict[str, Any]]] = []
    if report["accuracy"]["headline_available"]:
        sections.append((paired_label, report["accuracy"]["headline"]))
        sections.extend(
            (f"paired C / CWE-{cwe}", value)
            for cwe, value in report["accuracy"]["headline_by_cwe"].items()
        )
        sections.extend(
            (f"paired C / {scope}", value)
            for scope, value in report["accuracy"]["headline_by_tu_scope"].items()
        )
    sections.extend(
        (f"language={language}" + (" (experimental)" if language != "c" else ""), value)
        for language, value in report["accuracy"]["by_language"].items()
    )
    sections.append(("all languages (exploratory)", report["accuracy"]["overall"]))
    sections.extend(
        (f"all-language CWE-{cwe} (exploratory)", value)
        for cwe, value in report["accuracy"]["by_cwe"].items()
    )
    sections.extend(
        (f"all-language {scope} (exploratory)", value)
        for scope, value in report["accuracy"]["by_tu_scope"].items()
    )
    for label, section in sections:
        metric_rows.append(metric_row(label, plugin_label, section["plugin"]))
        metric_rows.append(
            metric_row(label, "GCC 16.2 -fanalyzer", section["gcc16_fanalyzer"])
        )

    alert_rows: list[str] = []
    alert_sections: list[tuple[str, dict[str, Any]]] = []
    if report["accuracy"]["headline_available"]:
        alert_sections.append(
            (paired_label, report["accuracy"]["headline"])
        )
    alert_sections.extend(
        (f"language={language}" + (" (experimental)" if language != "c" else ""), value)
        for language, value in report["accuracy"]["by_language"].items()
    )
    for label, section in alert_sections:
        for tool, tool_label in (
            ("plugin", plugin_label),
            ("gcc16_fanalyzer", "GCC 16.2 -fanalyzer"),
        ):
            data = section[tool]
            matrix = data["alert_burden_confusion_matrix"]
            metrics = data["alert_burden_metrics_on_evaluated_variants"]
            alert_rows.append(
                "<tr>"
                f"<td>{esc(label)}</td><td>{esc(tool_label)}</td>"
                f"<td>{matrix['TP']}</td><td>{matrix['FP']}</td>"
                f"<td>{matrix['TN']}</td><td>{matrix['FN']}</td>"
                f"<td>{fmt(metrics['precision'])}</td>"
                f"<td>{fmt(metrics['recall'])}</td>"
                f"<td>{fmt(metrics['false_positive_rate'])}</td>"
                f"<td>{data['finding_audit']['wrong_rule_findings_unscored']}</td>"
                "</tr>"
            )

    localization_rows: list[str] = []
    for label, section in alert_sections:
        for tool, tool_label in (
            ("plugin", plugin_label),
            ("gcc16_fanalyzer", "GCC 16.2 -fanalyzer"),
        ):
            data = section[tool]
            matrix = data["exact_site_confusion_matrix"]
            metrics = data["exact_site_metrics_on_evaluated_variants"]
            localization_rows.append(
                "<tr>"
                f"<td>{esc(label)}</td><td>{esc(tool_label)}</td>"
                f"<td>{matrix['TP']}</td><td>{matrix['FN']}</td>"
                f"<td>{fmt(metrics['recall'])}</td>"
                f"<td>{data['finding_audit']['matched_target_findings']}</td>"
                f"<td>{data['finding_audit']['unmatched_target_findings']}</td>"
                "</tr>"
            )

    coverage_rows: list[str] = []
    for tool, label in (("plugin", plugin_label), ("gcc16_fanalyzer", "GCC 16.2 -fanalyzer")):
        coverage = report["coverage"][tool]
        group_view = coverage["group_variants"]
        tu_view = coverage["translation_unit_variants"]
        baseline_view = coverage["shared_baseline_translation_units"]
        status_text = ", ".join(
            f"{key}={value}" for key, value in group_view["statuses"].items() if value
        ) or "none"
        baseline_status_text = ", ".join(
            f"{key}={value}"
            for key, value in baseline_view["statuses"].items()
            if value
        ) or "none"
        coverage_rows.append(
            "<tr>"
            f"<td>{esc(label)}</td>"
            f"<td>{group_view['analyzed_ok']}/{group_view['total']} "
            f"({fmt(group_view['coverage_rate'])})</td>"
            f"<td>{tu_view['analyzed_ok']}/{tu_view['total']} "
            f"({fmt(tu_view['coverage_rate'])})</td>"
            f"<td><code>{esc(status_text)}</code></td>"
            f"<td>{baseline_view['compiled_ok']}/{baseline_view['total']} "
            f"(<code>{esc(baseline_status_text)}</code>)</td></tr>"
        )

    performance = report["performance"]
    timing_sections: list[tuple[str, dict[str, Any]]] = []
    if performance["headline_available"]:
        timing_sections.append((paired_label, performance["headline"]))
    timing_sections.extend(
        (
            f"language={language}"
            + (" (experimental)" if language != "c" else ""),
            timing,
        )
        for language, timing in performance["by_language"].items()
    )
    timing_sections.append(("all languages (exploratory)", performance["overall"]))
    timing_rows = []
    for scope_label, timing in timing_sections:
        for label, key in (
            ("Shared baseline", "baseline_compile"),
            (plugin_label, "plugin_compile"),
            ("GCC 16.2 -fanalyzer", "gcc16_fanalyzer_compile"),
            ("Plugin incremental", "plugin_incremental_over_baseline_ms"),
            ("GCC incremental", "gcc16_fanalyzer_incremental_over_baseline_ms"),
        ):
            item = timing[key]
            timing_rows.append(
                "<tr>"
                f"<td>{esc(scope_label)}</td><td>{esc(label)}</td>"
                f"<td>{item['count']}</td>"
                f"<td>{fmt(item['total_ms'], 2)}</td>"
                f"<td>{fmt(item['mean_ms'], 2)}</td>"
                f"<td>{fmt(item['median_ms'], 2)}</td>"
                f"<td>{fmt(item['p95_ms'], 2)}</td>"
                f"<td>{fmt(item['max_ms'], 2)}</td></tr>"
            )

    overhead_rows: list[str] = []
    for scope_label, timing in timing_sections:
        for label, key in (
            (plugin_label, "plugin_over_baseline"),
            ("GCC 16.2 -fanalyzer", "gcc16_fanalyzer_over_baseline"),
        ):
            overhead = timing[key]
            overhead_rows.append(
                "<tr>"
                f"<td>{esc(scope_label)}</td><td>{esc(label)}</td>"
                f"<td>{overhead['pair_count']}</td>"
                f"<td>{fmt(overhead['baseline_total_ms'], 2)}</td>"
                f"<td>{fmt(overhead['analyzer_total_ms'], 2)}</td>"
                f"<td>{fmt(overhead['incremental_total_ms'], 2)}</td>"
                f"<td>{fmt(overhead['summed_duration_ratio'])}</td>"
                f"<td>{fmt(overhead['incremental_percent_of_baseline'], 2)}%</td>"
                f"<td>{overhead['analyzer_slower_pairs']}/"
                f"{overhead['analyzer_faster_pairs']}/{overhead['ties']}</td>"
                "</tr>"
            )

    group_rows: list[str] = []
    for group in report["groups"]:
        bad = group["variants"]["bad"]
        good = group["variants"]["good"]
        site_text = ", ".join(
            f"{site_value['file']}:{site_value['line']}" for site_value in group["expected_sites"]
        )
        links: list[str] = []
        for variant_name, variant in (("bad", bad), ("good", good)):
            for unit_index, unit in enumerate(variant["translation_units"], 1):
                for tool, label in (("plugin", "plugin"), ("gcc16_fanalyzer", "gcc")):
                    if not unit[tool].get("findings"):
                        continue
                    target = href_for(path, unit[tool].get("log"))
                    if target:
                        links.append(
                            f"<a href='{esc(target)}'>{variant_name}-{label}-tu{unit_index}</a>"
                        )
        group_rows.append(
            "<tr>"
            f"<td>{esc(group['cwe'])}</td><td>{esc(group['language'])}</td>"
            f"<td>{esc(group['tu_scope'])}</td>"
            f"<td><code>{esc(group['group_id'])}</code></td>"
            f"<td><code>{esc(site_text)}</code></td>"
            f"<td class='{esc(bad['plugin']['case_detection_classification'].lower())}'>"
            f"{esc(bad['plugin']['case_detection_classification'])}</td>"
            f"<td class='{esc(good['plugin']['case_detection_classification'].lower())}'>"
            f"{esc(good['plugin']['case_detection_classification'])}</td>"
            f"<td class='{esc(bad['gcc16_fanalyzer']['case_detection_classification'].lower())}'>"
            f"{esc(bad['gcc16_fanalyzer']['case_detection_classification'])}</td>"
            f"<td class='{esc(good['gcc16_fanalyzer']['case_detection_classification'].lower())}'>"
            f"{esc(good['gcc16_fanalyzer']['case_detection_classification'])}</td>"
            f"<td>{good['plugin']['memory_lifetime_finding_count']}</td>"
            f"<td>{good['gcc16_fanalyzer']['memory_lifetime_finding_count']}</td>"
            f"<td>{len(bad['plugin']['matched_target_findings'])}/"
            f"{len(bad['plugin']['unmatched_target_findings'])}</td>"
            f"<td>{len(bad['gcc16_fanalyzer']['matched_target_findings'])}/"
            f"{len(bad['gcc16_fanalyzer']['unmatched_target_findings'])}</td>"
            f"<td>{' · '.join(links) or 'n/a'}</td></tr>"
        )

    inventory = report["oracle"]["selected"]
    fair_timing = performance["headline"]
    ratio_value = (
        fair_timing["gcc16_over_plugin"]["summed_duration_ratio"]
        if performance["headline_available"]
        else None
    )
    timing_pair_count = (
        fair_timing["pair_count"] if performance["headline_available"] else 0
    )
    plugin_baseline_ratio = (
        fair_timing["plugin_over_baseline"]["summed_duration_ratio"]
        if performance["headline_available"]
        else None
    )
    gcc_baseline_ratio = (
        fair_timing["gcc16_fanalyzer_over_baseline"]["summed_duration_ratio"]
        if performance["headline_available"]
        else None
    )
    selected_languages = set(inventory.get("by_language", {}))
    if assisted:
        headline_notice = (
            "<strong>Contract-assisted comparison — NOT a raw benchmark headline.</strong> "
            "The plugin receives an explicitly supplied trusted contract header; "
            "the baseline and native GCC analyzer receive no such header. "
            "These results measure the plugin with additional semantic input. "
            "Plugin timings include the header's preprocessing cost."
        )
    elif report["accuracy"]["headline_available"]:
        headline_notice = (
            "<strong>Fair headline:</strong> common-paired C testcase groups. "
            "GCC 16.2 documents <code>-fanalyzer</code> as suitable only for C "
            "in this release."
        )
    elif "c" in selected_languages:
        headline_notice = (
            "<strong>No fair headline:</strong> the selection contains C cases, "
            "but no C testcase group completed both variants with both tools."
        )
    else:
        headline_notice = (
            "<strong>No fair native-vs-plugin headline:</strong> this selection "
            "contains only C++ cases. GCC 16.2 documents <code>-fanalyzer</code> "
            "as suitable only for C; the C++ results below are experimental."
        )
    compiler_lines = "<br>".join(
        f"{esc(language)}: {esc(identity['banner'])} [{esc(identity['executable'])}]"
        for language, identity in sorted(report["toolchains"]["drivers"].items())
    )
    summary = report.get("summary", {})
    configuration = report.get("configuration", {})
    order_counts = summary.get("timed_execution_order_counts", {})
    order_text = ", ".join(
        f"{order}={count}" for order, count in order_counts.items()
    ) or "not recorded"
    manifest_artifact = report.get("manifest_artifact", {})
    runner_artifact = report.get("runner_artifact", {})
    source_artifact = report.get("source_selection_artifact", {})
    verification = report.get("input_provenance_verification", {})
    extra_artifact_lines = ""
    for key, label in (
        ("plugin_contract_header_artifact", "Plugin-only contract header"),
        ("group_id_selection_artifact", "Exact group-id selection"),
    ):
        artifact = report.get(key)
        if artifact:
            extra_artifact_lines += (
                f"<br>{label}: <code>{esc(artifact['path'])}</code>"
                f"<br>{label} SHA-256: <code>{esc(artifact['sha256'])}</code>"
            )
    timing_scope_label = "Contract-assisted paired-C" if assisted else "Fair paired-C"
    ratio_scope_label = "Contract-assisted C" if assisted else "Fair C"
    extra_timing_notice = (
        " The plugin alone also includes the supplied trusted contract header; "
        "its preprocessing cost is included in plugin overhead."
        if assisted else ""
    )
    document = f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>{esc(mode['label'])} — Juliet manifest-oracle analyzer comparison</title>
<style>
body{{font:14px system-ui,sans-serif;background:#f5f7fa;color:#172033;margin:0}}main{{max-width:1600px;margin:32px auto;padding:0 20px}}h1,h2{{margin-top:28px}}.meta,.notice,.card{{background:#fff;border:1px solid #dce2ea;border-radius:9px;padding:14px}}.notice{{border-left:5px solid #b7791f;margin:14px 0}}.cards{{display:flex;gap:12px;flex-wrap:wrap;margin:18px 0}}.card{{min-width:170px}}.number{{font-size:24px;font-weight:700}}table{{width:100%;border-collapse:collapse;background:#fff;border:1px solid #dce2ea}}th,td{{padding:8px;text-align:left;border-bottom:1px solid #e5e9ef;vertical-align:top}}thead th{{position:sticky;top:0;background:#eef2f7}}code{{font-size:12px;overflow-wrap:anywhere}}.tp,.tn{{color:#177245;font-weight:700}}.fp,.fn,.unassessed{{color:#b42336;font-weight:700}}a{{color:#185abc}}
</style></head><body><main>
<h1>Juliet manifest-oracle analyzer comparison</h1>
<div class="notice">{esc(mode['label'])}{extra_artifact_lines}</div>
<div class="meta">Generated: {esc(report['generated_at'])}<br>{compiler_lines}<br>Jobs: {esc(configuration.get('jobs', 'not recorded'))}<br>Parallel-run wall duration: {esc(summary.get('parallel_run_wall_duration_ms', 'not recorded'))} ms<br>Timed-order histogram: <code>{esc(order_text)}</code><br>Plugin SHA-256: <code>{esc(report['plugin_artifact']['sha256'])}</code><br>Runner SHA-256: <code>{esc(runner_artifact.get('sha256', 'not recorded'))}</code><br>Manifest: <code>{esc(report['oracle']['inventory']['manifest'])}</code><br>Manifest SHA-256: <code>{esc(manifest_artifact.get('sha256', 'not recorded'))}</code><br>Selected-source-set SHA-256: <code>{esc(source_artifact.get('sha256', 'not recorded'))}</code><br>Input drift check before/after run: {esc(verification.get('verified_before_run', 'not recorded'))}/{esc(verification.get('verified_after_run', 'not recorded'))}</div>
<div class="cards"><div class="card"><div class="number">{inventory['groups']}</div>Testcase groups</div><div class="card"><div class="number">{inventory['translation_units']}</div>Source TUs</div><div class="card"><div class="number">{timing_pair_count}</div>{timing_scope_label} timing units</div><div class="card"><div class="number">{fmt(plugin_baseline_ratio, 2)}×</div>Plugin / baseline wall time</div><div class="card"><div class="number">{fmt(gcc_baseline_ratio, 2)}×</div>GCC analyzer / baseline wall time</div><div class="card"><div class="number">{fmt(ratio_value, 2)}</div>{ratio_scope_label} GCC/plugin summed ratio</div></div>
<div class="notice">{headline_notice} C++ is shown separately as experimental and cannot decide the native-vs-plugin winner. Primary accuracy credits a target-CWE warning anywhere in the bad testcase group. Exact manifest-line matching is secondary localization evidence because CWE-401 flaw lines are frequently comments. Metrics exclude unassessed variants; coverage and end-to-end recall retain them.</div>
<h2>Target-CWE case accuracy</h2><table><thead><tr><th>Slice</th><th>Tool</th><th>TP</th><th>FP</th><th>TN</th><th>FN</th><th>Precision</th><th>Recall</th><th>F1</th><th>Specificity</th><th>FPR</th><th>Balanced accuracy</th><th>MCC</th><th>End-to-end site recall</th><th>End-to-end case detection</th><th>Unassessed +/−</th></tr></thead><tbody>{''.join(metric_rows)}</tbody></table>
<h2>Any-lifetime-warning alert burden</h2><p>Positive credit still requires the testcase's target CWE. On a good variant, however, any mapped SA001–SA003 warning counts as an alert requiring triage. It may expose an incidental bug, so this is a strict alert-burden view rather than a claim that every cross-rule alert is semantically false.</p><table><thead><tr><th>Slice</th><th>Tool</th><th>TP</th><th>Good with alert</th><th>Good without alert</th><th>FN</th><th>Precision</th><th>Recall</th><th>Alert rate on good</th><th>Cross-rule findings</th></tr></thead><tbody>{''.join(alert_rows)}</tbody></table>
<h2>Exact-site localization (secondary)</h2><table><thead><tr><th>Slice</th><th>Tool</th><th>Site TP</th><th>Site FN</th><th>Site recall</th><th>Matched findings</th><th>Unmatched findings</th></tr></thead><tbody>{''.join(localization_rows)}</tbody></table>
<h2>Coverage and failures</h2><table><thead><tr><th>Tool</th><th>Group variants OK</th><th>TU variants OK</th><th>Group statuses</th><th>Shared baseline TUs</th></tr></thead><tbody>{''.join(coverage_rows)}</tbody></table>
<h2>Incremental analyzer overhead over plain GCC</h2><p>Each analyzer is compared with the plain compile of the same TU variant using the same GCC 16.2 driver, language standard, macro, include paths, optimization flags, and object-generation mode.{extra_timing_notice} Ratios are ratios of summed wall time; this avoids overweighting tiny TUs. A negative one-shot delta is retained as measurement noise, not clamped to zero.</p><table><thead><tr><th>Slice</th><th>Analyzer mode</th><th>Pairs</th><th>Baseline total ms</th><th>Analyzer total ms</th><th>Added ms</th><th>Analyzer / baseline</th><th>Added vs baseline</th><th>Slower/faster/tied pairs</th></tr></thead><tbody>{''.join(overhead_rows)}</tbody></table>
<h2>Paired timing distributions</h2><p>Milliseconds; one pair is one TU variant whose baseline and both analyzer processes completed. The speed headline uses only common-paired C groups; all-language and C++ timing are exploratory. All three measured modes rotate deterministically across six orders to reduce systematic warm-cache and process-order bias. These are end-to-end compiler-process wall times, including diagnostics and report serialization; CPU time is not inferred from a threaded runner.</p><table><thead><tr><th>Slice</th><th>Measurement</th><th>Count</th><th>Total</th><th>Mean</th><th>Median</th><th>P95</th><th>Max</th></tr></thead><tbody>{''.join(timing_rows)}</tbody></table>
<h2>Testcase groups</h2><table><thead><tr><th>CWE</th><th>Language</th><th>Scope</th><th>Group</th><th>Manifest flaw site</th><th>Plugin bad</th><th>Plugin good</th><th>GCC bad</th><th>GCC good</th><th>Plugin good lifetime alerts</th><th>GCC good lifetime alerts</th><th>Plugin matched/unmatched</th><th>GCC matched/unmatched</th><th>Finding logs</th></tr></thead><tbody>{''.join(group_rows)}</tbody></table>
</main></body></html>"""
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(document, encoding="utf-8")
    temporary.replace(path)


def selected_inventory(groups: Sequence[OracleGroup]) -> dict[str, Any]:
    languages = {group.group_id: group_language(group) for group in groups}
    return {
        "groups": len(groups),
        "translation_units": sum(len(group.sources) for group in groups),
        "flaw_sites": sum(len(group.expected_sites) for group in groups),
        "single_tu_groups": sum(group.tu_scope == "single_tu" for group in groups),
        "multi_tu_groups": sum(group.tu_scope == "multi_tu" for group in groups),
        "by_language": {
            language: {
                "groups": sum(languages[group.group_id] == language for group in groups),
                "translation_units": sum(
                    len(group.sources)
                    for group in groups
                    if languages[group.group_id] == language
                ),
            }
            for language in ("c", "c++")
            if any(languages[group.group_id] == language for group in groups)
        },
        "by_cwe": {
            cwe: {
                "groups": sum(group.cwe == cwe for group in groups),
                "translation_units": sum(
                    len(group.sources) for group in groups if group.cwe == cwe
                ),
            }
            for cwe in sorted({group.cwe for group in groups})
        },
    }


def print_summary(report: dict[str, Any], verbose: bool) -> None:
    mode = report.get("benchmark_mode", benchmark_mode(False))
    assisted = mode["kind"] == "contract-assisted"
    selected = report["oracle"]["selected"]
    print(
        f"\nCompared {selected['groups']} Juliet testcase groups "
        f"({selected['translation_units']} translation units, bad + good variants)"
    )
    print(mode["label"])
    scope_label = "Contract-assisted comparison" if assisted else "Fair headline"
    print(
        f"{scope_label}: {report['accuracy']['headline_scope']} "
        f"(n={report['accuracy']['headline_group_count']})"
    )
    order_counts = report["summary"].get("timed_execution_order_counts", {})
    print(
        "Timed-order histogram: "
        + (
            ", ".join(f"{order}={count}" for order, count in order_counts.items())
            or "n/a"
        )
    )
    print(f"Runner SHA-256: {report['runner_artifact']['sha256']}")
    print(f"Manifest SHA-256: {report['manifest_artifact']['sha256']}")
    print(
        "Selected-source-set SHA-256: "
        f"{report['source_selection_artifact']['sha256']}"
    )
    for key, label in (
        ("plugin_contract_header_artifact", "Plugin-only contract header"),
        ("group_id_selection_artifact", "Exact group-id selection"),
    ):
        artifact = report.get(key)
        if artifact:
            print(f"{label}: {artifact['path']}")
            print(f"{label} SHA-256: {artifact['sha256']}")
    print("Input drift check: verified unchanged before and after benchmark")
    if report["accuracy"]["headline_available"]:
        for tool, label in (
            ("plugin", "Plugin (contract-assisted)" if assisted else "Plugin"),
            ("gcc16_fanalyzer", "GCC 16.2 -fanalyzer"),
        ):
            accuracy = report["accuracy"]["headline"][tool]
            matrix = accuracy["evaluated_confusion_matrix"]
            metrics = accuracy["metrics_on_evaluated_variants"]
            burden = accuracy["alert_burden_confusion_matrix"]
            burden_metrics = accuracy["alert_burden_metrics_on_evaluated_variants"]
            print(
                f"{label}: TP={matrix['TP']} FP={matrix['FP']} TN={matrix['TN']} "
                f"FN={matrix['FN']}; precision={fmt(metrics['precision'])}, "
                f"recall={fmt(metrics['recall'])}, F1={fmt(metrics['f1'])}; "
                f"target-FPR={fmt(metrics['false_positive_rate'])}; "
                f"any-lifetime-good-alerts={burden['FP']} "
                f"({fmt(burden_metrics['false_positive_rate'])})"
            )
    else:
        print("No common-paired C comparison is available for this selection.")
    if "c++" in report["accuracy"]["by_language"]:
        print("C++ results are experimental: GCC 16.2 documents -fanalyzer as C-only.")
    performance = report["performance"]
    if performance["headline_available"]:
        timing = performance["headline"]
        plugin_overhead = timing["plugin_over_baseline"]
        gcc_overhead = timing["gcc16_fanalyzer_over_baseline"]
        print(
            f"{'Contract-assisted' if assisted else 'Fair'} paired-C timing units: "
            f"{timing['pair_count']}; "
            f"GCC/plugin summed ratio: "
            f"{fmt(timing['gcc16_over_plugin']['summed_duration_ratio'])}"
        )
        print(
            "Incremental wall time over plain GCC: "
            f"plugin={fmt(plugin_overhead['summed_duration_ratio'])}x "
            f"({fmt(plugin_overhead['incremental_percent_of_baseline'], 2)}%), "
            f"-fanalyzer={fmt(gcc_overhead['summed_duration_ratio'])}x "
            f"({fmt(gcc_overhead['incremental_percent_of_baseline'], 2)}%)"
        )
        if assisted:
            print("Plugin timings include the plugin-only contract header's preprocessing cost.")
    else:
        print("No fair C timing headline is available for this selection.")
    overall_timing = performance["overall"]
    if overall_timing["pair_count"] != performance["headline"]["pair_count"]:
        print(
            f"Exploratory all-language timing units: {overall_timing['pair_count']}; "
            f"GCC/plugin summed ratio: "
            f"{fmt(overall_timing['gcc16_over_plugin']['summed_duration_ratio'])}"
        )
    if verbose:
        for group in report["groups"]:
            bad = group["variants"]["bad"]
            good = group["variants"]["good"]
            print(
                f"{group['group_id']} [{group['tu_scope']}] "
                f"plugin={bad['plugin']['case_detection_classification']}/"
                f"{good['plugin']['case_detection_classification']} "
                f"gcc={bad['gcc16_fanalyzer']['case_detection_classification']}/"
                f"{good['gcc16_fanalyzer']['case_detection_classification']}"
            )


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    if args.jobs < 1:
        print("error: --jobs must be at least 1", file=sys.stderr)
        return 2
    if args.timeout <= 0:
        print("error: --timeout must be positive", file=sys.stderr)
        return 2
    if args.limit is not None and args.limit < 1:
        print("error: --limit must be at least 1", file=sys.stderr)
        return 2

    try:
        selected_cwes = args.cwe or list(CWE_CONFIGURATION)
        runner_path = Path(__file__).resolve()
        runner_sha256 = sha256_file(runner_path)
        manifest_path = args.manifest.expanduser().resolve()
        if not manifest_path.is_file():
            raise ConfigurationError(f"Juliet manifest not found: {manifest_path}")
        manifest_sha256 = sha256_file(manifest_path)
        contract_artifact = (
            contract_header_artifact(args.plugin_contract_header)
            if args.plugin_contract_header is not None else None
        )
        if contract_artifact is not None:
            args.plugin_contract_header = Path(contract_artifact["path"])
        exact_group_ids = None
        group_selection_artifact = None
        if args.group_id_file is not None:
            exact_group_ids, group_selection_artifact = group_id_selection_artifact(
                args.group_id_file
            )
            args.group_id_file = Path(group_selection_artifact["path"])
        extra_provenance = {
            "plugin_contract_header": args.plugin_contract_header,
            "initial_plugin_contract_header_sha256": (
                contract_artifact["sha256"] if contract_artifact else None
            ),
            "group_id_file": args.group_id_file,
            "initial_group_id_file_sha256": (
                group_selection_artifact["sha256"] if group_selection_artifact else None
            ),
        }
        all_groups, inventory = discover_oracle(manifest_path, selected_cwes)
        groups = sorted(
            select_groups(
                all_groups, args.language, args.match, args.limit, exact_group_ids
            ),
            key=lambda group: group.group_id,
        )
        rotation_starts = group_rotation_start_indices(groups)
        scheduled_tu_variants = 2 * sum(len(group.sources) for group in groups)
        order_counts = timed_order_histogram(scheduled_tu_variants)
        corpus_root = manifest_path.parent
        selected_sources = source_selection_artifact(groups, corpus_root)
        compilers, identities = validate_toolchains(args, groups)
        args.output = args.output.expanduser().resolve()
        args.output.mkdir(parents=True, exist_ok=True)
        plugin, plugin_sha256, original_plugin = snapshot_plugin(args.plugin, args.output)
        preflight = preflight_plugin(compilers, plugin, args.output, args.timeout)
        verify_input_provenance_unchanged(
            groups,
            corpus_root,
            selected_sources,
            manifest_path,
            manifest_sha256,
            runner_path,
            runner_sha256,
            **extra_provenance,
        )
    except (ConfigurationError, OSError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    started = time.monotonic()
    results: list[dict[str, Any]] = []
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {
            executor.submit(
                run_group,
                args,
                group,
                compilers,
                plugin,
                rotation_starts[group.group_id],
            ): group
            for group in groups
        }
        for future in as_completed(futures):
            group = futures[future]
            try:
                results.append(future.result())
            except ConfigurationError as error:
                for pending in futures:
                    pending.cancel()
                print(f"error: {error}", file=sys.stderr)
                return 2
            except Exception as error:
                results.append(failed_group(group, args.output, error))
    results.sort(key=lambda result: result["group_id"])
    wall_duration_ms = round((time.monotonic() - started) * 1000, 2)
    try:
        verified_hashes = verify_input_provenance_unchanged(
            groups,
            corpus_root,
            selected_sources,
            manifest_path,
            manifest_sha256,
            runner_path,
            runner_sha256,
            **extra_provenance,
        )
    except (ConfigurationError, OSError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    report = {
        "schema_version": 1,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "repository": str(REPOSITORY_ROOT),
        "benchmark_mode": benchmark_mode(contract_artifact is not None),
        "configuration": {
            **benchmark_mode(contract_artifact is not None),
            "selected_cwes": selected_cwes,
            "language": args.language or "all",
            "match": args.match,
            "limit": args.limit,
            "group_id_file": str(args.group_id_file) if args.group_id_file else None,
            "plugin_contract_header": (
                str(args.plugin_contract_header) if args.plugin_contract_header else None
            ),
            "plugin_only_extra_flags": (
                ["-include", str(args.plugin_contract_header)]
                if args.plugin_contract_header else []
            ),
            "jobs": args.jobs,
            "timeout_seconds_per_process": args.timeout,
            "variant_macros": {"bad": "OMITGOOD", "good": "OMITBAD"},
            "common_flags": list(COMMON_FLAGS),
            "baseline_mode": (
                "plain GCC compile: no -fplugin and no -fanalyzer"
            ),
            "plugin_options": list(PLUGIN_OPTIONS),
            "gcc16_fanalyzer_flags": list(GCC_ANALYZER_FLAGS),
            "timed_mode_order_policy": (
                "selected groups are sorted by group id, then every TU variant is "
                "assigned a global sequential index round-robin across all six "
                "permutations of baseline, plugin, and gcc16_fanalyzer; assignment "
                "does not use absolute paths and counts differ by at most one"
            ),
        },
        "oracle": {
            "policy": (
                "Juliet manifest testcase groups and exact <flaw file,line> sites; "
                "generated main files are excluded"
            ),
            "inventory": inventory,
            "selected": selected_inventory(groups),
        },
        "manifest_artifact": {
            "path": str(manifest_path),
            "sha256": manifest_sha256,
        },
        "runner_artifact": {
            "path": str(runner_path),
            "sha256": runner_sha256,
        },
        "source_selection_artifact": selected_sources,
        "plugin_contract_header_artifact": contract_artifact,
        "group_id_selection_artifact": group_selection_artifact,
        "input_provenance_verification": {
            **verified_hashes,
            "verified_before_run": True,
            "verified_after_run": True,
        },
        "toolchains": {
            "policy": (
                "plugin and -fanalyzer use the same strict GCC 16.2.x language "
                "driver for each source; plugin load/report is preflighted"
            ),
            "drivers": identities,
            "plugin_abi_preflight": preflight,
        },
        "plugin_artifact": {
            "original": str(original_plugin),
            "snapshot": str(plugin),
            "sha256": plugin_sha256,
        },
        "diagnostic_mapping": {
            "plugin_rules": {
                configuration["target_rule"]: configuration["label"]
                for configuration in CWE_CONFIGURATION.values()
            },
            "gcc_rule_to_plugin_rule": GCC_RULE_TO_PLUGIN_RULE,
        },
        "summary": {
            "parallel_run_wall_duration_ms": wall_duration_ms,
            "groups": len(results),
            "translation_units": sum(group["source_count"] for group in results),
            "timed_execution_order_counts": order_counts,
        },
        "coverage": {
            "plugin": coverage_for(results, "plugin"),
            "gcc16_fanalyzer": coverage_for(results, "gcc16_fanalyzer"),
        },
        "accuracy": accuracy_report(results, contract_assisted=contract_artifact is not None),
        "performance": performance_report(results, contract_assisted=contract_artifact is not None),
        "groups": results,
    }

    json_path = args.output / "comparison.json"
    temporary_json = json_path.with_suffix(".json.tmp")
    temporary_json.write_text(
        json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    temporary_json.replace(json_path)
    html_path = args.output / "comparison.html"
    write_html_report(html_path, report)
    print_summary(report, args.verbose)
    print(f"HTML: {html_path}")
    print(f"JSON: {json_path}")

    execution_failed = any(
        report["coverage"][tool]["group_variants"]["statuses"][status]
        for tool in ("plugin", "gcc16_fanalyzer")
        for status in ("BUILD_FAIL", "ERROR", "CRASH", "TIMEOUT", "NOT_RUN")
    )
    return 1 if execution_failed else 0


if __name__ == "__main__":
    sys.exit(main())
