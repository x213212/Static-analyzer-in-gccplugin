#!/usr/bin/env python3
"""Merge per-translation-unit static-analyzer reports.

The GCC plugin writes one JSON report for each translation unit.  This tool
turns those bounded reports into a deterministic project JSON document and a
self-contained HTML report.  It deliberately keeps findings and inconclusive
candidates separate: an inconclusive candidate is not a compiler warning.
"""

from __future__ import annotations

import argparse
from collections import Counter
from dataclasses import asdict, dataclass
import html
import json
import os
from pathlib import Path
import sys
import tempfile
from typing import Any, Iterable, Iterator, Mapping, Sequence


PROJECT_SCHEMA_VERSION = 1
PROJECT_TOOL_NAME = "gcc-static-analyzer-project-report"
PROJECT_TOOL_VERSION = "1.0"
DEFAULT_JSON_NAME = "static-analyzer-project-report.json"
KNOWN_RULES = ("SA001", "SA002", "SA003", "SA004")


class MergeError(RuntimeError):
    """An error that should be shown as a concise CLI diagnostic."""


@dataclass(frozen=True)
class MergeLimits:
    """Hard bounds for untrusted or unexpectedly large report trees."""

    max_json_files: int = 20_000
    max_reports: int = 10_000
    max_file_bytes: int = 16 * 1024 * 1024
    max_findings: int = 100_000
    max_inconclusive_samples: int = 100_000
    max_provenance_per_record: int = 32
    max_record_bytes: int = 64 * 1024 * 1024
    max_counter_keys: int = 1_024
    max_trace_points: int = 256
    max_error_samples: int = 256
    max_string_chars: int = 8_192

    def validate(self) -> None:
        for name, value in asdict(self).items():
            if value <= 0:
                raise MergeError(f"{name} must be greater than zero")


def _bounded_text(value: Any, limit: int, default: str = "") -> str:
    if isinstance(value, str):
        result = value
    elif value is None:
        result = default
    elif isinstance(value, (int, float, bool)):
        result = str(value)
    else:
        result = default
    if len(result) <= limit:
        return result
    return result[: max(0, limit - 1)] + "…"


def _nonnegative_int(value: Any, default: int = 0) -> int:
    if isinstance(value, bool):
        return default
    if isinstance(value, int):
        return max(0, min(value, 2**63 - 1))
    return default


def _normalise_location(raw: Any, limits: MergeLimits) -> dict[str, Any]:
    location = raw if isinstance(raw, Mapping) else {}
    return {
        "file": _bounded_text(location.get("file"), limits.max_string_chars),
        "line": _nonnegative_int(location.get("line")),
        "column": _nonnegative_int(location.get("column")),
    }


def _normalise_trace(raw: Any, limits: MergeLimits) -> list[dict[str, Any]]:
    if not isinstance(raw, list):
        return []
    result: list[dict[str, Any]] = []
    for point in raw[: limits.max_trace_points]:
        if not isinstance(point, Mapping):
            continue
        result.append(
            {
                "role": _bounded_text(
                    point.get("role"), limits.max_string_chars, "evidence"
                ),
                "file": _bounded_text(
                    point.get("file"), limits.max_string_chars
                ),
                "line": _nonnegative_int(point.get("line")),
                "column": _nonnegative_int(point.get("column")),
            }
        )
    return result


def _normalise_finding(raw: Any, limits: MergeLimits) -> dict[str, Any] | None:
    if not isinstance(raw, Mapping):
        return None
    rule_id = _bounded_text(raw.get("rule_id"), limits.max_string_chars)
    if not rule_id:
        return None
    raw_remediation = raw.get("remediation")
    remediation = raw_remediation if isinstance(raw_remediation, Mapping) else {}
    # Do not coerce strings or numbers to bool: a malformed value such as
    # "false" must never be rendered as an available automatic patch.
    automatic_patch = remediation.get("automatic_patch")
    if not isinstance(automatic_patch, bool):
        automatic_patch = False
    return {
        "rule_id": rule_id,
        "status": (
            _bounded_text(raw.get("status"), limits.max_string_chars)
            if isinstance(raw.get("status"), str)
            else "UNSPECIFIED"
        ),
        "confidence": (
            _bounded_text(raw.get("confidence"), limits.max_string_chars)
            if isinstance(raw.get("confidence"), str)
            else "unknown"
        ),
        **({
            "model_scope": _bounded_text(raw.get("model_scope"), limits.max_string_chars),
            "path_verdict": _bounded_text(raw.get("path_verdict"), limits.max_string_chars),
        } if raw.get("status") == "MODEL_REVIEW" else {}),
        "severity": _bounded_text(
            raw.get("severity"), limits.max_string_chars, "warning"
        ),
        "message": _bounded_text(raw.get("message"), limits.max_string_chars),
        "remediation": {
            "kind": _bounded_text(
                remediation.get("kind"),
                limits.max_string_chars,
                "manual-guidance",
            ),
            "automatic_patch": automatic_patch,
            "summary": _bounded_text(
                remediation.get("summary"), limits.max_string_chars
            ),
        },
        "location": _normalise_location(raw.get("location"), limits),
        "trace": _normalise_trace(raw.get("trace"), limits),
    }


