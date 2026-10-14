#!/usr/bin/env python3
"""Precompute caller results for two explicit external-call assumptions.

The generated HTML switches between actual compiler runs. It never changes
the original source or credits assumption-based findings as confirmed ones.
This initial runner analyzes one C translation unit without LTO. Extra source
providers and binary ownership certificates are not synthesized.
"""
from __future__ import annotations

import argparse
import hashlib
import html
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def render(runs: list[dict], symbol: str, argument: int, binary_trace: Path | None) -> str:
    esc = lambda value: html.escape(str(value), quote=True)
    parts = ["<!doctype html><html lang=zh-Hant><meta charset=utf-8>",
        "<meta name=viewport content='width=device-width,initial-scale=1'>",
        "<title>未知呼叫：假設檢視</title><style>body{font:16px system-ui;background:#f3f5f9;color:#172033;margin:0}main{max-width:1150px;margin:30px auto;padding:20px}button,.card{border:1px solid #dce2eb;background:white;border-radius:10px;padding:14px;margin:6px}button{cursor:pointer}button[aria-pressed=true]{background:#234bc2;color:white}.notice{background:#fff2cf;padding:16px;border-radius:10px}table{width:100%;border-collapse:collapse}td,th{text-align:left;padding:12px;border-bottom:1px solid #dce2eb}code{overflow-wrap:anywhere}.counts{display:flex;flex-wrap:wrap}.card strong{display:block;font-size:28px}section[hidden]{display:none}a{color:#234bc2}</style><main>",
        "<h1>未知呼叫：假設檢視</h1>",
        f"<p><code>{esc(symbol)}</code> 的第 {argument + 1} 個參數</p>",
        "<nav aria-label='分析模式'>"]
    labels = {"unknown": "原本結果", "borrow": "假設不釋放", "release": "假設會釋放"}
    for run in runs:
        key = run["mode"]
        parts.append(f"<button type=button data-mode='{key}' aria-pressed='{'true' if key == 'unknown' else 'false'}'>{labels[key]}</button>")
    if binary_trace:
        parts.append(f"<a href='{esc(binary_trace)}'>二次追蹤證據</a>")
    parts.append("</nav>")
    unresolved = [effect for effect in runs[0]["report"].get("callee_effects", [])
                  if effect.get("needs_review") and effect.get("symbol") == symbol
                  and effect.get("argument_index") == argument]
    parts.append("<h2>需確認的呼叫</h2><table><tr><th>呼叫者 → 函式</th><th>位置</th><th>目前資訊</th></tr>")
    for effect in unresolved:
        location = effect["location"]
        parts.append(f"<tr><td>{esc(effect['caller'])} → {esc(symbol)}</td><td title='{esc(location['file'])}'>{esc(Path(location['file']).name)}:{location['line']}</td><td>{esc(effect['fates'])}</td></tr>")
    if not unresolved:
        parts.append("<tr><td colspan=3>此清單沒有該符號／參數的未知效果；假設可能未套用，已知來源效果會保留。</td></tr>")
    parts.append("</table>")
    for run in runs:
        key, report = run["mode"], run["report"]
        parts.append(f"<section data-result='{key}' {'hidden' if key != 'unknown' else ''}>")
        if key != "unknown":
            meaning = "只借用，不釋放、不保留指標" if key == "borrow" else "正常返回前釋放指定參數指向的記憶體"
            parts.append(f"<p class=notice>假設結果：{esc(symbol)} {meaning}。這個效果尚未證實，以下結果不計入原本確認的檢出數。</p>")
        else:
            parts.append("<p>原本分析維持未知呼叫政策。未知並不等於安全。</p>")
        parts.append("<div class=counts>")
        for rule, label in (("SA001", "記憶體洩漏"), ("SA002", "重複釋放"), ("SA003", "釋放後使用")):
            parts.append(f"<div class=card>{label}<strong>{report['summary'].get(rule, 0)}</strong></div>")
        parts.append("</div><table><tr><th>問題</th><th>位置</th><th>證據</th></tr>")
        for finding in report["findings"]:
            location = finding["location"]
            points = "<br>".join(esc(f"{point['role']}: {Path(point['file']).name}:{point['line']}") for point in finding.get("trace", []))
            parts.append(f"<tr><td>{esc(finding['rule_id'])}<br>{esc(finding['message'])}</td><td title='{esc(location['file'])}'>{esc(Path(location['file']).name)}:{location['line']}</td><td>{points}</td></tr>")
        if not report["findings"]:
            parts.append("<tr><td colspan=3>此模式未產生告警；不代表完整證明安全。</td></tr>")
        parts.append(f"</table><p>分析未確認的候選：{report['summary'].get('inconclusive_total', 0)}。<a href='{key}/analysis.html'>完整報告</a></p></section>")
    parts.append("<script>document.querySelectorAll('[data-mode]').forEach(button=>button.addEventListener('click',()=>{document.querySelectorAll('[data-mode]').forEach(b=>b.setAttribute('aria-pressed',String(b===button)));document.querySelectorAll('[data-result]').forEach(s=>s.hidden=s.dataset.result!==button.dataset.mode);}));</script></main></html>")
    return "\n".join(parts)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", required=True)
    parser.add_argument("--plugin", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--symbol", required=True)
    parser.add_argument("--argument", type=int, default=0, help="zero-based lowered argument index")
    parser.add_argument("--compile-arg", action="append", default=[])
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--binary-trace-html", type=Path)
    args = parser.parse_args()
    if not re.fullmatch(r"[A-Za-z_$][A-Za-z0-9_.$]*", args.symbol) or not 0 <= args.argument <= 63:
        parser.error("invalid exact symbol/argument")
    # Reuse the project's allowlist: source/output/plugin/pass overrides and
    # response files must not turn a mode comparison into different programs.
    from compare_lto_project import validate_extra_args
    flags = validate_extra_args(args.compile_arg, "compile")
    cc = shutil.which(args.cc)
    if not cc:
        parser.error("compiler not found")
    source, plugin = args.source.resolve(), args.plugin.resolve()
    hashes = {str(path): digest(path) for path in (source, plugin, Path(cc), Path(__file__))}
    args.output = args.output.resolve()
    args.output.mkdir(parents=True, exist_ok=False)
    runs = []
    for mode in ("unknown", "borrow", "release"):
        directory = args.output / mode
        directory.mkdir()
        options = {"analysispolicy": "proof-only", "debugoutput": "0", "legacyengine": "0",
                   "memoryleakcheck": "1", "doublefreecheck": "1", "useafterfree": "1", "threadmod": "0",
                   "smt": "1", "report": "1", "reporthtml": "1", "reportpath": str(directory / "analysis"),
                   "calleemodel": mode, "calleesymbol": args.symbol, "calleearg": str(args.argument)}
        command = [cc, "-O1", "-fipa-pta", "-fno-inline", "-fno-tree-dce", "-fdiagnostics-color=never", *flags,
                   f"-fplugin={plugin}", *[f"-fplugin-arg-{plugin.stem}-{key}={value}" for key, value in options.items()],
                   "-c", str(source), "-o", str(directory / "source.o")]
        start = time.monotonic()
        with (directory / "stdout.txt").open("w") as out, (directory / "stderr.txt").open("w") as err:
            process = subprocess.run(command, stdout=out, stderr=err, timeout=30)
        if process.returncode:
            raise RuntimeError(f"{mode} compilation failed; see {directory / 'stderr.txt'}")
        report = json.loads((directory / "analysis.json").read_text())
        expected_status = "PROVEN" if mode == "unknown" else "ASSUMPTION_BASED"
        if report["analysis"].get("callee_model") != mode or any(finding.get("status") != expected_status for finding in report["findings"]):
            raise ValueError("mode/status mismatch in compiler output")
        runs.append(dict(mode=mode, command=command, duration_ms=round((time.monotonic() - start) * 1000, 3), report=report))
    if any(digest(Path(path)) != sha for path, sha in hashes.items()):
        raise ValueError("input changed during assumption comparison")
    binary_html = Path(os.path.relpath(args.binary_trace_html.resolve(), args.output)) if args.binary_trace_html else None
    (args.output / "report.html").write_text(render(runs, args.symbol, args.argument, binary_html))
    (args.output / "report.json").write_text(json.dumps(dict(schema_version=1,
        mode="callee-assumption-comparison", symbol=args.symbol, argument=args.argument,
        inputs=hashes, limits="One C TU; headers/environment unsnapshotted. Assumptions are not ownership proofs.",
        runs=runs), indent=2) + "\n")
    print(args.output / "report.html")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
