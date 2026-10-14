#!/usr/bin/env python3
"""Rank observed blockers from a completed Juliet project report.

Each missed group belongs to exactly one reason-set bucket. These are observed
analysis blockers, not independently proved root causes or recovered cases.
Case detection, exact-site scoring and cross-rule alert burden stay distinct.
"""
from __future__ import annotations

import argparse
from collections import defaultdict
import hashlib
import html
import json
import os
from pathlib import Path


def summarize(document: dict) -> dict:
    if (document.get("status") != "OK" or document.get("changed_inputs") != []
            or document.get("benchmark_mode", {}).get("kind") != "lto-project-oracle"):
        raise ValueError("requires a complete LTO project report without input drift")
    buckets = defaultdict(list)
    seen = set()
    totals = defaultdict(int)
    for group in document["groups"]:
        identity = group["group_id"]
        if identity in seen:
            raise ValueError(f"duplicate group: {identity}")
        seen.add(identity)
        variant = group["variants"]["bad"]
        if any(variant[tool]["status"] != "OK" for tool in ("plugin", "gcc16_fanalyzer")):
            raise ValueError(f"incomplete group: {identity}")
        if variant["plugin"]["case_detection_classification"] != "FN":
            continue
        rule = variant["plugin"]["target_rule"]
        sources = {str(Path(source).resolve()) for source in group["sources"]}
        evidence = variant["project_modes"]["plugin"]["plugin_report"]
        samples = [item for item in evidence.get("inconclusive", [])
                   if item["rule_id"] == rule
                   and str(Path(item["location"]["file"]).resolve()) in sources]
        reasons = sorted({item["reason"] for item in samples})
        # Truncated samples cannot justify the no-candidate classification.
        if evidence["inconclusive_summary"].get("samples_omitted", 0):
            reasons.append("report-samples-truncated")
        reasons = reasons or ["no-target-candidate"]
        native_detects = variant["gcc16_fanalyzer"]["case_detection_classification"] == "TP"
        row = {"group_id": identity, "native_detects": native_detects,
               "sources": group["sources"], "samples": samples,
               "report": variant["project_modes"]["plugin"]["plugin_html_path"]}
        buckets[(group["cwe"], tuple(reasons))].append(row)
        totals[group["cwe"]] += 1
    ranked = [{"cwe": cwe, "reasons": list(reasons), "missed": len(rows),
               "native_only": sum(row["native_detects"] for row in rows), "groups": rows}
              for (cwe, reasons), rows in buckets.items()]
    ranked.sort(key=lambda item: (-item["missed"], item["cwe"], item["reasons"]))
    accuracy = document["common_paired_accuracy"]
    expected = accuracy["plugin"]["case_detection_confusion_matrix"]["FN"]
    if sum(totals.values()) != expected:
        raise ValueError("missed-group partition disagrees with report accuracy")
    return {"schema_version": 1, "scoring": "case-detection / target-rule",
            "plugin_sha256": document["plugin_sha256"], "compiler": document["compiler"],
            "benchmark_mode": document["benchmark_mode"], "accuracy": accuracy,
            "missed_by_cwe": dict(totals), "buckets": ranked,
            "limits": "Observed reason sets partition misses; absence of a target candidate is not a safety proof. Cached native results retain their original validation limits.",
            "reference_limits": document.get("reference_cache", {}).get("limits")}