def _normalise_inconclusive(
    raw: Any, limits: MergeLimits
) -> dict[str, Any] | None:
    if not isinstance(raw, Mapping):
        return None
    rule_id = _bounded_text(raw.get("rule_id"), limits.max_string_chars)
    if not rule_id:
        return None
    related = raw.get("related", [])
    related = related[:8] if isinstance(related, list) else []
    return {
        "rule_id": rule_id,
        "status": "INCONCLUSIVE",
        "reason": _bounded_text(raw.get("reason"), limits.max_string_chars),
        "detail": _bounded_text(raw.get("detail"), limits.max_string_chars),
        "location": _normalise_location(raw.get("location"), limits),
        "related": _normalise_trace(related, limits),
    }


def _canonical_key(record: Mapping[str, Any]) -> str:
    return json.dumps(
        record, ensure_ascii=False, sort_keys=True, separators=(",", ":")
    )


def _sorted_count_map(counter: Mapping[str, int]) -> dict[str, int]:
    return {key: counter[key] for key in sorted(counter)}


def _report_kind(document: Any) -> str:
    """Return ``per-tu``, ``project``, or ``unrelated``."""

    if not isinstance(document, Mapping):
        return "unrelated"
    tool = document.get("tool")
    tool_name = tool.get("name") if isinstance(tool, Mapping) else None
    if tool_name == PROJECT_TOOL_NAME:
        return "project"
    if tool_name == "gcc-static-analyzer-plugin":
        return "per-tu"
    # Older plugin reports did not consistently identify the tool.  Requiring
    # all three fields avoids consuming benchmark manifests and comparison
    # reports that happen to contain a list called "findings".
    if (
        isinstance(document.get("source"), str)
        and isinstance(document.get("summary"), Mapping)
        and "findings" in document
    ):
        return "per-tu"
    return "unrelated"


def _iter_json_files(input_path: Path) -> Iterator[Path]:
    if input_path.is_file():
        yield input_path
        return
    for directory, dirnames, filenames in os.walk(input_path, followlinks=False):
        dirnames.sort()
        for filename in sorted(filenames):
            if filename.lower().endswith(".json"):
                yield Path(directory) / filename


def _display_report_path(path: Path, input_path: Path) -> str:
    if input_path.is_file():
        return path.name
    try:
        return path.relative_to(input_path).as_posix()
    except ValueError:
        return path.as_posix()


def _resolved(path: Path) -> Path:
    return path.expanduser().resolve(strict=False)


def _read_json(path: Path, limits: MergeLimits) -> Any:
    try:
        size = path.stat().st_size
    except OSError as error:
        raise MergeError(f"cannot stat JSON input: {error}") from error
    if size > limits.max_file_bytes:
        raise MergeError(
            f"JSON input is {size} bytes; limit is {limits.max_file_bytes}"
        )
    try:
        with path.open("r", encoding="utf-8") as source:
            return json.load(source)
    except (OSError, UnicodeError, json.JSONDecodeError) as error:
        raise MergeError(f"cannot parse JSON: {error}") from error


def _count_mapping(raw: Any) -> Counter[str]:
    counts: Counter[str] = Counter()
    if not isinstance(raw, Mapping):
        return counts
    for key, value in raw.items():
        if not isinstance(key, str):
            continue
        count = _nonnegative_int(value)
        if count:
            counts[key] += count
    return counts


def _schema_version(document: Mapping[str, Any]) -> int:
    version = _nonnegative_int(document.get("schema_version"), 1)
    return version or 1


def _provenance(
    report_path: str, document: Mapping[str, Any], limits: MergeLimits
) -> dict[str, Any]:
    source = _bounded_text(document.get("source"), limits.max_string_chars)
    result: dict[str, Any] = {
        "report": report_path,
        "source": source,
        "schema_version": _schema_version(document),
    }
    tool = document.get("tool")
    if isinstance(tool, Mapping):
        version = _bounded_text(tool.get("version"), limits.max_string_chars)
        if version:
            result["tool_version"] = version
    analysis = document.get("analysis")
    if isinstance(analysis, Mapping):
        policy = _bounded_text(analysis.get("policy"), limits.max_string_chars)
        if policy:
            result["analysis_policy"] = policy
    return result


