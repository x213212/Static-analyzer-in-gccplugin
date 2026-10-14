# Real-project build and leak validation — 2026-10-15

The acceptance criteria are compatible compilation, bounded analysis cost,
and low false-positive burden while retaining detection of confirmed leaks.
No precision score is inferred from zero warnings or from a project's name.
This is a development checkpoint, not a completed release.

## Latest: optional FFmpeg resource profile (Phase 20)

Final GCC16 plugin SHA-256:
`0ecf61d94c623c3e3e1876cea88bd3e46a11dbf39b2061d8d4206379589669cd`.
The [bilingual HTML](../artifacts/phase20-resource-protocol/overview.html)
shows the latest evidence with expandable source excerpts and upstream patches.

`-fplugin-arg-newplugin-librarymodel=ffmpeg` enables SDK models; the default is
`none`. SDK call classification, local output-resource checks, and registered
filter lifecycle checks are separate source modules compiled into one `.so`.
See [architecture](architecture.md#optional-library-resource-protocols).

| Actual unchanged affected-file pair | Default, before fix | SDK reviews, before fix | SDK alerts, fixed file |
| --- | ---: | ---: | ---: |
| RTSP / fds | 0 | 1 | 0 |
| HLS / options | 0 | 1 | 0 |
| colorcorrect / uhistogram, vhistogram | 0 | 2 | 0 |

All four SDK findings are `MODEL_REVIEW`, confidence `conditional`, with
`path_verdict=unknown`. They identify the resource missed by each actual
upstream fix, but do not prove the complete execution path. They do not add
four `PROVEN` findings, establish general precision, or update the frozen
Juliet score. The final default-mode replay still misses these three defects.
The GCC16 column in the HTML retains the earlier matched `-fanalyzer` runs.
No analyzer rule matches these filenames, bug IDs, line numbers, private
resource field names, or registration names. Exact SDK function signatures
and framework field relationships are modeled instead. No testcase source
annotations, replacement allocator bodies, or fake providers are injected.

Final checks: GCC16 and GCC10 each pass 150/150 regressions; Python passes
138 tests. The controls include repaired paths, guarded release, dictionary
deletion, unknown consumers, ownership return and observable global
publication, inline assembly, unrelated private field names, actual source
body overrides, wrong API prototypes, and opaque lifecycle cleanup. Publication
uses an observable global so GCC retains the store before this late IPA pass.
Earlier probes, including an unused-static-global test, are retained but are
not final validation evidence.

The final FFmpeg core rebuild compiles 175 C units in both modes with zero
alerts. Baseline/plugin make wall times are 7.686 / 7.844 seconds, a single
fixed-order, four-worker observation. The configuration excludes RTSP, HLS,
and avfilter, so historical affected-file checks above remain a separate
experiment. The core build retains 275 unresolved records, not safety proofs.
Previous SQLite / extracted-project results below belong to Phase 19.

Four model / `-Werror` checks pass: warning mode fails while retaining two
review records; report-only succeeds with the same two records; a real GCC
`#warning` still fails both baseline and report-only builds. This does not
guarantee every project will compile or conceal ABI errors and crashes.

Reproduce the historical experiment with a fresh output directory:

```sh
python3 tools/compile_known_leak_pairs.py \
  --manifest tests/cohorts/realworld_memory_leaks.json \
  --source-root artifacts/phase19-realworld-precision/known-leaks \
  --ffmpeg-source artifacts/phase19-realworld-precision/sources/ffmpeg-9.0 \
  --ffmpeg-build-dir artifacts/phase19-realworld-precision/known-leaks/ffmpeg-config \
  --compiler /root/toolchains/gcc-16.2/bin/gcc --plugin Compiler/newplugin.so \
  --library-model ffmpeg --output /tmp/ffmpeg-sdk-pairs
```

Final evidence under `artifacts/phase20-resource-protocol/`: `pairs-final/`,
`pairs-default-final/`, `ffmpeg-core-final/`, `unit-gcc16-verified/`,
`unit-gcc10-verified/`, `python-tests-final.log`, `werror-model/report.json`,
and `verification.json`. Plugin snapshots and fingerprints bind these checks
to the same final GCC16 binary. Intermediate probes are not substituted for
these final results. The optional profile retains the existing solver/work
limits and adds finite local-resource/lifecycle scan caps.

## Prior build integration (Phase 19)

GCC16 candidate SHA-256:
`485914ba0a1ee0146019ae60f2cc667635c6ae5803ee5f7699dee372f7b54069`.

`reportonly=1` keeps findings and traces in JSON/HTML without emitting plugin
compiler diagnostics. A known leak still produces one SA001 report finding
with `-Werror`, while compilation succeeds. A genuine GCC `#warning` still
fails with `-Werror` in this mode. It does not catch compiler crashes or
suppress source errors; matching GCC/plugin ABI and runtime dependencies
are still required. The default remains normal warning diagnostics.

| Actual compilation | Baseline | Plugin | Scope |
| --- | --- | --- | --- |
| SQLite 3.35.4 | OK | OK | Complete sqlite3.c, object compilation |
| SQLite 3.51.2 | OK | OK | Complete sqlite3.c, object compilation |
| FFmpeg 9.0 | OK | OK | 175 C objects, libavutil/libavcodec/libavformat archives |
| Historical reduced projects | 100 / 100 OK | 100 / 100 OK | Binutils, Git, OpenSSH, OpenSSL, tmux |

FFmpeg uses its native configure/make build, with codecs/demuxers, programs,
assembly and external autodetection disabled. This is a core-library build,
not a full-feature FFmpeg executable or a whole-program LTO measurement.
Only copies of cached sources were used; recorded input sources stayed
unchanged. SQLite's cached amalgamations are identified by their retained
hashes rather than assumed byte-identical to official release archives.

In the first retained FFmpeg build, baseline/plugin wall times were
7.964 / 8.113 seconds; in the report-only replay, 8.401 / 8.599 seconds.
These are single four-worker build observations including compiler costs,
with fixed execution order and possible host contention. They are not
isolated analyzer times, repeated medians, or a general speed ranking.

## Upstream-confirmed leaks: original default-mode baseline

The catalog is [realworld_memory_leaks.json](../tests/cohorts/realworld_memory_leaks.json).
It records exact fix and parent commits, affected files, and required analysis.
The retained good FFmpeg files match their upstream Git blob hashes; the
bad-to-good differences match the actual upstream patches.

| Confirmed defect | Upstream evidence | Experiment |
| --- | --- | --- |
| RTSP error path omits av_freep(&fds) | [14f9e47](https://github.com/FFmpeg/FFmpeg/commit/14f9e47314ab70a2c786cb040ca0ca2fc1a0e163) | Both source variants compile; plugin and GCC16 miss the leak |
| HLS filename-allocation failure omits av_dict_free(&options) | [9481b7d](https://github.com/FFmpeg/FFmpeg/commit/9481b7d932e895b903c260c54c7b26adfe402e1f) | Both source variants compile; plugin and GCC16 miss the leak |
| colorcorrect destructor omits two histogram releases | [5f87a68](https://github.com/FFmpeg/FFmpeg/commit/5f87a68cf70dafeab2fb89b42e41a4c29053b89b) | Both source variants compile with original header overlays; plugin and GCC16 miss the leak |
| curl HTTP/2 push header strings omitted during error cleanup | [CVE-2024-2398](https://curl.se/docs/CVE-2024-2398.html), [fix](https://github.com/curl/curl/commit/deca8039991886a559b67bcd6701db800a5cf764) | Confirmed reference and patch, not compiled yet |
| SQLite sqlite3_result_pointer leak | [3.20.1 release notes](https://www.sqlite.org/releaselog/3_20_1.html) | Confirmed reference; historical source pair not compiled yet |

The FFmpeg affected-file experiment uses unchanged historical C files plus
FFmpeg 9.0 configuration/headers and explicitly retained historical header
overlays. It is not a build of either complete historical checkout. No
allocator replacement, ownership annotations, or fabricated provider bodies
were injected. Out-of-file providers are unavailable to this object-only run.
GCC16 additionally reports an uninitialized value in both RTSP variants;
that warning is not credited as detection of the leak and is not automatically
classified as a false positive.

## Precision and next blockers

The FFmpeg core build has zero emitted findings, but 275 unresolved records:
271 allocator/release contract gaps, two conditional allocation summaries,
and two reallocation-status gaps. Each SQLite amalgamation has two unresolved
ownership-transfer records. These are abstentions, not proof of safety.

The historical 100 positive extracted cases give only 3 localized detections
for the plugin versus 26 for GCC16. There are no paired negative cases in
that corpus, so precision and false-positive rate are undefined. The complete
Juliet Phase-17 target FP0 measurement remains tied to its older frozen binary.

The Phase-20 SDK review module above addresses output-parameter resources and
explicit callback lifetimes under a selected contract. Fully proving those
contracts against loaded provider bodies, composing more framework callbacks,
and modeling container element cleanup remain future work. Missing bodies and
inconclusive conditions must stay unresolved; a function name alone cannot
prove ownership or release.
Compiler safety, detection recall, and warning presentation remain separate
acceptance checks.

## Reproduction and evidence

`tools/audit_realworld_precision.py` compiles supplied SQLite/FFmpeg sources
in isolated baseline/plugin builds, snapshots the plugin, retains commands
and logs, and checks recorded source hashes. `--report-only` enables the new
diagnostic mode. `tools/compile_known_leak_pairs.py` compiles the catalog's
affected files against supplied compatibility headers; its JSON explicitly
separates baseline failure, plugin failure, no detection, and manual review.

Artifacts are under `artifacts/phase19-realworld-precision/`: `build-before/`,
`build-reportonly/`, `extracted/`, `known-leaks/`, `paired-results/`,
`werror-checks/`, `unit-gcc16/`, `unit-gcc10/`, and `python-all.log`.
GCC10 and GCC16 each pass all 144 reviewed regressions; Python checks pass
137 tests. The report-only mode does not change solver or lifetime budgets.

The offline [Traditional Chinese overview](../artifacts/phase19-realworld-precision/overview.html)
shows all three final paired experiments and their matching GCC16 runs.
The per-case JSON includes the exact compile commands, status, timings,
and input hashes. All six historical file variants compile successfully in
`paired-results/`; earlier header compatibility probes are retained separately
and are not counted as plugin failures or analyzer detections.
