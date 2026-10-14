# Verified progress — 2026-10-15

Development branch: `optimize/recall-phase1`. This is a development checkpoint,
with the model-review and automatic-repair limits described below.
The complete Juliet measurement below uses the frozen Phase-17 snapshot:
`ecc415031c4e968654e4418314831f99b5bf226c0ab06287323253ba436a2739`.
The working GCC16 plugin now includes optional report-only diagnostics,
structural call-graph export, and an opt-in FFmpeg resource profile. Its
Phase-20 fingerprint is
`0ecf61d94c623c3e3e1876cea88bd3e46a11dbf39b2061d8d4206379589669cd`.
Do not attribute the frozen full-corpus measurement to this newer binary.
The GCC10 build is separate; it never replaces the GCC16 artifact.

The latest work targets build integration and upstream-confirmed leaks;
see [real-project validation](realworld-validation-2026-10-15.md). FFmpeg's
175-TU core-library build compiles with the final Phase-20 plugin; two complete
SQLite amalgamations compiled with the earlier Phase-19 plugin. In the matched
historical affected-file experiment, the default mode and retained GCC16
`-fanalyzer` runs miss all three leaks. The opt-in FFmpeg profile now localizes
RTSP `fds` (one review), HLS `options` (one), and colorcorrect's two histogram
fields (two). All three fixed files have zero alerts. These four findings are
explicitly `MODEL_REVIEW`, with conditional confidence and `path_verdict=unknown`,
not full-path proofs. This does not establish general real-world precision.