class _Accumulator:
    def __init__(self, root: Path, limits: MergeLimits) -> None:
        self.root = root
        self.limits = limits
        self.json_files_scanned = 0
        self.reports_processed = 0
        self.unrelated_json_files = 0
        self.prior_project_reports = 0
        self.malformed_json_files = 0
        self.invalid_records = 0
        self.error_count = 0
        self.errors: list[dict[str, str]] = []
        self.truncation_reasons: set[str] = set()
        self.reports: list[dict[str, Any]] = []
        self.findings: dict[str, dict[str, Any]] = {}
        self.inconclusive: dict[str, dict[str, Any]] = {}
        self.finding_occurrences = 0
        self.finding_counts: Counter[str] = Counter()
        self.finding_records_dropped = 0
        self.inconclusive_total = 0
        self.inconclusive_samples_seen = 0
        self.inconclusive_counts: Counter[str] = Counter()
        self.inconclusive_reasons: Counter[str] = Counter()
        self.source_samples_omitted = 0
        self.inconclusive_records_dropped = 0
        self.stored_record_bytes = 0

    def error(self, report: str, message: str, *, malformed_file: bool = False) -> None:
        self.error_count += 1
        if malformed_file:
            self.malformed_json_files += 1
        if len(self.errors) < self.limits.max_error_samples:
            self.errors.append({"report": report, "error": message})

    def truncated(self, reason: str) -> None:
        self.truncation_reasons.add(reason)

    def _count(self, counter: Counter[str], key: str, amount: int = 1) -> None:
        if amount <= 0:
            return
        # Reserve one bucket so adversarial future rule/reason strings cannot
        # turn the summary Counters into an unbounded side channel.
        if key not in counter and len(counter) >= self.limits.max_counter_keys - 1:
            key = "<other>"
            self.truncated("max_counter_keys")
        counter[key] += amount

    def _add_record(
        self,
        destination: dict[str, dict[str, Any]],
        record: dict[str, Any],
        provenance: dict[str, Any],
        maximum: int,
        kind: str,
    ) -> None:
        key = _canonical_key(record)
        existing = destination.get(key)
        provenance_cost = len(_canonical_key(provenance).encode("utf-8")) + 64
        if existing is None:
            if len(destination) >= maximum:
                if kind == "finding":
                    self.finding_records_dropped += 1
                else:
                    self.inconclusive_records_dropped += 1
                self.truncated(f"max_{kind}_records")
                return
            # The canonical key and the normalised record are both retained.
            # This approximate byte budget is intentionally conservative.
            record_cost = len(key.encode("utf-8")) * 2 + 128
            if (
                self.stored_record_bytes + record_cost + provenance_cost
                > self.limits.max_record_bytes
            ):
                if kind == "finding":
                    self.finding_records_dropped += 1
                else:
                    self.inconclusive_records_dropped += 1
                self.truncated("max_record_bytes")
                return
            existing = dict(record)
            existing["occurrence_count"] = 0
            existing["provenance"] = []
            existing["provenance_omitted"] = 0
            destination[key] = existing
            self.stored_record_bytes += record_cost
        existing["occurrence_count"] += 1
        provenance_items: list[dict[str, Any]] = existing["provenance"]
        if provenance in provenance_items:
            return
        if len(provenance_items) < self.limits.max_provenance_per_record:
            if (
                self.stored_record_bytes + provenance_cost
                <= self.limits.max_record_bytes
            ):
                provenance_items.append(provenance)
                self.stored_record_bytes += provenance_cost
            else:
                existing["provenance_omitted"] += 1
                self.truncated("max_record_bytes")
        else:
            existing["provenance_omitted"] += 1
            self.truncated("max_provenance_per_record")

    def add_report(self, report_path: str, document: Mapping[str, Any]) -> None:
        raw_findings = document.get("findings")
        if not isinstance(raw_findings, list):
            self.error(report_path, "per-TU report has no findings array")
            return
        raw_inconclusive = document.get("inconclusive", [])
        if raw_inconclusive is None:
            raw_inconclusive = []
        if not isinstance(raw_inconclusive, list):
            self.error(report_path, "per-TU report has an invalid inconclusive array")
            return

        provenance = _provenance(report_path, document, self.limits)
        report_finding_count = 0
        report_sample_count = 0

        for index, raw in enumerate(raw_findings):
            finding = _normalise_finding(raw, self.limits)
            if finding is None:
                self.invalid_records += 1
                self.error(report_path, f"invalid finding at index {index}")
                continue
            report_finding_count += 1
            self.finding_occurrences += 1
            self._count(self.finding_counts, finding["rule_id"])
            self._add_record(
                self.findings,
                finding,
                provenance,
                self.limits.max_findings,
                "finding",
            )

        normalised_samples: list[dict[str, Any]] = []
        for index, raw in enumerate(raw_inconclusive):
            sample = _normalise_inconclusive(raw, self.limits)
            if sample is None:
                self.invalid_records += 1
                self.error(report_path, f"invalid inconclusive sample at index {index}")
                continue
            report_sample_count += 1
            self.inconclusive_samples_seen += 1
            normalised_samples.append(sample)
            self._add_record(
                self.inconclusive,
                sample,
                provenance,
                self.limits.max_inconclusive_samples,
                "inconclusive",
            )

        raw_inconclusive_summary = document.get("inconclusive_summary")
        inconclusive_summary = (
            raw_inconclusive_summary
            if isinstance(raw_inconclusive_summary, Mapping)
            else {}
        )
        summary = document.get("summary")
        summary = summary if isinstance(summary, Mapping) else {}

        report_inconclusive_total = _nonnegative_int(
            inconclusive_summary.get("total"),
            _nonnegative_int(summary.get("inconclusive_total"), report_sample_count),
        )
        report_by_rule = _count_mapping(inconclusive_summary.get("by_rule"))
        report_by_reason = _count_mapping(inconclusive_summary.get("by_reason"))
        if not report_by_rule:
            report_by_rule.update(sample["rule_id"] for sample in normalised_samples)
        if not report_by_reason:
            report_by_reason.update(
                sample["reason"] for sample in normalised_samples if sample["reason"]
            )
        if report_inconclusive_total == 0 and normalised_samples:
            report_inconclusive_total = len(normalised_samples)

        self.inconclusive_total += report_inconclusive_total
        for rule, count in report_by_rule.items():
            self._count(self.inconclusive_counts, rule, count)
        for reason, count in report_by_reason.items():
            self._count(self.inconclusive_reasons, reason, count)
        self.source_samples_omitted += _nonnegative_int(
            inconclusive_summary.get("samples_omitted")
        )

        tool = document.get("tool")
        tool_version = ""
        if isinstance(tool, Mapping):
            tool_version = _bounded_text(
                tool.get("version"), self.limits.max_string_chars
            )
        analysis = document.get("analysis")
        policy = ""
        if isinstance(analysis, Mapping):
            policy = _bounded_text(
                analysis.get("policy"), self.limits.max_string_chars
            )
        self.reports.append(
            {
                "report": report_path,
                "source": provenance["source"],
                "schema_version": provenance["schema_version"],
                "tool_version": tool_version,
                "analysis_policy": policy,
                "finding_count": report_finding_count,
                "inconclusive_total": report_inconclusive_total,
                "inconclusive_samples": report_sample_count,
            }
        )
        self.reports_processed += 1

    @staticmethod
    def _record_sort_key(record: Mapping[str, Any]) -> tuple[Any, ...]:
        location = record.get("location")
        location = location if isinstance(location, Mapping) else {}
        return (
            _bounded_text(location.get("file"), 1_000_000),
            _nonnegative_int(location.get("line")),
            _nonnegative_int(location.get("column")),
            _bounded_text(record.get("rule_id"), 1_000_000),
            _bounded_text(record.get("status"), 1_000_000),
            _bounded_text(record.get("reason"), 1_000_000),
            _bounded_text(record.get("message"), 1_000_000),
            _canonical_key(record),
        )

    def _sorted_records(
        self, records: Mapping[str, dict[str, Any]]
    ) -> list[dict[str, Any]]:
        result = list(records.values())
        for record in result:
            record["provenance"].sort(
                key=lambda item: (
                    item.get("report", ""),
                    item.get("source", ""),
                    item.get("schema_version", 0),
                    item.get("tool_version", ""),
                    item.get("analysis_policy", ""),
                )
            )
        result.sort(key=self._record_sort_key)
        return result

    def document(self) -> dict[str, Any]:
        reports = sorted(
            self.reports,
            key=lambda item: (
                item["report"],
                item["source"],
                item["schema_version"],
            ),
        )
        errors = sorted(self.errors, key=lambda item: (item["report"], item["error"]))
        findings = self._sorted_records(self.findings)
        inconclusive = self._sorted_records(self.inconclusive)
        all_rules = sorted(
            set(KNOWN_RULES)
            | set(self.finding_counts)
            | set(self.inconclusive_counts)
        )
        finding_by_rule = {
            rule: self.finding_counts.get(rule, 0) for rule in all_rules
        }
        inconclusive_by_rule = {
            rule: self.inconclusive_counts.get(rule, 0) for rule in all_rules
        }
        truncation_reasons = sorted(self.truncation_reasons)
        return {
            "schema_version": PROJECT_SCHEMA_VERSION,
            "tool": {
                "name": PROJECT_TOOL_NAME,
                "version": PROJECT_TOOL_VERSION,
            },
            "input": {
                "root": self.root.as_posix(),
                "json_files_scanned": self.json_files_scanned,
                "reports_processed": self.reports_processed,
                "unrelated_json_files": self.unrelated_json_files,
                "prior_project_reports_ignored": self.prior_project_reports,
                "malformed_json_files": self.malformed_json_files,
                "invalid_records": self.invalid_records,
                "error_count": self.error_count,
                "error_samples_omitted": max(0, self.error_count - len(errors)),
                "truncated": bool(truncation_reasons),
                "truncation_reasons": truncation_reasons,
            },
            "limits": asdict(self.limits),
            "summary": {
                "finding_occurrences": self.finding_occurrences,
                "unique_findings": len(findings),
                "finding_records_dropped": self.finding_records_dropped,
                "findings_by_rule": finding_by_rule,
                "inconclusive_total": self.inconclusive_total,
                "inconclusive_samples_seen": self.inconclusive_samples_seen,
                "unique_inconclusive_samples": len(inconclusive),
                "inconclusive_records_dropped": self.inconclusive_records_dropped,
                "stored_record_bytes_approximate": self.stored_record_bytes,
            },
            "reports": reports,
            "findings": findings,
            "inconclusive_summary": {
                "total": self.inconclusive_total,
                "samples_seen": self.inconclusive_samples_seen,
                "unique_samples": len(inconclusive),
                "source_samples_omitted": self.source_samples_omitted,
                "merge_records_dropped": self.inconclusive_records_dropped,
                "by_rule": inconclusive_by_rule,
                "by_reason": _sorted_count_map(self.inconclusive_reasons),
            },
            "inconclusive": inconclusive,
            "errors": errors,
        }


