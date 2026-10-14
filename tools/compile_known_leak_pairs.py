#!/usr/bin/env python3
"""Compile upstream leak-fix file pairs, keeping baseline failures separate.

This is an affected-file experiment, not a build of either historical project.
The supplied FFmpeg tree/configuration provides compatibility headers. The
manifest records the exact upstream revisions of the affected C sources.
No source changes, ownership annotations, allocator substitutions, or stubs
are injected. Nonzero findings require manual target-object review.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import shutil
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.audit_realworld_precision import FLAGS, input_manifest, plugin_flags
from tools.compare_lto_project import run_process, sha256_file


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--manifest", type=Path, required=True)
    p.add_argument("--source-root", type=Path, required=True)
    p.add_argument("--ffmpeg-source", type=Path, required=True)
    p.add_argument("--ffmpeg-build-dir", type=Path, required=True)
    p.add_argument("--compiler", type=Path, required=True)
    p.add_argument("--plugin", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--timeout", type=float, default=90)
    p.add_argument("--library-model", choices=("none", "ffmpeg"), default="none")
    args = p.parse_args()
    manifest = json.loads(args.manifest.read_text())
    root, source, build = (x.resolve() for x in
                           (args.source_root, args.ffmpeg_source, args.ffmpeg_build_dir))
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    snapshot = output / "plugin-snapshot" / "newplugin.so"
    snapshot.parent.mkdir()
    shutil.copy2(args.plugin.resolve(), snapshot)
    inputs = [args.manifest.resolve(), build / "config.h", build / "config_components.h"]
    for case in manifest["ffmpeg_pairs"]:
        inputs.extend(root / case["id"] / v / case["file"] for v in ("bad", "good"))
        headers = root / case["id"] / "headers"
        if headers.is_dir():
            inputs.extend(sorted(x for x in headers.rglob("*.h") if x.is_file()))
    before = [input_manifest(x) for x in inputs]
    results = []
    for case in manifest["ffmpeg_pairs"]:
        variants = {}
        for variant in ("bad", "good"):
            original = root / case["id"] / variant / case["file"]
            directory = output / case["id"] / variant
            directory.mkdir(parents=True)
            modes = {}
            overlay = root / case["id"] / "headers"
            includes = [build, overlay, source]
            # Historical headers use sibling quoted includes. Never add
            # libavutil with -I: its time.h would shadow the host <time.h>.
            quote_includes = [overlay / Path(case["file"]).parent,
                              source / Path(case["file"]).parent,
                              overlay / "libavutil", source / "libavutil"]
            for mode in ("baseline", "plugin"):
                command = [str(args.compiler.resolve()), *FLAGS, "-std=c17",
                           "-D_GNU_SOURCE", "-D_ISOC11_SOURCE", "-DHAVE_AV_CONFIG_H",
                           *["-I" + str(x) for x in includes],
                           *[token for x in quote_includes for token in ("-iquote", str(x))],
                           "-c", str(original), "-o", str(directory / (mode + ".o"))]
                if mode == "plugin":
                    command += plugin_flags(snapshot, directory / "analysis")
                    if args.library_model != "none":
                        command.append("-fplugin-arg-newplugin-librarymodel=" + args.library_model)
                execution = run_process(command, build, directory / mode, args.timeout)
                modes[mode] = execution
                (directory / (mode + ".json")).write_text(json.dumps(execution, indent=2) + "\n")
                print(case["id"], variant, mode, execution["status"], flush=True)
                if mode == "baseline" and execution["status"] != "OK":
                    break
            analysis_path = directory / "analysis.json"
            analysis = json.loads(analysis_path.read_text()) if analysis_path.exists() else None
            assessment = "BASELINE_BUILD_FAIL"
            if modes["baseline"]["status"] == "OK":
                if modes["plugin"]["status"] != "OK":
                    assessment = "PLUGIN_BUILD_FAIL"
                elif analysis is None:
                    assessment = "REPORT_MISSING"
                elif analysis["summary"]["total"]:
                    assessment = "REQUIRES_TARGET_REVIEW"
                else:
                    assessment = "KNOWN_LEAK_MISSED" if variant == "bad" else "NO_ALERT_ON_FIXED_FILE"
            variants[variant] = {"modes": modes, "analysis": analysis,
                                 "assessment": assessment, "source_sha256": sha256_file(original)}
        results.append({**case, "variants": variants})
    after = [input_manifest(x) for x in inputs]
    report = {"scope": __doc__, "plugin_sha256": sha256_file(snapshot),
              "compiler_sha256": sha256_file(args.compiler.resolve()),
              "ffmpeg_header_context": str(source), "ffmpeg_build_config": str(build),
              "limits": "Header compatibility context is not either complete historical checkout; cross-TU provider bodies are unavailable in this object-only experiment. No global TP/FP or precision score is inferred.",
              "input_files_unchanged": before == after, "inputs": before, "cases": results}
    (output / "report.json").write_text(json.dumps(report, indent=2) + "\n")
    print("Report:", output / "report.json", flush=True)
    return 0 if before == after else 1


if __name__ == "__main__":
    raise SystemExit(main())
