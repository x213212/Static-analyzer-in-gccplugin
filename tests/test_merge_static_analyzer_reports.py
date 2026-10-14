from __future__ import annotations

from contextlib import redirect_stderr, redirect_stdout
import io
import json
from pathlib import Path
import tempfile
import unittest

from tools.merge_static_analyzer_reports import (
    MergeLimits,
    main,
    merge_reports,
    render_html,
    write_reports,
)


def finding(
    *,
    rule: str = "SA001",
    source: str = "/src/example.c",
    line: int = 7,
    message: str = "allocation is not released",
) -> dict[str, object]:
    return {
        "rule_id": rule,
        "severity": "warning",
        "message": message,
        "location": {"file": source, "line": line, "column": 3},
        "trace": [
            {
                "role": "allocation",
                "file": source,
                "line": max(1, line - 2),
                "column": 9,
            }
        ],
    }


def report(
    *,
    source: str = "/src/example.c",
    findings: list[dict[str, object]] | None = None,
    inconclusive: list[dict[str, object]] | None = None,
    inconclusive_total: int | None = None,
) -> dict[str, object]:
    findings = [] if findings is None else findings
    inconclusive = [] if inconclusive is None else inconclusive
    total = len(inconclusive) if inconclusive_total is None else inconclusive_total
    by_rule: dict[str, int] = {}
    by_reason: dict[str, int] = {}
    for item in inconclusive:
        rule = str(item["rule_id"])
        reason = str(item["reason"])
        by_rule[rule] = by_rule.get(rule, 0) + 1
        by_reason[reason] = by_reason.get(reason, 0) + 1
    if total > len(inconclusive) and by_rule:
        first_rule = sorted(by_rule)[0]
        first_reason = sorted(by_reason)[0]
        by_rule[first_rule] += total - len(inconclusive)
        by_reason[first_reason] += total - len(inconclusive)
    return {
        "schema_version": 2,
        "tool": {"name": "gcc-static-analyzer-plugin", "version": "2.0"},
        "analysis": {"policy": "proof-only"},
        "source": source,
        "summary": {
            "total": len(findings),
            "inconclusive_total": total,
        },
        "findings": findings,
        "inconclusive_summary": {
            "total": total,
            "samples_recorded": len(inconclusive),
            "samples_omitted": max(0, total - len(inconclusive)),
            "by_rule": by_rule,
            "by_reason": by_reason,
        },
        "inconclusive": inconclusive,
    }


def write_json(path: Path, document: object) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(document), encoding="utf-8")