def merge_reports(
    input_path: Path | str,
    *,
    excluded_paths: Iterable[Path | str] = (),
    limits: MergeLimits | None = None,
) -> dict[str, Any]:
    """Read and merge reports without writing any output files."""

    root = Path(input_path).expanduser()
    if not root.exists():
        raise MergeError(f"input path does not exist: {root}")
    if not root.is_dir() and not root.is_file():
        raise MergeError(f"input path is neither a file nor directory: {root}")
    root = root.resolve()
    limits = limits or MergeLimits()
    limits.validate()
    excluded = {_resolved(Path(path)) for path in excluded_paths}
    accumulator = _Accumulator(root, limits)

    for path in _iter_json_files(root):
        if _resolved(path) in excluded:
            continue
        if accumulator.json_files_scanned >= limits.max_json_files:
            accumulator.truncated("max_json_files")
            break
        accumulator.json_files_scanned += 1
        report_path = _display_report_path(path, root)
        try:
            document = _read_json(path, limits)
        except MergeError as error:
            accumulator.error(report_path, str(error), malformed_file=True)
            continue
        kind = _report_kind(document)
        if kind == "project":
            accumulator.prior_project_reports += 1
            continue
        if kind != "per-tu":
            accumulator.unrelated_json_files += 1
            continue
        if accumulator.reports_processed >= limits.max_reports:
            accumulator.truncated("max_reports")
            break
        accumulator.add_report(report_path, document)

    return accumulator.document()