The [latest bilingual HTML](../artifacts/phase20-resource-protocol/overview.html)
shows locations, source excerpts, actual upstream patches, and the limits of
the experiment. New SDK classification, output-resource checks, and registered
callback checks are separate source modules within one `.so`; see the
[module boundaries](architecture.md#optional-library-resource-protocols).

## Complete C project measurement

All 1,234 C groups / 2,468 bad-good project variants completed. Both tools
receive the real testcase sources plus `testcasesupport/io.c` under LTO.
No runner-injected ownership contracts or unknown-call assumptions are used.
Native results are validated cached results; plugin objects are rebuilt.
This is an LTO project measurement, separate from historical isolated-TU runs.

| Metric | Plugin | GCC16 `-fanalyzer` |
| --- | ---: | ---: |
| Target-rule case detections | 1,123 / 1,234 (91.0%) | 974 / 1,234 (78.9%) |
| Target-rule false positives | 0 | 48 |
| Missed bad groups | 111 | 260 |
| Exact manifest-site detections | 293 | 398 |
| Good groups with any lifetime alert | 123 | 182 |

There are 919 shared detections, 204 plugin-only detections, 55 GCC-only
detections, and 56 groups missed by both. The plugin leads in case counts;
it has not covered every GCC detection or every manifest location. SA001's
primary location is the allocation, which often differs from the oracle's
end-of-lifetime site. Keep that location metric separate from case detection.
Cross-rule good-group alerts are not silently counted as target FP, and
target FP0 does not establish universal zero false positives.

| Defect class | Plugin detections | Remaining |
| --- | ---: | ---: |
| CWE401 leak | 788 / 868 | 80 |
| CWE415 double free | 216 / 228 | 12 |
| CWE416 use after free | 119 / 138 | 19 |

The [complete comparison](../artifacts/phase17-conditioned-borrow/final-full-c/comparison.html)
contains source and finding evidence. The [remaining-case report](../artifacts/phase17-conditioned-borrow/triage/triage.html)
expands each blocker to its actual cases. Its largest bucket is 40 global
publication cases, followed by 20 unsupported leak-path conditions. A global
store is an escape under the current function-local leak obligation, so
flagging every still-reachable global would change that policy. The remaining
work is detailed in [recall priorities](recall-cohort-priorities.md).

Phase-16 measured 1,122 detections. Its one additional miss was a function
CPU-budget abstention; that case completes in this replay. The difference
does not demonstrate a new Juliet semantic recovery from conditional guards.

## Implemented and independently checked

- Bounded narrow/wide standard-library effects, local pointer confinement,
  contained-pointer access, and fresh-return/null-realloc specialization.
- Visible source bodies and aliases take precedence over library models.
  A discovered source-defined `wcscpy` alias that releases its argument is
  preserved as a double-free regression, rather than overlaid with borrow.
- Conditional source deallocator guards compose with caller constraints.
  A `live|freed` summary alone cannot select a leak witness. Exact guard
  contradictions retain no finding; private arbitrary inputs, unknown
  extra effects, loops, unsupported forms, and bounds remain inconclusive.
- The report separates findings and unresolved ownership. Optional buttons
  recompute unknown/assumed-borrow/assumed-release results. Binary inspection
  is a bounded post-link evidence prototype with hash-based caching;
  it does not establish binary ownership certificates or runtime binding.

Standard-library results assume the checked ABI and valid API entry
conditions. The ordinary-stdout model excludes hidden runtime interposition;
observed custom callbacks/formatters, stdout mutation, or assembly disable
it. JSON records the profile and its scope. These library assumptions are
distinct from source-body proofs and from manually selected callee assumptions.

| Validation | Result |
| --- | --- |
| GCC16 reviewed regressions (Phase 20) | 150 / 150 |
| GCC10 reviewed regressions (Phase 20) | 150 / 150 |
| Python harness/report/oracle checks (Phase 20) | 138 / 138 |
| Generated diagnostic set, seed 20261027 | 207 / 207 leaks; 0 / 49 safe-case alarms |
| Fresh generated set after implementation, seed 20261031 | 419 / 419 leaks; 0 / 93 safe-case alarms |

Before the conditional-guard refinement, the diagnostic set detected
140 / 207 leaks. The second seed was checked after implementation. Both
oracles exhaust the five Boolean input classes independently of the analyzer;
the generated programs are compiled, not executed. This narrow synthetic
success does not imply complete C/C++ coverage or real-world precision.

## Speed and bounded search

The complete eight-worker plugin replay took 172.5 seconds. Only plugin
compilation/linking was timed; cached native results cannot support a speed
ranking. On one retained synthetic source, ten shuffled serial rounds give
median wall/CPU times of 48.49 / 46.15 ms for the candidate and
50.35 / 48.05 ms for GCC. The median paired CPU ratio is 0.995: close,
not evidence of a general speed advantage. Plugin runs enable SA001;
native GCC enables its usual rule bundle. These are compiler costs.

Path-scaling and budget checks pass. The 30-diamond regression represents
over a billion theoretical complete paths but uses 33/34 symbolic states
and two solver checks after exact abstraction. The query-budget test admits
256 queries and leaves the next 64 UNKNOWN, without findings.
Solver checks remain capped at 25 ms, queries at 100 ms / 4,096 symbolic
states / 256 constraints, and function analysis at 100 ms CPU. The new
conditional guards count within the existing constraint budget. Unsupported
obfuscated paths may lose recall; they cannot remove these limits.

## Reproduction and artifacts

```sh
make plugin PLUGIN_CXX=/root/toolchains/gcc-16.2/bin/g++
python3 tests/run.py --compiler /root/toolchains/gcc-16.2/bin/gcc \
  --cxx-compiler /root/toolchains/gcc-16.2/bin/g++ \
  --plugin Compiler/newplugin.so --manifest tests/manifest.json \
  --output /tmp/analyzer-regressions --jobs 1
python3 tools/fuzz_ownership_paths.py \
  --compiler /root/toolchains/gcc-16.2/bin/gcc \
  --plugin Compiler/newplugin.so --seed 20261031 --cases 512 \
  --output /tmp/analyzer-generated
python3 tests/compare_juliet_project.py \
  --cc /root/toolchains/gcc-16.2/bin/gcc --plugin Compiler/newplugin.so \
  --output /tmp/analyzer-full-c --jobs 8 --timeout 30 \
  --reuse-reference artifacts/phase17-conditioned-borrow/final-full-c/comparison.json
```

Use fresh output directories. Full-corpus replay fingerprints the compiler,
plugin, harness, oracle, sources, and support file; system headers, compiler
subprograms, and runtime environment are not fully snapshotted.

Artifacts under `artifacts/phase17-conditioned-borrow/`:
`final-snapshot/inputs.json`, `unit-gcc16-verified/report.json`,
`unit-gcc10-verified/report.json`, `python-tests-final.txt`,
`holdout-20261027-final/report.json`, `holdout-20261031/report.json`,
`serial-speed/report.json`, and `budget-checks-final.log`.
