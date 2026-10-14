#!/usr/bin/env python3
"""Seeded, bounded ownership-program generation with an independent truth oracle.

Enumerate all Boolean equivalence classes of up to four input guards and one
callee selector. GCC compiles generated sources; resulting programs are never
executed. A missed leak is distinct from a false alarm or an inconclusive proof.
This is synthetic property coverage, not a replacement for Juliet accuracy.
"""
from __future__ import annotations

import argparse
import itertools
import json
from pathlib import Path
import random

import compare_lto_project as project


def generate(rng: random.Random, depth: int):
    if depth == 0 or rng.random() < .28:
        return rng.choice(("free", "free", "none", "maybe"))
    return (rng.randrange(4), rng.choice((True, False)),
            generate(rng, depth - 1), generate(rng, depth - 1))


def evaluate(tree, inputs):
    if isinstance(tree, str):
        return tree
    index, positive, yes, no = tree
    return evaluate(yes if inputs[index] == positive else no, inputs)


def render(tree, indent="  "):
    if isinstance(tree, str):
        return [indent + {"free": "free(value);", "none": "(void)value;",
                          "maybe": "maybe_release(value, release);"}[tree]]
    index, positive, yes, no = tree
    condition = f"g{index} {'!=' if positive else '=='} 0"
    return [f"{indent}if ({condition}) {{", *render(yes, indent + "  "),
            f"{indent}}} else {{", *render(no, indent + "  "), f"{indent}}}"]


def oracle(tree):
    witness = None
    independent = None
    for values in itertools.product((False, True), repeat=5):
        inputs, release = values[:4], values[4]
        leaf = evaluate(tree, inputs)
        # Interpret the callee selector independently of the static analyzer.
        leaked = leaf == "none" or (leaf == "maybe" and not release)
        if leaked and witness is None:
            witness = {"guards": list(inputs), "release": release, "leaf": leaf}
        if leaf == "none" and independent is None:
            independent = {"guards": list(inputs), "leaf": leaf}
    return {"leak_exists": witness is not None, "witness": witness,
            "independent_witness": independent}


def run_batch(compiler: Path, plugin: Path, source: Path, output: Path) -> dict:
    output.mkdir(parents=True, exist_ok=True)
    stem = output / "analysis"
    for suffix in (".json", ".html"):
        stem.with_suffix(suffix).unlink(missing_ok=True)
    options = [argument.replace("doublefreecheck=1", "doublefreecheck=0")
               .replace("useafterfree=1", "useafterfree=0")
               for argument in project.plugin_arguments(plugin, stem)]
    command = [str(compiler), "-O1", "-fipa-pta", "-fno-inline",
               "-fno-tree-dce", "-fno-tree-dse", "-fno-tree-fre",
               *options, "-c", str(source),
               "-o", str(output / "program.o")]
    result = project.run_process(command, Path.cwd(), output / "compile", 30)
    if result["status"] != "OK":
        raise RuntimeError(f"compiler failed: {output}: {result['status']}")
    result["report"] = project.load_plugin_report(stem.with_suffix(".json"))
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--baseline-plugin", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--seed", type=int, default=20261015)
    parser.add_argument("--cases", type=int, default=96)
    args = parser.parse_args()
    if not 1 <= args.cases <= 1024:
        parser.error("--cases must be between 1 and 1024")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    rng = random.Random(args.seed)
    programs = [generate(rng, 4) for _ in range(args.cases)]
    modes = {"candidate": args.plugin.resolve()}
    if args.baseline_plugin:
        modes["baseline"] = args.baseline_plugin.resolve()
    hashes = {name: project.sha256_file(plugin) for name, plugin in modes.items()}
    outcomes = {name: [] for name in modes}
    counterexamples = []
    runs = []
    for start in range(0, len(programs), 8):
        batch = output / f"batch-{start // 8:03d}"
        batch.mkdir(exist_ok=True)
        source = batch / "program.c"
        lines = ["#include <stdlib.h>", "__attribute__((noinline))",
                 "static void maybe_release(void *value, int release) {",
                 "  if (release) free(value);", "}"]
        cases = []
        for index, tree in enumerate(programs[start:start + 8], start):
            lines.append(f"void generated_{index}(int g0, int g1, int g2, int g3, int release) {{")
            allocation_line = len(lines) + 1
            lines.extend(["  void *value = malloc(32);", "  if (value == NULL) return;",
                          *render(tree), "}"])
            cases.append({"id": index, "allocation_line": allocation_line,
                          "program": tree, **oracle(tree)})
        source.write_text("\n".join(lines) + "\n")
        for name, plugin in modes.items():
            result = run_batch(args.compiler.resolve(), plugin, source, batch / name)
            report = result.pop("report")
            findings = {finding["location"]["line"] for finding in report["findings"]
                        if finding["rule_id"] == "SA001"}
            known_lines = {case["allocation_line"] for case in cases}
            if findings - known_lines:
                counterexamples.append({"mode": name, "batch": str(source),
                                        "unmapped_finding_lines": sorted(findings - known_lines)})
            for case in cases:
                detected = case["allocation_line"] in findings
                row = {**case, "detected": detected, "source": str(source)}
                outcomes[name].append(row)
                if detected and not case["leak_exists"]:
                    counterexamples.append({"mode": name, **row})
            runs.append({"mode": name, "source": str(source),
                         "source_sha256": project.sha256_file(source),
                         "process": result, "inconclusive_summary": report["inconclusive_summary"]})
    if hashes != {name: project.sha256_file(plugin) for name, plugin in modes.items()}:
        raise RuntimeError("plugin changed during fuzz validation")
    summary = {name: {"cases": len(rows),
                       "known_leaks": sum(row["leak_exists"] for row in rows),
                       "safe": sum(not row["leak_exists"] for row in rows),
                       "detected": sum(row["detected"] for row in rows),
                       "false_alarms": sum(row["detected"] and not row["leak_exists"] for row in rows),
                       "independent_witnesses_detected": sum(row["detected"] and row["independent_witness"] is not None for row in rows),
                       "independent_witnesses": sum(row["independent_witness"] is not None for row in rows)}
               for name, rows in outcomes.items()}
    report = {"status": "FAIL" if counterexamples else "OK", "seed": args.seed,
              "scope": "Bounded synthetic C ownership programs; exhaustive 16 guard classes plus callee selector; no execution of generated binaries; no real-world accuracy or universal zero-FP claim.",
              "plugin_sha256": hashes, "summary": summary,
              "counterexamples": counterexamples, "cases": outcomes, "runs": runs}
    (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({"status": report["status"], "summary": summary}, indent=2))
    return bool(counterexamples)


if __name__ == "__main__":
    raise SystemExit(main())
