#!/usr/bin/env python3
"""Render retained SDK-review evidence without running analysis or patching sources."""
from __future__ import annotations

import argparse
import html
import json
import os
from pathlib import Path


def esc(value: object) -> str:
    return html.escape(str(value), quote=True)


def bi(zh: str, en: str) -> str:
    return f'<span data-lang="zh">{esc(zh)}</span><span data-lang="en">{esc(en)}</span>'


def read(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def link(path: Path, output: Path, label: str) -> str:
    return f'<a href="{esc(os.path.relpath(path.resolve(), output.parent.resolve()))}">{esc(label)}</a>'


def snippet(finding: dict) -> str:
    location = finding.get("location", {})
    source = Path(location.get("file", ""))
    line = location.get("line", 0)
    if not source.is_file() or not isinstance(line, int) or line < 1:
        return ""
    lines = source.read_text(encoding="utf-8", errors="replace").splitlines()
    selected = []
    for i in range(max(0, line - 5), min(len(lines), line + 5)):
        selected.append(f'<span class="{"focus" if i + 1 == line else "line"}">'
                        f'<span class="lineno">{i + 1:5}</span> {esc(lines[i])}</span>')
    return '<pre class="source">' + "\n".join(selected) + '</pre>'


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pairs", type=Path, required=True)
    parser.add_argument("--default-pairs", type=Path, required=True)
    parser.add_argument("--native", type=Path, required=True)
    parser.add_argument("--commits", type=Path, required=True)
    parser.add_argument("--core-build", type=Path, required=True)
    parser.add_argument("--verification", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    pairs, default = read(args.pairs), read(args.default_pairs)
    native, core = read(args.native), read(args.core_build)
    verification = read(args.verification)
    core_inputs = read(args.core_build.parent / "inputs.json")
    commits = {x["commit"]: x for x in read(args.commits)}
    if pairs["plugin_sha256"] != default["plugin_sha256"] or pairs["plugin_sha256"] != verification["plugin_sha256"]:
        raise ValueError("paired evidence and validation measured different plugins")
    if core_inputs["plugin_sha256"] != pairs["plugin_sha256"]:
        raise ValueError("core build measured a different plugin")
    if not pairs["input_files_unchanged"] or not default["input_files_unchanged"] or not core["source_files_unchanged"]:
        raise ValueError("recorded source inputs changed during measurement")
    native_cases = {(c["id"], c["variant"]): c for c in native["cases"]}
    default_cases = {c["id"]: c for c in default["cases"]}
    names = {
        "ffmpeg-rtsp": ("RTSP：錯誤返回漏釋放 fds", "RTSP: fds lost on an error return"),
        "ffmpeg-hls": ("HLS：配置失敗漏釋放 options", "HLS: options lost on allocation failure"),
        "ffmpeg-colorcorrect": ("colorcorrect：清理回呼漏釋放 histogram", "colorcorrect: histograms omitted from cleanup"),
    }
    rows, details = [], []
    total_reviews, total_unknown = 0, 0
    fixed_zero = 0
    for case in pairs["cases"]:
        ident = case["id"]
        before, after = (case["variants"][v] for v in ("bad", "good"))
        for variant in case["variants"].values():
            if any(m["status"] != "OK" for m in variant["modes"].values()):
                raise ValueError("cannot present an unsuccessful compilation as analyzed")
        findings = before["analysis"]["findings"]
        reviews = [f for f in findings if f.get("status") == "MODEL_REVIEW"]
        unknown = sum(f.get("path_verdict") == "unknown" for f in reviews)
        total_reviews += len(reviews)
        total_unknown += unknown
        fixed = after["analysis"]["summary"]["total"]
        fixed_zero += fixed == 0
        ordinary = default_cases[ident]["variants"]["bad"]["analysis"]["summary"]["total"]
        nc = native_cases[(ident, "bad")]
        nd = nc.get("diagnostics", {})
        native_leaks = nd.get("warning_options", {}).get("-Wanalyzer-malloc-leak", 0)
        native_cell = str(native_leaks) if nc["status"] == "OK" and nd.get("status") == "OK" else "unavailable"
        label = bi(*names.get(ident, (ident, ident)))
        rows.append(f'<tr><td><a href="#{esc(ident)}">{label}</a></td><td>{ordinary}</td>'
                    f'<td>{esc(native_cell)}</td><td class="amber">{len(reviews)} MODEL_REVIEW</td>'
                    f'<td class="green">{fixed}</td></tr>')
        cards = []
        for finding in findings:
            loc = finding["location"]
            trace = "".join('<li><code>' + esc(Path(p["file"]).name if p.get("file") else "?") + ':' +
                            esc(p.get("line", 0)) + '</code> · ' + esc(p.get("role", "")) + '</li>'
                            for p in finding.get("trace", []))
            cards.append('<article><p><strong>' + esc(finding.get("status", "")) + '</strong> · ' +
                         bi("路徑判定：", "Path verdict: ") + '<code>' +
                         esc(finding.get("path_verdict", "unspecified")) + '</code></p><p><code>' +
                         esc(Path(loc["file"]).name) + ':' + esc(loc["line"]) + '</code></p>' +
                         snippet(finding) + '<details><summary>' + bi("展開已記錄的追蹤與模型範圍", "Recorded trace and model scope") +
                         '</summary><ol>' + trace + '</ol><p class="small">' +
                         esc(finding.get("model_scope", "")) + '</p><pre>' +
                         esc(json.dumps(finding, ensure_ascii=False, indent=2)) + '</pre></details></article>')
        patch = "\n\n".join(f["patch"] for f in commits[case["fix"]]["files"] if f["path"] == case["file"])
        details.append(f'<details class="case" id="{esc(ident)}"><summary>{label} · {len(reviews)} MODEL_REVIEW</summary>'
                       '<p><code>' + esc(case["function"]) + '</code> · <a href="' + esc(case["reference"]) + '">' +
                       bi("上游確認與修補", "Upstream confirmation and fix") + '</a></p><div class="columns"><div>' +
                       "".join(cards) + '</div><div><h3>' + bi("修補位置：上游實際 patch", "Repair locations: actual upstream patch") +
                       '</h3><pre>' + esc(patch) + '</pre><p>' + bi(
                           "使用 av_freep 或 av_dict_free 等對應 API；此處展示歷史修補，沒有自動修改你的程式碼。",
                           "Use the corresponding release API, such as av_freep or av_dict_free. This historical patch is displayed for review; no source changes are applied.") +
                       '</p><p>' + bi("修補後完整檔案：", "Complete fixed file: ") + esc(fixed) +
                       bi(" 筆告警。這不是整個檔案已證明安全。", " alerts. This does not prove the entire file safe.") +
                       '</p></div></div></details>')

    project = core["projects"][0]
    baseline, modeled = (project["modes"][m] for m in ("baseline", "plugin"))
    units = modeled["summary"]["compiler_processes"]
    if any(m["build"]["status"] != "OK" for m in (baseline, modeled)):
        raise ValueError("core build did not complete")
    elapsed = [m["build"]["duration_ms"] / 1000 for m in (baseline, modeled)]
    validation = verification["regressions"]
    evidence = [link(path, args.output, label) for path, label in (
        (args.pairs, "FFmpeg model: JSON"), (args.default_pairs, "Default profile: JSON"),
        (args.native, "GCC16 -fanalyzer: JSON"), (args.core_build, "FFmpeg core build: JSON"),
        (args.verification, "Validation / SHA-256: JSON"))]
    page = '''<!doctype html><html lang="zh-Hant" data-language="zh"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>FFmpeg · 資源生命週期驗證</title>
<style>
*{box-sizing:border-box}body{margin:0;background:#11151c;color:#dce4ef;font:15px/1.7 system-ui,sans-serif}main{max-width:1240px;margin:auto;padding:30px 24px 70px}h1{font-size:30px;margin:0}h2{font-size:22px;margin-top:32px}h3{font-size:17px}p{color:#bac7d8}a{color:#86bfff}header{display:flex;justify-content:space-between;gap:20px;align-items:start}button{cursor:pointer;background:#223349;color:#dce4ef;border:1px solid #536984;padding:8px 12px;border-radius:7px}button[aria-pressed=true]{background:#255d99}.notice{border-left:4px solid #eab45d;padding:15px 20px;background:#29251e;margin:24px 0}.cards{display:grid;grid-template-columns:repeat(4,1fr);gap:14px}.card,.case,article{background:#19212d;border:1px solid #344153;border-radius:9px;padding:18px}.metric{font-size:28px;font-weight:700;color:#f0f5ff}.small{font-size:12px;overflow-wrap:anywhere}.amber{color:#f2bf70}.green{color:#99d7b6}.scroll{overflow-x:auto}table{width:100%;border-collapse:collapse}th,td{text-align:left;vertical-align:top;padding:13px;border-bottom:1px solid #344153}th{font-size:13px;color:#9eafc4}.case{margin:14px 0}summary{cursor:pointer;font-weight:650}details>p{margin-top:16px}.columns{display:grid;grid-template-columns:1.05fr 1fr;gap:18px;margin-top:16px}article{margin:0 0 14px;padding:14px}pre{background:#1e1e1e;color:#d4d4d4;overflow:auto;padding:15px;font:12px/1.7 ui-monospace,Consolas,monospace;white-space:pre-wrap;overflow-wrap:anywhere}.source{white-space:pre;overflow-wrap:normal}.focus{background:#532d2d;display:block}.line{display:block}.lineno{color:#8c8c8c}code{font:13px ui-monospace,Consolas,monospace}html[data-language=zh] [data-lang=en],html[data-language=en] [data-lang=zh]{display:none}footer{margin-top:32px;border-top:1px solid #344153;padding-top:14px}.evidence{display:flex;gap:16px;flex-wrap:wrap}@media(max-width:800px){main{padding:20px 14px}.columns{grid-template-columns:1fr}.cards{grid-template-columns:repeat(2,1fr)}header{display:block}header nav{margin-top:14px}h1{font-size:25px}}
</style></head><body><main><header><div><h1>@@TITLE@@</h1><p>@@INTRO@@</p></div><nav aria-label="Language"><button id="zh" aria-pressed="true">繁體中文</button> <button id="en" aria-pressed="false">English</button></nav></header>
<p class="notice">@@NOTICE@@</p><div class="cards">@@CARDS@@</div>
<h2>@@COMPARE_TITLE@@</h2><div class="scroll"><table><thead><tr>@@HEADINGS@@</tr></thead><tbody>@@ROWS@@</tbody></table></div>
<p>@@SCOPE@@</p><p class="small">@@NATIVE_SCOPE@@</p>
<h2>@@CASES_TITLE@@</h2><p>@@CASES_INTRO@@</p>@@DETAILS@@
<h2>@@BUILD_TITLE@@</h2><p>@@BUILD@@</p><p>@@BUILD_SCOPE@@</p>
<details class="case"><summary>@@MODULE_TITLE@@</summary><p>@@MODULE_INTRO@@</p><pre>library_model.cc       → SDK API signatures / ownership contracts
resource_protocol.cc   → output parameters / local error exits
framework_lifecycle.cc → registered config_props / uninit callbacks
feasibility.cc         → shared bounded CFG / Z3 queries
analysis_budget.cc     → shared CPU / work limits
report.cc              → JSON / HTML / compiler diagnostics</pre><p>@@MODULE_SCOPE@@</p></details>
<h2>@@CHECK_TITLE@@</h2><p>@@CHECKS@@</p><p>@@BOUNDS@@</p>
<footer><div class="evidence">@@EVIDENCE@@</div><p class="small">Plugin SHA-256: @@SHA@@</p></footer></main>
<script>function language(code){document.documentElement.dataset.language=code;document.documentElement.lang=code==='zh'?'zh-Hant':'en';for(const id of ['zh','en'])document.getElementById(id).setAttribute('aria-pressed',String(id===code));document.title=code==='zh'?'FFmpeg · 資源生命週期驗證':'FFmpeg · resource lifecycle validation'}for(const id of ['zh','en'])document.getElementById(id).addEventListener('click',()=>language(id));</script></body></html>'''
    cards = [(str(len(pairs["cases"])), "上游確認的歷史問題", "Upstream-confirmed historical defects"),
             (str(total_reviews), "模型待確認告警", "Conditional model reviews"),
             (f'{fixed_zero}/{len(pairs["cases"])}', "修補後零告警的檔案", "Fixed files with zero alerts"),
             (str(units), "FFmpeg 核心編譯單元", "FFmpeg core translation units")]
    content = {
        "TITLE": bi("FFmpeg：真實洩漏與資源生命週期", "FFmpeg: real leaks and resource lifecycles"),
        "INTRO": bi("相同 .so，選擇性開啟函式庫模型；原始碼保持不變。", "One .so with optional library models; original sources are unchanged."),
        "NOTICE": bi(f"新增 {total_reviews} 筆模型待確認告警，其中 {total_unknown} 筆完整路徑仍是 unknown。這些不是 PROVEN，也沒有併入既有 Juliet 成績或宣稱整體誤報率為零。", f"{total_reviews} conditional model reviews; {total_unknown} complete paths remain unknown. These are not PROVEN, are not added to frozen Juliet scores, and do not establish a zero false-positive rate."),
        "CARDS": "".join('<div class="card"><div class="metric">' + esc(n) + '</div>' + bi(z, e) + '</div>' for n, z, e in cards),
        "COMPARE_TITLE": bi("1. 開啟模型後改變了什麼？", "1. What changes with the model enabled?"),
        "HEADINGS": "".join('<th>' + bi(z, e) + '</th>' for z, e in (("歷史問題", "Historical defect"), ("預設模式：修補前", "Default: before fix"), ("GCC16：洩漏告警", "GCC16: leak warnings"), ("FFmpeg 模型：修補前", "FFmpeg model: before fix"), ("FFmpeg 模型：修補後", "FFmpeg model: after fix"))),
        "ROWS": "".join(rows),
        "SCOPE": bi("這是三組完整受影響 C 檔的修補前後物件編譯。使用 FFmpeg 9.0 相容標頭與保留的歷史標頭，沒有注入註解、假 provider 或替換配置函式；不是完整歷史專案建置。", "These are before/after object compilations of three complete affected C files, using FFmpeg 9.0 compatibility headers and retained historical headers. No annotations, fabricated providers or allocator substitutions are injected. This is not a full historical project build."),
        "NATIVE_SCOPE": bi("GCC16 欄沿用先前相同檔案與參數的 -fanalyzer 記錄。RTSP 兩版另有未初始化值告警，沒有當作洩漏命中，也沒有直接算成誤報。零告警不等於安全。", "The GCC16 column uses retained -fanalyzer runs on the same files and flags. Both RTSP versions also have an uninitialized-value warning, neither credited as a leak detection nor automatically classified as a false positive. Zero alerts do not establish safety."),
        "CASES_TITLE": bi("2. 展開：問題行號、追蹤、實際修補位置", "2. Expand: locations, traces and actual repair sites"),
        "CASES_INTRO": bi("左側是分析器實際列出的告警；右側是上游歷史修補。追蹤是部分證據，完整可行執行路徑仍待確認。", "The left pane contains actual analyzer reviews; the right pane shows upstream historical fixes. Recorded traces are partial evidence, with complete path feasibility still unresolved."),
        "DETAILS": "".join(details),
        "BUILD_TITLE": bi("3. 編譯相容性與時間", "3. Build integration and time"),
        "BUILD": bi(f"原編譯 {elapsed[0]:.3f} 秒；模型加 report-only {elapsed[1]:.3f} 秒。兩版均成功編譯 {units} 個 C 單元；模型版 {len(modeled['summary']['findings'])} 筆告警。", f"Baseline {elapsed[0]:.3f} s; model with report-only {elapsed[1]:.3f} s. Both builds successfully compiled {units} C translation units; the model build emitted {len(modeled['summary']['findings'])} alerts."),
        "BUILD_SCOPE": bi("單次、固定順序、4 個工作程序的核心函式庫建置時間，包含編譯成本；不是純分析耗時或通用速度排名。codec/demuxer、程式、網路、組合語言與 avfilter 均停用，因此沒有包含上方三個受影響檔案。未確認記錄不算已證明安全。", "These are single fixed-order core-library builds with four workers, including compilation; not isolated analysis cost or a general speed ranking. Codecs/demuxers, programs, networking, assembly and avfilter are disabled, so the three affected files above are excluded. Unresolved records are not safety proofs."),
        "MODULE_TITLE": bi("模組化設計與使用方式", "Module boundaries and usage"),
        "MODULE_INTRO": bi("新增模型與檢查器分成三個來源模組，最後編譯成同一個 ABI 相符的 newplugin.so。", "Three separate source modules implement models and checkers, compiled into the same ABI-matched newplugin.so."),
        "MODULE_SCOPE": bi("預設 librarymodel=none。以 -fplugin-arg-newplugin-librarymodel=ffmpeg 開啟模型；搭配 -fplugin-arg-newplugin-reportonly=1 可將告警留在報告，避免插件警告觸發 -Werror。新增 SDK 目前需要修改模組並重編譯，沒有外部動態模型載入介面。", "The default is librarymodel=none. Enable the profile with -fplugin-arg-newplugin-librarymodel=ffmpeg and optionally add -fplugin-arg-newplugin-reportonly=1 to retain reviews without plugin warnings triggering -Werror. Adding an SDK currently requires editing modules and rebuilding; there is no external dynamic model loader."),
        "CHECK_TITLE": bi("4. 回歸與限制", "4. Regressions and limits"),
        "CHECKS": bi(f"GCC16 {validation['gcc16']['passed']}/{validation['gcc16']['total']}、GCC10 {validation['gcc10']['passed']}/{validation['gcc10']['total']}、Python {verification['python_tests']} 項通過。-Werror 與 report-only 的四項模型整合對照通過；真正 GCC 警告仍會照規則失敗。", f"GCC16 {validation['gcc16']['passed']}/{validation['gcc16']['total']}, GCC10 {validation['gcc10']['passed']}/{validation['gcc10']['total']}, and {verification['python_tests']} Python checks pass. Four model / report-only / -Werror integration checks pass; genuine GCC warnings still fail under -Werror."),
        "BOUNDS": bi("新的本地資源檢查最多處理 512 個 CFG 狀態或待處理狀態，並沿用既有 CPU、工作量與 Z3 上限。到達上限保留未知；不是保證所有混淆路徑都能求解。", "The local resource checker caps processed and pending CFG states at 512 and shares existing CPU, work and Z3 limits. Exhaustion remains unresolved; arbitrary obfuscated paths are not guaranteed solvable."),
        "EVIDENCE": "".join(evidence), "SHA": esc(pairs["plugin_sha256"]),
    }
    for key, value in content.items():
        page = page.replace('@@' + key + '@@', value)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(page, encoding="utf-8")
    print(args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