def _escape(value: Any) -> str:
    return html.escape(str(value), quote=True)


def _location_text(record: Mapping[str, Any]) -> str:
    location = record.get("location")
    if not isinstance(location, Mapping):
        return "?:0:0"
    return (
        f"{location.get('file', '')}:{location.get('line', 0)}:"
        f"{location.get('column', 0)}"
    )


def _provenance_html(record: Mapping[str, Any]) -> str:
    provenance = record.get("provenance")
    if not isinstance(provenance, list) or not provenance:
        return "<span class=\"muted\">unknown report</span>"
    items = []
    for item in provenance:
        if not isinstance(item, Mapping):
            continue
        report = _escape(item.get("report", ""))
        source = _escape(item.get("source", ""))
        items.append(f"<li><code>{report}</code><br><span>{source}</span></li>")
    omitted = _nonnegative_int(record.get("provenance_omitted"))
    if omitted:
        items.append(f"<li class=\"muted\">{omitted} more provenance item(s)</li>")
    return f"<ul class=\"provenance\">{''.join(items)}</ul>"


def render_html(document: Mapping[str, Any]) -> str:
    """Render a self-contained, escaped project report."""

    summary = document.get("summary")
    summary = summary if isinstance(summary, Mapping) else {}
    input_info = document.get("input")
    input_info = input_info if isinstance(input_info, Mapping) else {}
    findings = document.get("findings")
    findings = findings if isinstance(findings, list) else []
    inconclusive = document.get("inconclusive")
    inconclusive = inconclusive if isinstance(inconclusive, list) else []
    errors = document.get("errors")
    errors = errors if isinstance(errors, list) else []

    finding_rows: list[str] = []
    for finding in findings:
        if not isinstance(finding, Mapping):
            continue
        remediation = finding.get("remediation")
        remediation = remediation if isinstance(remediation, Mapping) else {}
        automatic_patch = remediation.get("automatic_patch") is True
        remediation_summary = _escape(remediation.get("summary", ""))
        remediation_kind = _escape(remediation.get("kind", "manual-guidance"))
        remediation_html = (
            '<div class="remediation"><strong>Remediation:</strong> '
            + remediation_summary
            + '<div class="muted">kind='
            + remediation_kind
            + " · automatic_patch="
            + ("true" if automatic_patch else "false")
            + "</div></div>"
        )
        finding_state_html = (
            '<div class="finding-state">status='
            + _escape(finding.get("status", "UNSPECIFIED"))
            + " · confidence="
            + _escape(finding.get("confidence", "unknown"))
            + "</div>"
        )
        if finding.get("status") == "MODEL_REVIEW":
            finding_state_html += (
                '<div class="finding-state">library model review: '
                + _escape(finding.get("model_scope", "unknown profile"))
                + " · path=" + _escape(finding.get("path_verdict", "unknown"))
                + "; not an unconditional body/path proof</div>"
            )
        trace_items = []
        trace = finding.get("trace")
        if isinstance(trace, list):
            for point in trace:
                if not isinstance(point, Mapping):
                    continue
                trace_items.append(
                    "<li>"
                    + _escape(point.get("role", "evidence"))
                    + ": <code>"
                    + _escape(
                        f"{point.get('file', '')}:{point.get('line', 0)}:"
                        f"{point.get('column', 0)}"
                    )
                    + "</code></li>"
                )
        trace_html = (
            f"<ol class=\"trace\">{''.join(trace_items)}</ol>"
            if trace_items
            else ""
        )
        finding_rows.append(
            "<tr><td class=\"rule\">"
            + _escape(finding.get("rule_id", ""))
            + "</td><td>"
            + _escape(finding.get("message", ""))
            + finding_state_html
            + remediation_html
            + trace_html
            + "</td><td><code>"
            + _escape(_location_text(finding))
            + "</code></td><td>"
            + _escape(finding.get("occurrence_count", 1))
            + "</td><td>"
            + _provenance_html(finding)
            + "</td></tr>"
        )
    if not finding_rows:
        finding_rows.append(
            '<tr><td class="ok" colspan="5">No findings.</td></tr>'
        )

    inconclusive_rows: list[str] = []
    for sample in inconclusive:
        if not isinstance(sample, Mapping):
            continue
        detail = _escape(sample.get("detail", ""))
        if detail:
            detail = f'<div class="muted detail">{detail}</div>'
        related = sample.get("related", [])
        if isinstance(related, list) and related:
            detail += '<ul class="trace">' + ''.join(
                '<li>' + _escape(point.get("role", "evidence")) + ': <code>'
                + _escape(_location_text({"location": point})) + '</code></li>'
                for point in related[:8] if isinstance(point, Mapping)
            ) + '</ul>'
        if sample.get("rule_id") == "SA001" and sample.get("reason") in (
            "ambiguous-callee-ownership", "opaque-ownership-transfer"
        ):
            detail += (
                '<details class="muted detail"><summary>Ownership assumptions to check</summary>'
                '<p>Ownership preserved without release or transfer: the outstanding object may leak. '
                'Released or transferred: a local leak is not established; later accesses need a '
                'separate lifetime check.</p><p>These are model alternatives, not verified executions. '
                'This record remains INCONCLUSIVE.</p></details>'
            )
        inconclusive_rows.append(
            "<tr><td class=\"rule\">"
            + _escape(sample.get("rule_id", ""))
            + "</td><td class=\"inconclusive\">INCONCLUSIVE · "
            + _escape(sample.get("reason", ""))
            + detail
            + "</td><td><code>"
            + _escape(_location_text(sample))
            + "</code></td><td>"
            + _escape(sample.get("occurrence_count", 1))
            + "</td><td>"
            + _provenance_html(sample)
            + "</td></tr>"
        )
    if not inconclusive_rows:
        inconclusive_rows.append(
            '<tr><td class="ok" colspan="5">No inconclusive samples.</td></tr>'
        )

    report_rows: list[str] = []
    reports = document.get("reports")
    reports = reports if isinstance(reports, list) else []
    for report in reports:
        if not isinstance(report, Mapping):
            continue
        report_rows.append(
            "<tr><td><code>"
            + _escape(report.get("report", ""))
            + "</code></td><td>"
            + _escape(report.get("source", ""))
            + "</td><td>"
            + _escape(report.get("finding_count", 0))
            + "</td><td>"
            + _escape(report.get("inconclusive_total", 0))
            + "</td></tr>"
        )
    if not report_rows:
        report_rows.append(
            '<tr><td class="ok" colspan="4">No per-TU reports found.</td></tr>'
        )

    error_section = ""
    if errors:
        error_items = []
        for error in errors:
            if not isinstance(error, Mapping):
                continue
            error_items.append(
                "<li><code>"
                + _escape(error.get("report", ""))
                + "</code>: "
                + _escape(error.get("error", ""))
                + "</li>"
            )
        error_section = (
            '<section><h2>Input diagnostics</h2><p class="note">Malformed '
            "or invalid records were skipped.</p><ul>"
            + "".join(error_items)
            + "</ul></section>"
        )

    truncated_note = ""
    if input_info.get("truncated"):
        reasons = input_info.get("truncation_reasons", [])
        if not isinstance(reasons, list):
            reasons = []
        truncated_note = (
            '<div class="banner">Aggregation reached a configured limit: '
            + _escape(", ".join(str(item) for item in reasons))
            + ". Counts and samples may be incomplete.</div>"
        )

    return f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Static Analyzer Project Report</title>
