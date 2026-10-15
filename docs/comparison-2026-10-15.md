# 最新靜態分析比較 — 2026-10-15

[繁體中文](comparison-2026-10-15.md) · [English](comparison-2026-10-15.en.md) · [JSON](measurements/2026-10-15-summary.json)

目前版本已通過本次樣本的速度檢查。下表使用最新完整 C 插件重跑、已核對的 GCC 歷史準確度，以及新的單程序速度測量；四筆 FFmpeg 模型告警仍待確認。

## 檢出效果：1,234 組 C LTO 案例

| 指標 | 目前插件 | GCC16 `-fanalyzer` |
| --- | ---: | ---: |
| 完成分析的有問題／安全變體 | 2,468 / 2,468 | 2,468 / 2,468 |
| 命中目標規則的有問題案例 | 1122 / 1,234 | 974 / 1,234 |
| 案例檢出率 | 90.92% | 78.93% |
| 漏抓案例 | 112 | 260 |
| 安全變體的目標規則誤報 | 0 | 48 |
| 與 oracle 行號完全對齊的命中 | 292 | 398 |
| 安全變體出現任一生命週期告警（未全部定性為誤報） | 123 | 182 |

兩邊使用同一組原始 testcase 與 io.c，在 LTO 模式分析，沒有注入所有權契約。SA001 主要標記配置點，oracle 可能標記生命週期結束點，所以案例命中與行號完全對齊分開列出。目標規則 FP0 不表示每一筆告警都正確，也不表示所有真實專案都零誤報。GCC 參考結果經重新核對與計分，沒有重新測量 GCC 全案例耗時。

本次完整重跑比凍結 Phase-17 的 TP1123 少一例：CWE416_Use_After_Free__malloc_free_int64_t_10 觸發原有 100 ms 函式 CPU 上限而保留未確認。另行單例診斷中，目前版與舊版各抓到一筆 UAF；這些診斷結果沒有回填整體成績，仍記 TP1122／FN112，也沒有調高上限。

| 類別 | 案例數 | 插件命中 | GCC 命中 |
| --- | ---: | ---: | ---: |
| CWE401 / leak | 868 | 788 | 666 |
| CWE415 / double free | 228 | 216 | 228 |
| CWE416 / use after free | 138 | 118 | 80 |

## 單檔編譯成本：20 回合單程序配對測量

| 模式 | 實際耗時中位數 | CPU 耗時中位數 |
| --- | ---: | ---: |
| 原編譯 GCC | 51.46 ms | 47.38 ms |
| 舊 Phase-17 插件 | 57.36 ms | 53.60 ms |
| 目前預設插件 | 54.93 ms | 50.08 ms |
| GCC16 -fanalyzer | 60.02 ms | 55.34 ms |

排除兩個暖機回合，正式 20 回合打亂各模式順序。每回合配對後的 CPU 比值中位數：目前／舊版 0.942，目前／GCC 0.928。這是單一 201 行產生式程式的編譯、JSON 與程序成本；插件只開 SA001，GCC 使用原有規則組，不能當成純分析耗時或通用速度排名。

## 真實核心建置：FFmpeg 9.0

| 模式 | 實際耗時中位數 | CPU 耗時中位數 | 每輪 C 單元 |
| --- | ---: | ---: | ---: |
| 原編譯 | 39.735 s | 38.884 s | 175 / 175 |
| 插件＋FFmpeg 模型＋report-only | 41.265 s | 40.536 s | 175 / 175 |

### 每輪原始測量

| 正式輪次 | 原編譯實際耗時 | 插件實際耗時 | 配對耗時增加 |
| --- | ---: | ---: | ---: |
| 1 | 34.185 s | 39.429 s | +15.34% |
| 2 | 39.832 s | 42.830 s | +7.53% |
| 3 | 39.735 s | 41.265 s | +3.85% |

排除一組暖機，以單一工作程序交錯執行三組乾淨建置。配對後的耗時增加中位數為 7.53%，CPU 增加中位數為 7.53%，通過本次 10% 樣本門檻。逐輪比值的中位數與兩個耗時中位數相除是不同統計量。每輪 175 個單元均編譯成功、零告警；插件每輪另有 275 筆未確認，不是安全證明。配置停用 codec/demuxer、程式、網路、組合語言及 avfilter；不計 configure 與 make clean，保留編譯包裝器和 JSON 成本。這是核心函式庫，不是完整功能程式，也不包含下方歷史受影響檔案。

## FFmpeg 歷史問題：模型待確認告警

| 歷史問題 | 預設插件／GCC 洩漏告警 | 模型：修補前 | 模型：修補後 |
| --- | ---: | ---: | ---: |
| [ffmpeg-rtsp](https://github.com/FFmpeg/FFmpeg/commit/14f9e47314ab70a2c786cb040ca0ca2fc1a0e163) | 0 / 0 | 1 | 0 |
| [ffmpeg-hls](https://github.com/FFmpeg/FFmpeg/commit/9481b7d932e895b903c260c54c7b26adfe402e1f) | 0 / 0 | 1 | 0 |
| [ffmpeg-colorcorrect](https://github.com/FFmpeg/FFmpeg/commit/5f87a68cf70dafeab2fb89b42e41a4c29053b89b) | 0 / 0 | 2 | 0 |

四筆均為 MODEL_REVIEW、條件式信心，完整路徑判定仍是 unknown，沒有算成 PROVEN 或加入 Juliet 檢出成績。原始受影響 C 檔使用相容標頭，不是完整歷史 checkout。自動修補尚未實作。GCC 在 RTSP 兩版另有未初始化值告警，沒有當作洩漏命中。

## 驗證與重現

GCC10／GCC16 各通過 150／150 回歸案例，Python 通過 138／138 檢查。本次完整插件重跑總耗時為 201.7 s（8 個工作程序）；GCC 使用已驗證快取，因此這個總耗時不能作全案例速度比較。

```sh
python3 tools/benchmark_ownership_paths.py \
  --compiler "$GCC16_CC" --source tests/fixtures/ownership_speed_probe.c \
  --plugin Compiler/newplugin.so --baseline-plugin "$PREVIOUS_PLUGIN" \
  --output /tmp/analyzer-serial-speed --rounds 20
python3 tools/benchmark_ffmpeg_core.py \
  --audit-report "$FFMPEG_AUDIT_REPORT" --plugin Compiler/newplugin.so \
  --output /tmp/analyzer-core-speed --rounds 3 --warmups 1 --jobs 1
python3 tests/compare_juliet_project.py \
  --cc "$GCC16_CC" --plugin Compiler/newplugin.so \
  --output /tmp/analyzer-full-c --jobs 8 --timeout 30 \
  --reuse-reference "$VALIDATED_NATIVE_COMPARISON"
```

請指定編譯器、保留的舊版 ABI 相符插件、FFmpeg 編譯審查報告與已驗證 GCC 比較檔。省略 --reuse-reference 可連同原編譯與 GCC 分析模式重新建置。輸出目錄請用新路徑。舊 .so 和大型本機報告沒有上傳；精簡測量資料、雜湊與小型來源 fixture 已納入儲存庫。

Plugin SHA-256: `0ecf61d94c623c3e3e1876cea88bd3e46a11dbf39b2061d8d4206379589669cd`

Compiler SHA-256: `929f1fdb459e7b17915f6d2c7362187c9d1d6f54dc4d9a20ac2368b891baf9ac`

Fixture SHA-256: `fbceec82d99f2177f310c1057b59db1e6494c6ebddaeee002ad78caafdd0df70`
