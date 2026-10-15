#!/usr/bin/env python3
"""Repeat clean builds of the FFmpeg configuration retained by the audit tool.

Times make/compiler/wrapper/report costs, excluding configure and make clean.
Uses copied build directories and preserves all original measurement records.
This narrow build gate is not a universal performance or precision guarantee.
"""
from __future__ import annotations

import argparse
import json
import math
from pathlib import Path
import resource
import shutil
import statistics
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.audit_realworld_precision import input_manifest, summarize
from tools.compare_lto_project import run_process, sha256_file


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--audit-report", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--rounds", type=int, default=3)
    parser.add_argument("--warmups", type=int, default=1)
    parser.add_argument("--jobs", type=int, default=1)
    parser.add_argument("--timeout", type=float, default=300)
    parser.add_argument("--library-model", choices=("none", "ffmpeg"), default="ffmpeg")
    parser.add_argument("--max-overhead", type=float, default=0.10)
    args = parser.parse_args()
    if not 2 <= args.rounds <= 10 or not 0 <= args.warmups <= 2 or not 1 <= args.jobs <= 16:
        parser.error("rounds=2..10, warmups=0..2, jobs=1..16 required")
    if not math.isfinite(args.timeout) or args.timeout <= 0 or not 0 <= args.max_overhead <= 1:
        parser.error("positive finite timeout and max-overhead=0..1 required")
    audit = json.loads(args.audit_report.read_text())
    inputs = json.loads((args.audit_report.parent / "inputs.json").read_text())
    project = next(p for p in audit["projects"] if p["project"].startswith("ffmpeg-"))
    compiler = Path(inputs["compiler"])
    source = Path(inputs["sources"][0]["root"])
    if sha256_file(compiler) != inputs["compiler_sha256"]:
        raise ValueError("compiler changed since configuration audit")
    source_before = input_manifest(source)
    if source_before["sha256"] != inputs["sources"][0]["sha256"]:
        raise ValueError("source changed since configuration audit")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    snapshot = output / "plugin-snapshot" / "newplugin.so"
    snapshot.parent.mkdir()
    shutil.copy2(args.plugin.resolve(), snapshot)
    plugin_hash = sha256_file(snapshot)
    tools_dir = Path(__file__).resolve().parent
    builds, commands, expected = {}, {}, {}
    for mode in ("baseline", "plugin"):
        old = project["modes"][mode]
        directory = output / "builds" / mode
        shutil.copytree(Path(old["build"]["cwd"]), directory, symlinks=True,
                        ignore=shutil.ignore_patterns("records", "*.o", "*.a", "*.d",
                                                     "*.stdout.txt", "*.stderr.txt"))
        command = old["build"]["command"]
        if command[0] != "make":
            raise ValueError("expected an audited make invocation")
        commands[mode] = [arg for arg in command if not arg.startswith(("-j", "CC="))]
        builds[mode] = directory
        expected[mode] = old["summary"]["compiler_processes"]
    rows = []
    for round_number in range(-args.warmups, args.rounds):
        # Alternating order avoids always charging one mode for the first build.
        order = ("baseline", "plugin") if round_number % 2 == 0 else ("plugin", "baseline")
        for mode in order:
            run_dir = output / f"round-{round_number + args.warmups:02d}" / mode
            run_dir.mkdir(parents=True)
            records = run_dir / "records"
            config = {"compiler": str(compiler), "plugin": str(snapshot),
                      "records": str(records), "mode": mode, "timeout": args.timeout,
                      "report_only": True, "library_model": args.library_model}
            wrapper = builds[mode] / "benchmark-cc"
            wrapper.write_text("#!/usr/bin/env python3\nimport sys\n"
                               f"sys.path.insert(0, {str(tools_dir)!r})\n"
                               "from audit_realworld_precision import compiler_wrapper\n"
                               f"raise SystemExit(compiler_wrapper({config!r}, sys.argv[1:]))\n")
            wrapper.chmod(0o700)
            clean = run_process(["make", "clean"], builds[mode], run_dir / "clean", args.timeout)
            if clean["status"] != "OK":
                raise RuntimeError("make clean failed; retained logs identify the invocation")
            command = [*commands[mode], f"-j{args.jobs}", f"CC={wrapper}"]
            before = resource.getrusage(resource.RUSAGE_CHILDREN)
            process = run_process(command, builds[mode], run_dir / "build", args.timeout)
            after = resource.getrusage(resource.RUSAGE_CHILDREN)
            cpu_ms = 1000 * (after.ru_utime + after.ru_stime - before.ru_utime - before.ru_stime)
            summary = summarize(records)
            if process["status"] != "OK" or summary["statuses"] != {"OK": expected[mode]}:
                raise RuntimeError("build failed or compiled unit count changed")
            if mode == "plugin" and summary["reports"] != expected[mode]:
                raise RuntimeError("plugin reports are incomplete")
            row = {"round": round_number, "warmup": round_number < 0, "mode": mode,
                   "cpu_ms": cpu_ms, "process": process, "summary": summary}
            rows.append(row)
            (run_dir / "result.json").write_text(json.dumps(row, indent=2) + "\n")
            print(round_number, mode, process["status"], process["duration_ms"],
                  "ms", summary["compiler_processes"], "units", flush=True)
    if input_manifest(source) != source_before or sha256_file(snapshot) != plugin_hash:
        raise RuntimeError("measurement inputs changed")
    summary = {}
    for mode in ("baseline", "plugin"):
        timed = [r for r in rows if r["mode"] == mode and not r["warmup"]]
        summary[mode] = {"median_wall_ms": statistics.median(r["process"]["duration_ms"] for r in timed),
                         "median_cpu_ms": statistics.median(r["cpu_ms"] for r in timed),
                         "units_per_run": expected[mode],
                         "findings_per_run": [len(r["summary"]["findings"]) for r in timed]}
    paired = {}
    for metric in ("wall", "cpu"):
        ratios = []
        for number in range(args.rounds):
            same = {r["mode"]: r for r in rows if r["round"] == number}
            value = lambda r: r["cpu_ms"] if metric == "cpu" else r["process"]["duration_ms"]
            ratios.append(value(same["plugin"]) / value(same["baseline"]))
        paired[metric] = {"ratios": ratios, "median_plugin_to_baseline_ratio": statistics.median(ratios)}
    passed = all(p["median_plugin_to_baseline_ratio"] <= 1 + args.max_overhead for p in paired.values())
    report = {"status": "OK", "scope": __doc__, "rounds": args.rounds, "warmups": args.warmups,
              "jobs": args.jobs, "library_model": args.library_model, "report_only": True,
              "plugin_sha256": plugin_hash, "compiler_sha256": sha256_file(compiler),
              "source_sha256": source_before["sha256"], "source_unchanged": True,
              "summary": summary, "paired": paired, "runs": rows,
              "gate": {"maximum_median_overhead": args.max_overhead, "passed": passed,
                       "scope": "This configured FFmpeg core build only; no general speed guarantee."}}
    (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({"summary": summary, "paired": paired, "gate": report["gate"]}, indent=2))
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
