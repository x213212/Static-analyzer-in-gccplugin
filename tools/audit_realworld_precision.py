#!/usr/bin/env python3
"""Compile retained real sources and retain every warning and abstention.

No accuracy oracle is inferred from project reputation or warning counts.
FFmpeg uses its native out-of-tree configure/make build with a documented
core-library configuration; SQLite compiles the complete amalgamation.
The resulting programs are never executed. No ownership contracts are injected.
"""
from __future__ import annotations

import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import math
from pathlib import Path
import shutil
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.compare_lto_project import run_process, sha256_file

FLAGS = ["-O1", "-fipa-pta", "-fno-inline", "-fno-tree-dce",
         "-fno-tree-dse", "-fno-tree-fre", "-fdiagnostics-color=never"]


def plugin_flags(plugin: Path, stem: Path, report_only: bool = False,
                 library_model: str = "none") -> list[str]:
    options = ["analysispolicy=proof-only", "legacyengine=0", "debugoutput=0",
               "traceallfucntion=1", "memoryleakcheck=1", "doublefreecheck=1",
               "useafterfree=1", "threadmod=0", "smt=1", "report=1",
               "reporthtml=0", f"reportpath={stem}"]
    if report_only:
        options.append("reportonly=1")
    if library_model != "none":
        options.append("librarymodel=" + library_model)
    return [f"-fplugin={plugin}", *[f"-fplugin-arg-newplugin-{x}" for x in options]]


def compiler_wrapper(config: dict, argv: list[str]) -> int:
    """One record and report per object; build-system probes are separate."""
    cwd = Path.cwd()
    object_name = argv[argv.index("-o") + 1] if "-o" in argv else "probe"
    key = hashlib.sha256(str(cwd / object_name).encode()).hexdigest()[:20]
    directory = Path(config["records"]) / key
    directory.mkdir(parents=True, exist_ok=True)
    command = [config["compiler"], *argv]
    if config["mode"] == "plugin" and "-c" in argv:
        command += plugin_flags(Path(config["plugin"]), directory / "analysis",
                                config.get("report_only", False),
                                config.get("library_model", "none"))
    result = run_process(command, cwd, directory / "compiler", config["timeout"])
    result["object"] = object_name
    result["sources"] = [str((cwd / x).resolve()) for x in argv if x.endswith(".c")]
    (directory / "execution.json").write_text(json.dumps(result, indent=2) + "\n")
    for stream, target in (("stdout_path", sys.stdout), ("stderr_path", sys.stderr)):
        target.write(Path(result[stream]).read_text(errors="replace"))
    return int(result["exit_code"] or (0 if result["status"] == "OK" else 1))


def input_manifest(source: Path) -> dict:
    files = [source] if source.is_file() else sorted(
        p for p in source.rglob("*") if p.is_file())
    hashes = {str(p.relative_to(source.parent if source.is_file() else source)):
              sha256_file(p) for p in files}
    digest = hashlib.sha256(json.dumps(hashes, sort_keys=True).encode()).hexdigest()
    return {"root": str(source), "sha256": digest, "files": hashes}