def write_html(report: dict, path: Path) -> None:
    escape = html.escape
    parts = ["<!doctype html><html lang=en><meta charset=utf-8>",
             "<meta name=viewport content='width=device-width,initial-scale=1'>",
             "<title>Analyzer improvement priorities</title><style>",
             "body{font:15px system-ui;background:#f3f5f9;color:#172033;margin:0}main{max-width:1100px;margin:32px auto;padding:0 20px}h1{font-size:30px}.cards{display:flex;gap:16px;flex-wrap:wrap}.card,details{background:white;border:1px solid #dce2eb;border-radius:12px;padding:20px;margin:14px 0}.card strong{display:block;font-size:30px}table{width:100%;border-collapse:collapse}th,td{text-align:left;padding:12px 8px;border-bottom:1px solid #e2e7ef;vertical-align:top}.muted{color:#56647a}summary{cursor:pointer;font-weight:650}.pill{color:#a64b00}code{overflow-wrap:anywhere}a{color:#245bd6}.bar{height:8px;background:#e9edf4;border-radius:4px;margin-top:12px}.bar span{display:block;height:100%;background:#dc7d26;border-radius:4px}",
             "</style><main><h1>Analyzer improvement priorities</h1>",
             "<p class=muted>Verified project benchmark · target-rule case detection · no injected contracts</p><div class=cards>"]
    for tool, title in (("plugin", "Plugin"), ("gcc16_fanalyzer", "GCC 16.2")):
        score = report["accuracy"][tool]
        matrix = score["case_detection_confusion_matrix"]
        recall = score["case_detection_metrics_on_evaluated_variants"]["recall"]
        parts.append(f"<div class=card>{title}<strong>{recall:.1%}</strong>{matrix['TP']} detected · {matrix['FN']} missed · {matrix['FP']} target-rule FP</div>")
    parts.append("</div><p>Start with the largest observed blocker. Expand a bucket for source locations and analysis evidence.</p>")
    for bucket in report["buckets"]:
        count = bucket["missed"]
        total = report["missed_by_cwe"][bucket["cwe"]]
        label = escape(" + ".join(bucket["reasons"]))
        parts.append(f"<details><summary>CWE{bucket['cwe']} · {count} missed · {label}</summary><p class=pill>{bucket['native_only']} also detected by GCC · {count / total:.1%} of this CWE's misses</p><div class=bar><span style='width:{100 * count / total:.1f}%'></span></div><table><tr><th>Case</th><th>GCC detects</th><th>Observed blocker</th></tr>")
        for row in bucket["groups"]:
            link = escape(os.path.relpath(row["report"], path.parent), quote=True)
            evidence = "<br>".join(escape(f"{Path(sample['location']['file']).name}:{sample['location']['line']} — {sample['reason']}: {sample.get('detail', '')}") for sample in row["samples"])
            related = [point for sample in row["samples"] for point in sample.get("related", [])[:8]]
            if related:
                evidence += "<ul>" + "".join(f"<li>{escape(point['role'])}: <code>{escape(Path(point['file']).name)}:{point['line']}</code></li>" for point in related) + "</ul>"
            parts.append(f"<tr><td><a href='{link}'>{escape(row['group_id'])}</a></td><td>{'Yes' if row['native_detects'] else 'No'}</td><td>{evidence or 'No recorded target candidate; investigate allocation/summary discovery.'}</td></tr>")
        parts.append("</table></details>")
    parts.append("<p class=muted>Target-rule FP excludes other lifetime rules. Good variants with any lifetime alert: ")
    parts.append(" · ".join(f"{name}: {report['accuracy'][tool]['alert_burden_confusion_matrix']['FP']}" for tool, name in (("plugin", "Plugin"), ("gcc16_fanalyzer", "GCC"))))
    parts.append(". No universal zero-FP or speed claim follows from this benchmark.</p>")
    parts.append(f"<p class=muted>{escape(report['limits'])}</p><p><code>Plugin SHA-256: {escape(report['plugin_sha256'])}</code></p></main></html>")
    path.write_text("\n".join(parts), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    data = args.report.read_bytes()
    report = summarize(json.loads(data))
    report.update(input_report=str(args.report.resolve()), input_sha256=hashlib.sha256(data).hexdigest())
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / "triage.json").write_text(json.dumps(report, indent=2) + "\n")
    write_html(report, args.output / "triage.html")
    for bucket in report["buckets"]:
        print(f"CWE{bucket['cwe']}: {bucket['missed']} missed, {bucket['native_only']} native-only: {' + '.join(bucket['reasons'])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