<style>
:root{{--bg:#f5f7fa;--card:#fff;--line:#dce2ea;--ink:#172033;--muted:#65718a;--warn:#a51d2d;--inc:#875c00}}
*{{box-sizing:border-box}}body{{font:15px system-ui,sans-serif;margin:0;background:var(--bg);color:var(--ink)}}
main{{width:100%;max-width:960px;margin:32px auto;padding:0 20px;overflow-wrap:anywhere}}h1{{margin-bottom:6px}}h2{{margin-top:34px}}code{{white-space:normal;overflow-wrap:anywhere}}
.source,.muted,.note{{color:var(--muted)}}.cards{{display:grid;grid-template-columns:repeat(auto-fit,minmax(130px,1fr));gap:12px;margin:26px 0}}
.card,table{{background:var(--card);border:1px solid var(--line);border-radius:10px;box-shadow:0 2px 8px #18243a0d}}
.card{{padding:16px}}.count{{font-size:27px;font-weight:700}}.label{{color:var(--muted)}}
.banner{{padding:13px 16px;background:#fff4d8;border:1px solid #e7bd59;border-radius:8px;margin:18px 0}}
.table-wrap{{width:100%}}table{{width:100%;table-layout:fixed;border-collapse:separate;border-spacing:0;overflow:hidden}}th,td{{padding:12px;text-align:left;border-bottom:1px solid #e7ebf0;vertical-align:top;overflow-wrap:anywhere}}
th{{background:#eef2f7}}.rule{{font-weight:700;color:var(--warn)}}.inconclusive{{color:var(--inc)}}.ok{{padding:26px;color:#177245;font-weight:650}}
.trace,.provenance{{margin:8px 0 0;padding-left:20px;font-size:13px}}.detail{{margin-top:6px}}.finding-state{{margin-top:6px;color:#177245;font-size:13px}}.remediation{{margin-top:8px;padding:8px 10px;background:#f6f8fb;border-radius:6px}}footer{{margin:24px 0;color:#778196;font-size:13px}}
@media(max-width:640px){{main{{margin:20px auto;padding:0 12px}}.cards{{grid-template-columns:repeat(2,minmax(0,1fr))}}table{{font-size:13px}}thead{{display:none}}tbody,tr,td{{display:block;width:100%}}td{{padding:10px 12px}}tr+tr{{border-top:2px solid var(--line)}}}}
</style></head><body><main>
<h1>Static Analyzer Project Report</h1><div class="source">Input: {_escape(input_info.get('root', ''))}</div>
{truncated_note}<div class="cards">
<div class="card"><div class="count">{_escape(input_info.get('reports_processed', 0))}</div><div class="label">Translation units</div></div>
<div class="card"><div class="count">{_escape(summary.get('finding_occurrences', 0))}</div><div class="label">Finding occurrences</div></div>
<div class="card"><div class="count">{_escape(summary.get('unique_findings', 0))}</div><div class="label">Unique findings</div></div>
<div class="card"><div class="count">{_escape(summary.get('inconclusive_total', 0))}</div><div class="label">Inconclusive candidates</div></div>
<div class="card"><div class="count">{_escape(input_info.get('error_count', 0))}</div><div class="label">Input diagnostics</div></div>
</div>
<section><h2>Findings</h2><p class="note">Duplicate records are retained as one row with occurrence and report provenance counts.</p><div class="table-wrap"><table><thead><tr><th>Rule</th><th>Problem / trace</th><th>Location</th><th>Occurrences</th><th>Source reports</th></tr></thead><tbody>{''.join(finding_rows)}</tbody></table></div></section>
<section><h2>Inconclusive candidates</h2><p class="note">These records are not findings and did not emit compiler warnings. Per-TU sampling and project aggregation are bounded.</p><div class="table-wrap"><table><thead><tr><th>Rule</th><th>Status / reason</th><th>Location</th><th>Occurrences</th><th>Source reports</th></tr></thead><tbody>{''.join(inconclusive_rows)}</tbody></table></div></section>
<section><h2>Translation-unit reports</h2><div class="table-wrap"><table><thead><tr><th>Report</th><th>Source</th><th>Findings</th><th>Inconclusive</th></tr></thead><tbody>{''.join(report_rows)}</tbody></table></div></section>
{error_section}<footer>{PROJECT_TOOL_NAME} {PROJECT_TOOL_VERSION} · deterministic bounded aggregation</footer>
</main></body></html>
"""


def _atomic_write(path: Path, contents: str) -> None:
    path = path.expanduser()
    try:
        path.parent.mkdir(parents=True, exist_ok=True)
        descriptor, temporary_name = tempfile.mkstemp(
            prefix=f".{path.name}.", suffix=".tmp", dir=path.parent
        )
        temporary_path = Path(temporary_name)
        try:
            with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as output:
                output.write(contents)
                output.flush()
                os.fsync(output.fileno())
                os.fchmod(output.fileno(), 0o644)
            os.replace(temporary_path, path)
        except BaseException:
            try:
                temporary_path.unlink()
            except FileNotFoundError:
                pass
            raise
    except OSError as error:
        raise MergeError(f"cannot write output '{path}': {error}") from error


def write_reports(
    document: Mapping[str, Any], json_path: Path | str, html_path: Path | str
) -> None:
    json_output = Path(json_path)
    html_output = Path(html_path)
    if _resolved(json_output) == _resolved(html_output):
        raise MergeError("JSON and HTML output paths must be different")
    json_text = json.dumps(
        document, ensure_ascii=False, indent=2, sort_keys=True
    ) + "\n"
    _atomic_write(json_output, json_text)
    _atomic_write(html_output, render_html(document))


def _positive_integer(value: str) -> int:
    try:
        result = int(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError("must be an integer") from error
    if result <= 0:
        raise argparse.ArgumentTypeError("must be greater than zero")
    return result


def _parser() -> argparse.ArgumentParser:
    defaults = MergeLimits()
    parser = argparse.ArgumentParser(
        description=(
            "Recursively merge per-translation-unit GCC static-analyzer plugin "
            "JSON reports into deterministic project JSON and HTML reports."
        )
    )
    parser.add_argument("input", type=Path, help="JSON report or directory to scan")
    parser.add_argument("--json", dest="json_output", type=Path, help="project JSON output")
    parser.add_argument("--html", dest="html_output", type=Path, help="project HTML output")
    parser.add_argument(
        "--strict",
        action="store_true",
        help="return status 2 when malformed input or truncation was recorded",
    )
    parser.add_argument("--quiet", action="store_true", help="suppress success summary")
    parser.add_argument(
        "--max-json-files",
        type=_positive_integer,
        default=defaults.max_json_files,
    )
    parser.add_argument(
        "--max-reports", type=_positive_integer, default=defaults.max_reports
    )
    parser.add_argument(
        "--max-file-bytes", type=_positive_integer, default=defaults.max_file_bytes
    )
    parser.add_argument(
        "--max-findings", type=_positive_integer, default=defaults.max_findings
    )
    parser.add_argument(
        "--max-inconclusive-samples",
        type=_positive_integer,
        default=defaults.max_inconclusive_samples,
    )
    parser.add_argument(
        "--max-provenance-per-record",
        type=_positive_integer,
        default=defaults.max_provenance_per_record,
    )
    parser.add_argument(
        "--max-record-bytes",
        type=_positive_integer,
        default=defaults.max_record_bytes,
        help="approximate aggregate byte budget for retained records",
    )
    parser.add_argument(
        "--max-counter-keys",
        type=_positive_integer,
        default=defaults.max_counter_keys,
        help="maximum distinct rule/reason summary keys",
    )
    return parser


def _output_paths(arguments: argparse.Namespace) -> tuple[Path, Path]:
    input_path: Path = arguments.input.expanduser()
    output_directory = input_path if input_path.is_dir() else input_path.parent
    default_json = output_directory / DEFAULT_JSON_NAME
    if arguments.json_output is not None:
        json_output = arguments.json_output
    elif arguments.html_output is not None:
        json_output = arguments.html_output.with_suffix(".json")
    else:
        json_output = default_json
    if arguments.html_output is not None:
        html_output = arguments.html_output
    elif arguments.json_output is not None:
        html_output = arguments.json_output.with_suffix(".html")
    else:
        html_output = default_json.with_suffix(".html")
    return json_output, html_output


def main(argv: Sequence[str] | None = None) -> int:
    parser = _parser()
    arguments = parser.parse_args(argv)
    json_output, html_output = _output_paths(arguments)
    limits = MergeLimits(
        max_json_files=arguments.max_json_files,
        max_reports=arguments.max_reports,
        max_file_bytes=arguments.max_file_bytes,
        max_findings=arguments.max_findings,
        max_inconclusive_samples=arguments.max_inconclusive_samples,
        max_provenance_per_record=arguments.max_provenance_per_record,
        max_record_bytes=arguments.max_record_bytes,
        max_counter_keys=arguments.max_counter_keys,
    )
    try:
        document = merge_reports(
            arguments.input,
            excluded_paths=(json_output, html_output),
            limits=limits,
        )
        write_reports(document, json_output, html_output)
    except MergeError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    input_info = document["input"]
    if not arguments.quiet:
        print(
            f"merged {input_info['reports_processed']} report(s): "
            f"{document['summary']['finding_occurrences']} finding occurrence(s), "
            f"{document['summary']['inconclusive_total']} inconclusive candidate(s)"
        )
        print(f"JSON: {json_output}")
        print(f"HTML: {html_output}")
    if input_info["error_count"]:
        print(
            f"warning: skipped {input_info['error_count']} malformed or invalid "
            "input item(s); see the project report",
            file=sys.stderr,
        )
    if arguments.strict and (input_info["error_count"] or input_info["truncated"]):
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