def summarize(directory: Path) -> dict:
    executions = [json.loads(p.read_text()) for p in sorted(directory.glob("*/execution.json"))]
    reports = [json.loads(p.read_text()) for p in sorted(directory.glob("*/analysis.json"))]
    findings, unknown = [], Counter()
    skipped = []
    for report in reports:
        findings.extend(report.get("findings", []))
        unknown.update(report.get("inconclusive_summary", {}).get("by_reason", {}))
        budget = report.get("analysis_budget", {})
        if "functions_skipped" in budget:
            skipped.append(int(budget["functions_skipped"]))
    return {"compiler_processes": len(executions),
            "statuses": dict(Counter(e["status"] for e in executions)),
            "compiler_wall_ms_sum": round(sum(e["duration_ms"] for e in executions), 3),
            "reports": len(reports), "findings": findings,
            "rules": dict(Counter(f["rule_id"] for f in findings)),
            "inconclusive_reasons": dict(unknown),
            "functions_skipped": sum(skipped) if len(skipped) == len(reports) else None,
            "accuracy_scored": False}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--sqlite-source", type=Path, action="append", default=[])
    parser.add_argument("--ffmpeg-source", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--timeout", type=float, default=120)
    parser.add_argument("--report-only", action="store_true",
                        help="retain findings without emitting compiler warnings")
    parser.add_argument("--library-model", choices=("none", "ffmpeg"), default="none")
    args = parser.parse_args()
    if args.jobs < 1 or not math.isfinite(args.timeout) or args.timeout <= 0:
        parser.error("jobs and timeout must be positive")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    snapshot = output / "plugin-snapshot" / "newplugin.so"
    snapshot.parent.mkdir()
    shutil.copy2(args.plugin.resolve(), snapshot)
    inputs = [p.resolve() for p in args.sqlite_source]
    if args.ffmpeg_source:
        inputs.append(args.ffmpeg_source.resolve())
    manifests = [input_manifest(p) for p in inputs]
    (output / "inputs.json").write_text(json.dumps({
        "plugin_sha256": sha256_file(snapshot),
        "compiler": str(args.compiler.resolve()),
        "compiler_sha256": sha256_file(args.compiler.resolve()),
        "sources": manifests}, indent=2) + "\n")
    results = []

    def sqlite(source: Path) -> dict:
        project = output / source.parent.name
        project.mkdir()
        modes = {}
        for mode in ("baseline", "plugin"):
            directory = project / mode
            directory.mkdir()
            command = [str(args.compiler.resolve()), *FLAGS, "-fPIC", "-pthread",
                       "-c", str(source), "-o", str(directory / "sqlite3.o")]
            if mode == "plugin":
                command += plugin_flags(snapshot, directory / "analysis", args.report_only,
                                        args.library_model)
            result = run_process(command, directory, directory / "compile", args.timeout)
            modes[mode] = result
            if mode == "plugin" and (directory / "analysis.json").exists():
                modes[mode]["analysis"] = json.loads((directory / "analysis.json").read_text())
            print(source.parent.name, mode, result["status"], result["duration_ms"], flush=True)
        return {"project": source.parent.name, "scope": "complete sqlite3.c amalgamation; object compilation", "modes": modes}

    # Separate large SQLite TUs from the parallel FFmpeg build to reduce contention.
    with ThreadPoolExecutor(max_workers=min(args.jobs, 2)) as pool:
        results.extend(pool.map(sqlite, [p.resolve() for p in args.sqlite_source]))
    if args.ffmpeg_source:
        source = args.ffmpeg_source.resolve()
        project = output / source.name
        project.mkdir()
        modes = {}
        for mode in ("baseline", "plugin"):
            build = project / mode
            build.mkdir()
            configure = [str(source / "configure"), f"--cc={args.compiler.resolve()}",
                         "--disable-everything", "--disable-programs", "--disable-doc",
                         "--disable-autodetect", "--disable-network", "--disable-asm",
                         "--disable-x86asm", "--disable-avdevice", "--disable-avfilter",
                         "--disable-swresample", "--disable-swscale", "--optflags=-O1",
                         "--extra-cflags=" + " ".join(FLAGS[1:])]
            config_result = run_process(configure, build, build / "configure", args.timeout)
            modes[mode] = {"configure": config_result}
            if config_result["status"] != "OK":
                print(source.name, mode, "CONFIGURE", config_result["status"], flush=True)
                continue
            records = build / "records"
            records.mkdir()
            config = {"compiler": str(args.compiler.resolve()), "plugin": str(snapshot),
                      "records": str(records), "mode": mode, "timeout": args.timeout,
                      "report_only": args.report_only}
            config["library_model"] = args.library_model
            wrapper = build / "audit-cc"
            wrapper.write_text("#!/usr/bin/env python3\nimport sys\nsys.path.insert(0, " +
                               repr(str(Path(__file__).resolve().parent)) + ")\n" +
                               "from audit_realworld_precision import compiler_wrapper\n" +
                               "raise SystemExit(compiler_wrapper(" + repr(config) + ", sys.argv[1:]))\n")
            wrapper.chmod(0o755)
            command = ["make", f"-j{args.jobs}", f"CC={wrapper}",
                       "libavutil/libavutil.a", "libavcodec/libavcodec.a", "libavformat/libavformat.a"]
            build_result = run_process(command, build, build / "make", args.timeout * 8)
            modes[mode].update({"build": build_result, "summary": summarize(records)})
            print(source.name, mode, build_result["status"], modes[mode]["summary"]["rules"], flush=True)
        results.append({"project": source.name,
                        "scope": "native out-of-tree core-library build; codecs/demuxers, programs, ASM and external autodetection disabled; no LTO", "modes": modes})
    after = [input_manifest(p) for p in inputs]
    report = {"schema_version": 1, "accuracy_scored": False,
              "scope": "Observed findings require manual allocation/release/path review; zero warnings does not establish precision or safety. Objects/libraries are built but never executed.",
              "source_files_unchanged": after == manifests, "projects": results}
    (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print("Report:", output / "report.json", flush=True)
    return 0 if after == manifests else 1


if __name__ == "__main__":
    raise SystemExit(main())
