#!/usr/bin/env python3
"""Serial paired compiler-cost probe on one retained synthetic C source.

Measures compiler plus waited-for descendants CPU, not analyzer-only CPU.
Plugin modes enable SA001 and JSON reports; native uses its normal rule bundle.
These sampled compile costs do not establish arbitrary-project superiority.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import random
import resource
import statistics

import compare_lto_project as project


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--baseline-plugin", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--rounds", type=int, default=10)
    args = parser.parse_args()
    if not 3 <= args.rounds <= 30:
        parser.error("--rounds must be between 3 and 30")
    source, compiler, output = args.source.resolve(), args.compiler.resolve(), args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    plugins = {"baseline": args.baseline_plugin.resolve(), "candidate": args.plugin.resolve()}
    inputs = {str(path): project.sha256_file(path) for path in [source, compiler, *plugins.values()]}
    rows = []
    rng = random.Random(20261015)
    for round_number in range(-2, args.rounds):
        order = ["plain", "baseline", "candidate", "native"]
        rng.shuffle(order)
        for mode in order:
            dest = output / f"round-{round_number + 2:02d}" / mode
            dest.mkdir(parents=True, exist_ok=True)
            extra = []
            if mode in plugins:
                extra = [arg.replace("doublefreecheck=1", "doublefreecheck=0")
                         .replace("useafterfree=1", "useafterfree=0")
                         .replace("reporthtml=1", "reporthtml=0")
                         for arg in project.plugin_arguments(plugins[mode], dest / "analysis")]
            elif mode == "native":
                extra = ["-fanalyzer", "-fanalyzer-verbosity=0", "-fdiagnostics-format=sarif-stderr"]
            command = [str(compiler), "-O1", "-fipa-pta", "-fno-inline",
                       "-fno-tree-dce", "-fno-tree-dse", "-fno-tree-fre", *extra,
                       "-c", str(source), "-o", str(dest / "case.o")]
            before = resource.getrusage(resource.RUSAGE_CHILDREN)
            result = project.run_process(command, Path.cwd(), dest / "compile", 30)
            after = resource.getrusage(resource.RUSAGE_CHILDREN)
            if result["status"] != "OK":
                raise RuntimeError(f"{mode} compile failed: {dest}")
            cpu = 1000 * (after.ru_utime + after.ru_stime - before.ru_utime - before.ru_stime)
            rows.append({"round": round_number, "mode": mode, "warmup": round_number < 0,
                         "cpu_ms": cpu, "process": result})
    if inputs != {path: project.sha256_file(Path(path)) for path in inputs}:
        raise RuntimeError("measurement input changed")
    summary = {}
    for mode in ("plain", "baseline", "candidate", "native"):
        timed = [row for row in rows if row["mode"] == mode and not row["warmup"]]
        summary[mode] = {"median_cpu_ms": statistics.median(row["cpu_ms"] for row in timed),
                         "median_wall_ms": statistics.median(row["process"]["duration_ms"] for row in timed),
                         "cpu_ms": [row["cpu_ms"] for row in timed]}
    paired = {}
    for reference in ("baseline", "native"):
        ratios = []
        for round_number in range(args.rounds):
            same_round = {row["mode"]: row["cpu_ms"] for row in rows if row["round"] == round_number}
            ratios.append(same_round["candidate"] / same_round[reference])
        paired[reference] = {"median_candidate_cpu_ratio": statistics.median(ratios), "ratios": ratios}
    report = {"status": "OK", "scope": __doc__, "warmups": 2, "rounds": args.rounds,
              "seed": 20261015, "inputs_sha256": inputs, "summary": summary,
              "paired": paired, "runs": rows}
    (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({"summary": summary, "paired": paired}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