class MergeStaticAnalyzerReportsTest(unittest.TestCase):
    def test_ownership_barrier_locations_are_bounded_and_remain_inconclusive(self) -> None:
        candidate = {
            "rule_id": "SA001", "reason": "ambiguous-callee-ownership",
            "location": {"file": "/src/example.c", "line": 7, "column": 3},
            "related": [{"role": "call <script>", "file": "/src/example.c",
                         "line": line, "column": 1} for line in range(20, 40)],
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(root / "analysis.json", report(inconclusive=[candidate]))
            document = merge_reports(root)
        self.assertEqual(document["findings"], [])
        self.assertEqual(len(document["inconclusive"][0]["related"]), 8)
        self.assertEqual(document["inconclusive"][0]["related"][0]["line"], 20)
        rendered = render_html(document)
        self.assertIn("Ownership assumptions to check", rendered)
        self.assertIn("call &lt;script&gt;", rendered)
        self.assertNotIn("call <script>", rendered)

    def test_same_named_reports_keep_provenance_and_dedupe_stably(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            duplicate = finding()
            write_json(root / "object-a" / "analysis.json", report(findings=[duplicate]))
            write_json(root / "object-b" / "analysis.json", report(findings=[duplicate]))

            document = merge_reports(root)

        self.assertEqual(document["input"]["reports_processed"], 2)
        self.assertEqual(document["summary"]["finding_occurrences"], 2)
        self.assertEqual(document["summary"]["unique_findings"], 1)
        merged = document["findings"][0]
        self.assertEqual(merged["occurrence_count"], 2)
        self.assertEqual(
            [item["report"] for item in merged["provenance"]],
            ["object-a/analysis.json", "object-b/analysis.json"],
        )

    def test_different_records_do_not_collide_at_same_location(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            first = finding(rule="SA001", message="first interpretation")
            second = finding(rule="SA002", message="second interpretation")
            write_json(root / "one.json", report(findings=[first, second]))

            document = merge_reports(root)

        self.assertEqual(document["summary"]["unique_findings"], 2)
        self.assertEqual(
            [item["rule_id"] for item in document["findings"]],
            ["SA001", "SA002"],
        )

    def test_proof_and_remediation_metadata_are_preserved_and_rendered(self) -> None:
        attack = "release before return <script>repair()</script>&"
        enriched = finding()
        enriched.update(
            {
                "status": "PROVEN",
                "confidence": "high",
                "remediation": {
                    "kind": "manual-guidance",
                    "automatic_patch": False,
                    "summary": attack,
                },
            }
        )
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(root / "proof.json", report(findings=[enriched]))
            document = merge_reports(root)
            rendered = render_html(document)

        merged = document["findings"][0]
        self.assertEqual(merged["status"], "PROVEN")
        self.assertEqual(merged["confidence"], "high")
        self.assertEqual(
            merged["remediation"],
            {
                "kind": "manual-guidance",
                "automatic_patch": False,
                "summary": attack,
            },
        )
        self.assertIn("status=PROVEN · confidence=high", rendered)
        self.assertIn("automatic_patch=false", rendered)
        self.assertIn("release before return &lt;script&gt;repair()", rendered)
        self.assertNotIn("<script>repair()", rendered)

    def test_library_review_does_not_lose_unknown_path_or_profile(self) -> None:
        candidate = finding()
        candidate.update(status="MODEL_REVIEW", confidence="conditional",
                         model_scope="ffmpeg-sdk-v1 <script>", path_verdict="unknown")
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            write_json(root / "review.json", report(findings=[candidate]))
            document = merge_reports(root)
            rendered = render_html(document)
        merged = document["findings"][0]
        self.assertEqual(merged["status"], "MODEL_REVIEW")
        self.assertEqual(merged["path_verdict"], "unknown")
        self.assertEqual(merged["model_scope"], "ffmpeg-sdk-v1 <script>")
        self.assertIn("not an unconditional body/path proof", rendered)
        self.assertIn("ffmpeg-sdk-v1 &lt;script&gt;", rendered)
        self.assertNotIn("ffmpeg-sdk-v1 <script>", rendered)
        from tools.project_report_html import analysis_evidence
        presented = analysis_evidence({"findings": [candidate]})["findings"][0]
        self.assertEqual(presented["status"], "MODEL_REVIEW")
        self.assertEqual(presented["path_verdict"], "unknown")
        self.assertEqual(presented["model_scope"], candidate["model_scope"])

    def test_remediation_is_part_of_canonical_dedupe_key(self) -> None:
        first = finding()
        second = finding()
        first["remediation"] = {
            "kind": "manual-guidance",
            "automatic_patch": False,
            "summary": "release on the error path",
        }
        second["remediation"] = {
            "kind": "manual-guidance",
            "automatic_patch": False,
            "summary": "transfer ownership to the caller",
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(root / "one.json", report(findings=[first, second]))
            document = merge_reports(root)

        self.assertEqual(document["summary"]["finding_occurrences"], 2)
        self.assertEqual(document["summary"]["unique_findings"], 2)

    def test_malformed_remediation_cannot_enable_patch_and_is_bounded(self) -> None:
        unsafe = finding()
        unsafe.update(
            {
                "status": ["PROVEN"],
                "confidence": {"level": "high"},
                "remediation": {
                    "kind": ["automatic"],
                    "automatic_patch": "false",
                    "summary": "0123456789abcdef",
                },
            }
        )
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(root / "unsafe.json", report(findings=[unsafe]))
            document = merge_reports(root, limits=MergeLimits(max_string_chars=8))

        merged = document["findings"][0]
        self.assertEqual(merged["status"], "UNSPECIFIED")
        self.assertEqual(merged["confidence"], "unknown")
        self.assertEqual(merged["remediation"]["kind"], "manual-…")
        self.assertIs(merged["remediation"]["automatic_patch"], False)
        self.assertEqual(merged["remediation"]["summary"], "0123456…")

    def test_empty_directory_is_a_valid_empty_report(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            document = merge_reports(Path(directory))
            rendered = render_html(document)

        self.assertEqual(document["input"]["reports_processed"], 0)
        self.assertEqual(document["summary"]["finding_occurrences"], 0)
        self.assertEqual(document["findings"], [])
        self.assertIn("No per-TU reports found.", rendered)
        self.assertIn("No findings.", rendered)

    def test_malformed_json_is_recorded_and_strict_cli_fails(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "broken.json").write_text("{not json", encoding="utf-8")
            write_json(root / "valid.json", report(findings=[finding()]))
            json_output = root / "project.json"
            html_output = root / "project.html"
            stdout = io.StringIO()
            stderr = io.StringIO()
            with redirect_stdout(stdout), redirect_stderr(stderr):
                status = main(
                    [
                        str(root),
                        "--json",
                        str(json_output),
                        "--html",
                        str(html_output),
                        "--strict",
                    ]
                )
            merged = json.loads(json_output.read_text(encoding="utf-8"))

        self.assertEqual(status, 2)
        self.assertEqual(merged["input"]["reports_processed"], 1)
        self.assertEqual(merged["input"]["malformed_json_files"], 1)
        self.assertEqual(merged["input"]["error_count"], 1)
        self.assertEqual(merged["errors"][0]["report"], "broken.json")
        self.assertIn("warning: skipped 1", stderr.getvalue())

    def test_html_escapes_all_report_controlled_text(self) -> None:
        attack = '<script>alert("owned")</script>&'
        sample = {
            "rule_id": "SA003",
            "status": "INCONCLUSIVE",
            "reason": attack,
            "detail": attack,
            "location": {"file": attack, "line": 4, "column": 2},
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(
                root / "hostile.json",
                report(
                    source=attack,
                    findings=[finding(source=attack, message=attack)],
                    inconclusive=[sample],
                ),
            )
            rendered = render_html(merge_reports(root))

        self.assertNotIn("<script>alert", rendered)
        self.assertIn("&lt;script&gt;alert", rendered)
        self.assertIn("&amp;", rendered)

    def test_output_is_excluded_and_byte_deterministic(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(root / "z" / "tu.json", report(findings=[finding(line=20)]))
            write_json(root / "a" / "tu.json", report(findings=[finding(line=10)]))
            json_output = root / "static-analyzer-project-report.json"
            html_output = root / "static-analyzer-project-report.html"

            first = merge_reports(
                root, excluded_paths=(json_output, html_output)
            )
            write_reports(first, json_output, html_output)
            first_json = json_output.read_bytes()
            first_html = html_output.read_bytes()

            second = merge_reports(
                root, excluded_paths=(json_output, html_output)
            )
            write_reports(second, json_output, html_output)
            second_json = json_output.read_bytes()
            second_html = html_output.read_bytes()
            temporary_files = list(root.glob(".*.tmp"))

        self.assertEqual(first, second)
        self.assertEqual(first_json, second_json)
        self.assertEqual(first_html, second_html)
        self.assertEqual(temporary_files, [])
        self.assertEqual(
            [item["location"]["line"] for item in second["findings"]],
            [10, 20],
        )

    def test_old_schema_without_tool_metadata_is_accepted(self) -> None:
        legacy = {
            "source": "legacy.c",
            "summary": {"total": 1, "SA001": 1},
            "findings": [finding(source="legacy.c")],
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(root / "legacy.json", legacy)
            document = merge_reports(root)

        self.assertEqual(document["input"]["reports_processed"], 1)
        self.assertEqual(document["reports"][0]["schema_version"], 1)
        self.assertEqual(document["summary"]["finding_occurrences"], 1)
        merged = document["findings"][0]
        self.assertEqual(merged["status"], "UNSPECIFIED")
        self.assertEqual(merged["confidence"], "unknown")
        self.assertEqual(
            merged["remediation"],
            {
                "kind": "manual-guidance",
                "automatic_patch": False,
                "summary": "",
            },
        )

    def test_inconclusive_totals_survive_bounded_source_samples(self) -> None:
        sample = {
            "rule_id": "SA004",
            "status": "INCONCLUSIVE",
            "reason": "path-feasibility-budget",
            "detail": "bounded proof search",
            "location": {"file": "thread.c", "line": 8, "column": 2},
        }
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            write_json(
                root / "thread.json",
                report(
                    source="thread.c",
                    inconclusive=[sample],
                    inconclusive_total=5,
                ),
            )
            document = merge_reports(root)

        self.assertEqual(document["summary"]["inconclusive_total"], 5)
        self.assertEqual(document["summary"]["inconclusive_samples_seen"], 1)
        self.assertEqual(document["inconclusive_summary"]["source_samples_omitted"], 4)
        self.assertEqual(document["inconclusive_summary"]["by_rule"]["SA004"], 5)

    def test_record_and_provenance_limits_are_reported(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for index in range(3):
                write_json(
                    root / str(index) / "report.json",
                    report(
                        findings=[
                            finding(line=7),
                            finding(line=100 + index, message=f"unique {index}"),
                        ]
                    ),
                )
            document = merge_reports(
                root,
                limits=MergeLimits(
                    max_findings=1,
                    max_provenance_per_record=1,
                ),
            )

        self.assertTrue(document["input"]["truncated"])
        self.assertIn("max_finding_records", document["input"]["truncation_reasons"])
        self.assertEqual(document["summary"]["finding_occurrences"], 6)
        self.assertEqual(document["summary"]["unique_findings"], 1)
        self.assertGreater(document["summary"]["finding_records_dropped"], 0)
        self.assertEqual(document["findings"][0]["provenance_omitted"], 2)


if __name__ == "__main__":
    unittest.main()
