#!/usr/bin/env python3
"""Render recorded project detections or comparisons as offline bilingual HTML."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

import project_report_html as presentation


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report", type=Path, required=True)
    parser.add_argument("--speed-report", type=Path)
    parser.add_argument("--graph-report", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--detections-only", action="store_true",
                        help="show the whole-batch detection list without analyzer comparison tabs")
    parser.add_argument("--expand-problems", action="store_true",
                        help="open the cases tab and expand the current page's problem cards")
    args = parser.parse_args()
    try:
        report = json.loads(args.report.read_text())
        speed = json.loads(args.speed_report.read_text()) if args.speed_report else None
        graphs = json.loads(args.graph_report.read_text()) if args.graph_report else None
        if graphs is not None and (graphs.get('status') != 'OK' or
                                  graphs.get('report_sha256') != hashlib.sha256(args.report.read_bytes()).hexdigest()):
            raise ValueError('call graph evidence does not match this recorded comparison')
        if report.get("benchmark_mode", {}).get("kind") != "lto-project-oracle":
            raise ValueError("requires a recorded LTO project comparison")
        default_output = (args.report.with_name("detection.html") if args.detections_only
                          else args.report.with_suffix(".html"))
        output = (args.output or default_output).resolve()
        rendered = presentation.render_html(report, output.parent, speed, args.expand_problems, graphs,
                                           detection_only=args.detections_only)
        output.parent.mkdir(parents=True, exist_ok=True)
        temporary = output.with_name(output.name + ".tmp")
        temporary.write_text(rendered, encoding="utf-8")
        temporary.replace(output)
        inputs = [args.report, Path(presentation.__file__),
                  Path(presentation.__file__).with_name("project_report_template.html")]
        if args.speed_report:
            inputs.append(args.speed_report)
        if args.graph_report:
            inputs.append(args.graph_report)
        metadata = {"kind": "presentation-only", "analysis_rerun": False,
                    "expand_problems": args.expand_problems,
                    "detections_only": args.detections_only,
                    "input_hashes": {str(p.resolve()): hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs}}
        output.with_suffix(".presentation.json").write_text(json.dumps(metadata, indent=2) + "\n")
    except (OSError, ValueError, KeyError, TypeError) as error:
        parser.exit(2, f"report rendering failed: {error}\n")
    print(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
