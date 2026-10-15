# Current analyzer comparison — 2026-10-15

[繁體中文](comparison-2026-10-15.md) · [English](comparison-2026-10-15.en.md) · [JSON](measurements/2026-10-15-summary.json)

The current binary passed the sampled speed gate. This page uses a fresh complete C plugin replay, validated cached native accuracy, and fresh serial timing; the four FFmpeg SDK reviews remain conditional.

## Accuracy: 1,234 C LTO groups

| Metric | Current plugin | GCC16 `-fanalyzer` |
| --- | ---: | ---: |
| Completed bad/good variants | 2,468 / 2,468 | 2,468 / 2,468 |
| Detected target-rule cases | 1122 / 1,234 | 974 / 1,234 |
| Case recall | 90.92% | 78.93% |
| Missed bad cases | 112 | 260 |
| Target-rule alerts on good variants | 0 | 48 |
| Exact oracle-site matches | 292 | 398 |
| Good variants with any lifetime alert (not all adjudicated false positives) | 123 | 182 |

Both analyzers receive the real testcase TUs plus io.c under LTO, without injected ownership contracts. SA001 reports allocations, while the oracle may mark the end of lifetime: exact-site matching and case detection measure different things. Target FP0 does not mean every warning is correct or every real project has zero false positives. Native reference results are revalidated and rescored; native full-corpus timing is not remeasured.

This whole-run result is one detection below the frozen Phase-17 TP1123. CWE416_Use_After_Free__malloc_free_int64_t_10 hit the unchanged 100 ms function CPU cap and remained inconclusive. Separate isolated links with both current and frozen plugins each detect the UAF; those diagnostic reruns do not replace the whole-run TP1122/FN112 headline. The cap was not raised.

| Class | Groups | Plugin detections | GCC detections |
| --- | ---: | ---: | ---: |
| CWE401 / leak | 868 | 788 | 666 |
| CWE415 / double free | 228 | 216 | 228 |
| CWE416 / use after free | 138 | 118 | 80 |

## Serial compiler cost: 20 paired rounds

| Mode | Median wall time | Median CPU time |
| --- | ---: | ---: |
| Plain GCC | 51.46 ms | 47.38 ms |
| Frozen Phase-17 plugin | 57.36 ms | 53.60 ms |
| Current default plugin | 54.93 ms | 50.08 ms |
| GCC16 -fanalyzer | 60.02 ms | 55.34 ms |

After two warmup rounds, 20 rounds use shuffled mode order. Median paired current/previous CPU ratio: 0.942; current/native: 0.928. These are compiler, JSON and process costs on one 201-line generated source, with only SA001 enabled for plugin modes and the usual native rule bundle. They are not isolated analyzer times or a general speed ranking.

## Real core build: FFmpeg 9.0

| Mode | Median wall time | Median CPU time | Compiled C units per run |
| --- | ---: | ---: | ---: |
| Plain build | 39.735 s | 38.884 s | 175 / 175 |
| Plugin + FFmpeg model + report-only | 41.265 s | 40.536 s | 175 / 175 |

### Individual clean-build rounds

| Round | Plain wall time | Plugin wall time | Paired wall increase |
| --- | ---: | ---: | ---: |
| 1 | 34.185 s | 39.429 s | +15.34% |
| 2 | 39.832 s | 42.830 s | +7.53% |
| 3 | 39.735 s | 41.265 s | +3.85% |

One warmup pair is excluded; three clean-build pairs alternate order and use one worker. Median paired wall increase is 7.53%, and paired CPU increase is 7.53%, passing the 10% sample gate. The median of per-round ratios differs from dividing the two separate medians. All runs compile 175 units with zero alerts; 275 unresolved records per plugin run are abstentions, not safety proofs. The configuration disables codecs/demuxers, programs, networking, assembly and avfilter. Configure and make clean time are excluded; compiler wrapper and JSON costs remain included. These are core libraries, not a full-feature executable or the historical files below.

## Historical FFmpeg defects: conditional reviews

| Historical defect | Default plugin / native leak alerts | SDK reviews before fix | SDK alerts after fix |
| --- | ---: | ---: | ---: |
| [ffmpeg-rtsp](https://github.com/FFmpeg/FFmpeg/commit/14f9e47314ab70a2c786cb040ca0ca2fc1a0e163) | 0 / 0 | 1 | 0 |
| [ffmpeg-hls](https://github.com/FFmpeg/FFmpeg/commit/9481b7d932e895b903c260c54c7b26adfe402e1f) | 0 / 0 | 1 | 0 |
| [ffmpeg-colorcorrect](https://github.com/FFmpeg/FFmpeg/commit/5f87a68cf70dafeab2fb89b42e41a4c29053b89b) | 0 / 0 | 2 | 0 |

All four SDK reviews have conditional confidence and unknown complete-path verdicts. They are not PROVEN and are excluded from the Juliet detection score. Original affected C files use compatibility headers, not complete historical checkouts. No automatic source repair is implemented. RTSP also has a native uninitialized-value warning in both versions; that is not credited as a leak detection.

## Validation and reproduction

GCC10/GCC16 each pass 150/150 regressions; Python checks pass 138/138. Full plugin replay wall time was 201.7 s with eight workers; native results were cached, so this is not a full-corpus speed comparison.

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

Set the compiler, retained previous ABI-matched plugin, FFmpeg audit report and validated native reference paths explicitly. Omit --reuse-reference to rebuild native/reference modes too. Use fresh output directories. The retained old .so and large local reports are not published. Aggregate measurements, hashes and the small source fixture are included in this repository.

Plugin SHA-256: `0ecf61d94c623c3e3e1876cea88bd3e46a11dbf39b2061d8d4206379589669cd`

Compiler SHA-256: `929f1fdb459e7b17915f6d2c7362187c9d1d6f54dc4d9a20ac2368b891baf9ac`

Fixture SHA-256: `fbceec82d99f2177f310c1057b59db1e6494c6ebddaeee002ad78caafdd0df70`
