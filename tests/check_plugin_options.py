#!/usr/bin/env python3
"""Check the production option parser through real GCC plugin invocations."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import subprocess


REMOVED_OPTIONS = (
    "legacyengine", "debugoutput", "debugmod", "traceallfucntion",
    "contextsentive", "relatemod", "retnotheapobjskipcheck",
    "prebranchexit", "vscode_extensionmod",
)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", required=True)
    parser.add_argument("--plugin", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    source = output / "input.c"
    source.write_text("int option_probe(void) { return 0; }\n")
    plugin = args.plugin.resolve()
    prefix = f"-fplugin-arg-{plugin.stem}-"
    runs = []
    for key in REMOVED_OPTIONS:
        for value in ("0", "1"):
            command = [args.compiler, f"-fplugin={plugin}",
                       f"{prefix}{key}={value}", "-c", str(source),
                       "-o", str(output / "rejected.o")]
            result = subprocess.run(command, capture_output=True, text=True, timeout=15)
            passed = result.returncode != 0 and f"unknown plugin argument '{key}'" in result.stderr
            runs.append({"option": f"{key}={value}", "passed": passed})
            if not passed:
                raise RuntimeError(f"obsolete option was not rejected: {key}={value}: {result.stderr}")
    stem = output / "analysis"
    command = [args.compiler, f"-fplugin={plugin}", f"{prefix}reportpath={stem}",
               f"{prefix}reportonly=1", "-c", str(source), "-o", str(output / "accepted.o")]
    result = subprocess.run(command, capture_output=True, text=True, timeout=15)
    if result.returncode:
        raise RuntimeError(f"current options failed: {result.stderr}")
    document = json.loads(stem.with_suffix(".json").read_text())
    if document["analysis"]["diagnostics"] != "report-only":
        raise RuntimeError("current options did not reach the report configuration")
    (output / "result.json").write_text(json.dumps({"removed_options": runs,
                                                 "current_options": "PASS"}, indent=2) + "\n")
    print(f"PASS: {len(runs)} obsolete option/value pairs rejected; current options accepted")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
