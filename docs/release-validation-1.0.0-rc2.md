# rc2 working validation — updated 2026-10-15

This records implementation evidence and historical input modes, not a
completed release or a general claim of superiority over native GCC. The
current project score exceeds GCC's case-detection count, while exact-site
detection and some individual cases still lag. Work continues on external
effects and broader accuracy/speed validation.

## Current verified build

Branch: `optimize/recall-phase1`. This remains a development checkpoint.
Current Phase-20 GCC16 DSO SHA-256:
`0ecf61d94c623c3e3e1876cea88bd3e46a11dbf39b2061d8d4206379589669cd`.
GCC10/16 each pass 150 regression cases; Python checks pass 138/138.
The opt-in FFmpeg profile yields four conditional `MODEL_REVIEW` findings
on three upstream-confirmed affected-file pairs; complete path verdicts
remain `unknown`. All three fixed files have zero alerts. Automatic repair
is not implemented. See [real-project validation](realworld-validation-2026-10-15.md).

Latest Phase-21 full C replay uses the current DSO: TP1122 / target FP0 /
FN112, with all 1,234 groups complete. Native cached accuracy remains TP974 /
FP48. One UAF case hits the unchanged 100 ms function CPU limit; diagnostic
isolated reruns are not substituted into the whole-run headline. The current
20-round serial probe and three paired FFmpeg clean builds pass the sample
speed checks; median FFmpeg paired wall/CPU overhead is 7.53% against a 10%
gate. See the [bilingual comparison](comparison-2026-10-15.md) and its
[aggregate data](measurements/2026-10-15-summary.json).

Historical complete project measurement: frozen Phase-17 (2026-10-15), TP1123 /
target FP0 against native TP974 / FP48 on the same 1,234 C LTO groups, using
GCC16 DSO SHA-256:
`ecc415031c4e968654e4418314831f99b5bf226c0ab06287323253ba436a2739`.
That frozen snapshot differs from the current binary. Keep its historical
TP1123 score separate from the latest current-binary TP1122 result.
Full report: `artifacts/phase17-conditioned-borrow/final-full-c/comparison.html`.
See [current progress](current-progress-2026-10-15.md) for the current
remaining-class partition, library model assumptions, independent validation,
exact-site limitations, and reproduction commands. Earlier measurements
below remain historical build evidence and must not be mixed across modes.
The subsequent body-access build is recorded below; the following section
preserves the earlier Phase-2 evidence and its exact build fingerprint.

### Phase-2 build

GCC16 plugin SHA-256:
`3ad02cb0c8eaafeb2fea70df18789d1a04c93c7c13fd7e39bcf3f4ca1c27d9a4`.
Built with the installed GCC16 headers/compiler; GCC itself was not rebuilt.
Z3 is directly linked. The DSO exports only `plugin_init` and
`plugin_is_GPL_compatible`.

- GCC16: 117/117 reviewed regression cases, C and C++.
  `artifacts/phase2-general-effects-unit-gcc16-verified/report.json`.
- GCC10: 117/117 cases with its ABI-specific plugin.
  `artifacts/phase2-general-effects-unit-gcc10-final/report.json`.
- Python harness/manifest tests: 66/66.
- Current GCC16 plugin restored after the GCC10 check; `git diff --check` passes.

Tests cover argument identity, C++ source/this mapping, access guards, returned
aliases, integer representation escapes, PHI returns and allocation-failure
edges. The old opaque-choice return fixture now expects UNKNOWN: an external
function might always select the ownership-transfer arm. A separate symbolic
input fixture preserves the positive leak assertion.

## Full C oracle measurements

Both runs use 1,234 official groups / 3,428 bad-good TU variants. Every variant
completed; reports fingerprint the compiler, DSO, runner, oracle and sources.
Measurements credit target-CWE detection at group level; exact-site results
and cross-rule alerts remain separately visible.

| Mode | TP | FP | TN | FN | Recall |
| --- | ---: | ---: | ---: | ---: | ---: |
| Raw plugin | 274 | 0 | 1,234 | 960 | 22.20% |
| Plugin + explicit library contracts | 585 | 0 | 1,234 | 649 | 47.41% |
| Native GCC16 `-fanalyzer` | 725 | 85 | 1,149 | 509 | 58.75% |

Raw report: `artifacts/phase2-final-raw/comparison.html` and `.json`.
Assisted report: `artifacts/phase2-final-assisted/comparison.html` and `.json`.
The latter is explicitly ineligible as the raw headline: only the plugin is
given the extra trusted declarations in `tests/contracts/juliet_support_effects.h`.
Header SHA-256:
`cbb6634f771f16f403f141087f32a50a599fd6e83b8d9e3075ad2fbd146047fe`.

| CWE | Raw plugin TP | Assisted plugin TP | Native GCC TP |
| --- | ---: | ---: | ---: |
| 401 memory leak | 124 | 408 | 515 |
| 415 double free | 96 | 96 | 156 |
| 416 UAF | 54 | 81 | 54 |

Any-lifetime good-variant alert counts are raw45, assisted78, native221.
These include incidental cross-CWE defects; target FP0 is not an assertion
that all diagnostics or arbitrary projects have zero false positives.

The frozen 462-group intervention recovered309, including89 missed by GCC.
The full assisted delta is311 because two additional CWE401 groups lie
outside that frozen opaque-call cohort: `CWE401_Memory_Leak__strdup_char_42`
and `CWE401_Memory_Leak__strdup_wchar_t_42`. No previously detected group was
lost. Full raw group classifications remain unchanged from
the prior rc2 baseline. rc1's earlier178 TP and rc2's274 TP are historical
snapshots; the newer contracts must not be folded into the raw improvement.

## Speed and unfinished validation

These two accuracy runs use eight workers. Their timing fields are exploratory
compiler-process measurements, not controlled serial performance results.
The assisted mode also pays extra header preprocessing cost. No zero-overhead
or faster-than-GCC claim follows from these runs. Repeated full serial runs
and current-build 4,980-source robustness checks remain pending.

The largest next automatic coverage improvement is provider information:
ownership/access plus external scalar/global/constant-return facts. See
`docs/recall-cohort-priorities.md` for the fixed cohort, exact remaining-cause
counts and reproduction command. Automatic provider summary import,
PHI-guard projection and shared eventful loop summaries are not yet complete.

## Phase-3 automatic body access — verified follow-up

GCC16 plugin SHA-256:
`350a08af0130d2220fd3f42c23fc2e245716346fd457923aafdc86b7d638282e`.
The reviewed manifest now has 119 cases. The GCC16 run is
`artifacts/phase3-body-access-unit-gcc16-final/report.json` (119/119).
The added body-access fixture proves six source-defined UAF calls, preserves
five explicitly inconclusive calls and keeps unrelated-argument/null-path
negatives quiet. Empty-object/address-only regressions also remain quiet.

Both full 1,234-group C modes were rerun with this identical DSO hash:

| Mode | TP | Target-CWE FP | Report |
| --- | ---: | ---: | --- |
| Isolated-TU raw | 274 | 0 | `artifacts/phase3-body-access-raw/comparison.html` |
| Explicit contracts | 585 | 0 | `artifacts/phase3-body-access-assisted/comparison.html` |

Thus 585 did not regress to 274: these are different input modes, not successive
scores. The body theorem is implemented and tested, but raw full-corpus recall
has not yet increased. Neither this theorem nor preliminary LTO provider probes
establish that the largest leak cohort has surpassed native GCC. Full same-input
project-mode oracle comparisons remain unfinished.

## Phase-4/5 complete same-input C project measurements

The subsequent project runs use the actual testcase members plus `io.c`,
`INCLUDEMAIN`, and the same LTO/optimization controls for both analyzers.
No runner-injected contracts are present. All1,234 groups completed in each
run; these are not isolated-TU raw scores.

| Build | Plugin TP | Target FP | Native TP | Native target FP |
| --- | ---: | ---: | ---: | ---: |
| Phase-3 body access | 641 | 0 | 974 | 48 |
| Phase-4 live-string preconditions | 649 | 0 | 974 | 48 |
| Phase-5 v2 confinement + complete release barriers + shared loop certificate | 685 | 0 | 974 | 48 |

Reports are `artifacts/phase3-project-full-c/comparison.json`,
`artifacts/phase4-live-precondition-full-c/comparison.json`, and
`artifacts/phase5-core-v2/full-c/comparison.json` (HTML alongside each).
Phase-3 fingerprint:
`c82ac44be0335dc684e27c194e7ba67a7a7cef9fb2872323a9db2b706c34aede`.
Phase-4 fingerprint:
`b9f13f617f1809d9f2f9cc229d0a21413133e3643417c46deec4bf7ceab134cf`.
Phase-5 v2 fingerprint:
`e915f7ac7177b075395ab78b9877e0fbf5aaf711c96a1a76c96e27afc8b0b991`.
The v2 improvement over Phase-4 is24 container/aggregate leak groups and12
eventful-loop leak groups. It is not a claim that all48 container candidates
or all24 return-summary candidates have been recovered.

The first shared-loop experiment is retained as rejected evidence in
`artifacts/phase5-one-trip-project-c/comparison.json`: 12 new target FPs
exposed missing possible-release barriers and conditional-release edges.
The complete v2 replay above eliminates those FPs. A MAY release still does
not become a definite free or proof of safety.

An existing unit positive also exposed the same missing-edge mistake:
`opaque_choice()` may always select the releasing arm of a PHI. Its original
program is retained as an inconclusive negative; a separate explicit input
parameter supplies the real feasible positive. This is an oracle correction,
not relabeling a known leak as safe. Subsequent v3 unit runs pass122/122 on
GCC10 and GCC16; harness tests pass110/110. Evidence:
`artifacts/phase5-core-v3/unit-gcc10/report.json` and `unit-gcc16/report.json`.
These v3 unit runs do not replace the v2 full-corpus fingerprint.

Actual parameter-summary transport was validated separately with32 GCC16
compile/link variants: C/C++, partition=none/1to1, no-producer controls,
weak/strong replacement, interposition, type mismatch, O2, fat-LTO and forced
GC. See `artifacts/phase5-provider-smoke/RESULT.md`. Positive remote findings
carry `provider-body` provenance and require a genuine unloaded provider in
the caller partition. These are transport controls, not extra oracle TPs.
The full project runner above remains link-only/partition=none and therefore
does not exercise producer transport or measure its compilation cost.

All full-project timing recorded here is eight-worker exploratory timing.
No controlled speed superiority claim or all-project zero-FP guarantee is
supported. Cross-rule alerts, exact-site scoring and unassessed cases must
remain visible; target FP0 alone does not establish universal correctness.

## Phase-9 bounded ownership refinement — 2026-10-15

The previous handoff correctly identified a GCC API constness build failure.
The pending PHI/nonnull work now compiles with GCC10 and GCC16. Current GCC16
DSO SHA-256 is
`65316e69cf6696b7ff1e33f10fdc5f215f967744683557e007de7cdbf647f68e`.
The retained binary and Compiler source hashes are in
`artifacts/phase9-ownership/final-snapshot/`. Both reviewed C/C++ manifest runs
pass 124/124: `final-unit-gcc10/report.json` and `final-unit-gcc16/report.json`.
Python runner, oracle, merger and manifest checks pass 127/127.

These regression passes include safe and deliberately inconclusive controls.
They are not a count of detected defects. Known buggy oracle groups with no
target finding, including INCONCLUSIVE results, remain false negatives.

The new ownership mechanism treats unresolved/mixed body calls as path-local
witness barriers. A leak can be proved on an independent caller path that
avoids every such call; an unknown outcome inside a callee is not guessed.
Exact allocation/NULL and independent integral entry-parameter zero/nonzero
conditions use a bounded finite domain. A later hard predicate replays the
same prefix into SMT; sibling paths roll back their constraints. Fixed-width
integral AND/OR/XOR lowering is also supported. Solver-disabled and inexact
memory predicates retain their prior conservative policy.

### Full C project oracle

`artifacts/phase9-ownership/final-full-c/comparison.json` fingerprints the
current DSO and all 1,234 completed groups, with no source/input drift and no
injected contracts. Plugin compilation/link/analysis was rerun; native results
were re-scored from the validated v10 reference cache. This is same-input LTO
project detection, not isolated-TU raw accuracy or a fresh native timing run.

| Target CWE | Plugin detected | Native detected | Groups |
| --- | ---: | ---: | ---: |
| 401 memory leak | 460 | 666 | 868 |
| 415 double free | 216 | 228 | 228 |
| 416 use after free | 101 | 80 | 138 |
| Total | 777 | 974 | 1,234 |

Plugin recall is 62.97%, native recall is 78.93%. Target-rule FP is 0 versus
48. Good variants with any lifetime alert remain 87 versus native 182; this
broader alert burden must not be confused with target-rule FP.
Compared with frozen v10, eight additional char UAF groups are detected and
no previously detected group is lost. UAF improvements include the restored
pending PHI work; they are not credited to the new caller-bypass leak theorem.
The ownership bypass theorem improves synthetic caller coverage but does not
recover the large string-I/O Juliet cohort in this replay.

### Largest observed blocker

`final-triage/triage.json` and `triage.html` partition all missed groups using
actual report evidence and preserve native detection and source locations.
The original v10 largest disjoint reason bucket was 306 ambiguous-callee leak
groups. Current early opaque-transfer handling splits those same groups into
167 ambiguous-callee and 139 opaque-transfer records; this is a diagnostic
classification change, not 139 recovered cases. Two further ownership cases
also require realloc reasoning. Together these 308 groups are still the largest
ownership family. The 80 no-target-candidate groups are a separate bucket.

Typical inputs allocate char/wchar strings, use `printLine`/`printWLine`, and
exit without an explicit free. An unconditional stdio borrow contract remains
invalid: `stdio_callback_ownership.c` demonstrates a real cookie callback that
releases an input buffer. Call-binding and conditional ownership summaries,
or explicitly labelled model assumptions, are required before promoting this
family. Full CEGAR/automatic library inference is not implemented.

### Seeded ownership oracle and serial cost

`final-fuzz-seed-20261015/report.json` records 96 generated bounded programs,
an independent exhaustive 32-valuation oracle per program, and both DSO
hashes. There are 73 known leaks and 23 known safe programs. Frozen v10 detects
17 leaks; the current DSO detects 50, with zero alarms on the 23 safe programs.
All 50 independent caller-path witnesses are found. The remaining 23 real
leaks require callee-outcome reasoning and stay missed; passing the validation
does not credit these abstentions as detections. Generated binaries are not
executed. This grammar-specific population is not added to Juliet TPs.

`final-serial-speed/report.json` uses one retained eight-function source, two
warmups, ten serial paired rounds in seeded shuffled order, and compiler plus
waited-for descendant CPU. CPU medians are plain GCC 36.87 ms, frozen v10
61.81 ms, current plugin 39.84 ms, native fanalyzer 42.24 ms. Median paired
current/v10 ratio is 0.646; current/native is 0.939. This limited source is
dominated by simple guards, so these measurements do not establish production
speed superiority. Plugins enable SA001 and JSON output; native uses its normal
rule bundle. All runs and variability are retained.

The original hard caps remain: 4,096 symbolic states/query, 25 ms/solver check,
100 ms/query, shared TU query/state/check bounds, 100 ms/function CPU and
2,000 ms/TU CPU. These bound plugin analysis work, not all of GCC compilation.
Exhaustion yields UNKNOWN/INCONCLUSIVE and is visible as missed coverage.

`report-preview.html` is a merged review of one caller-branch fixture and one
real string-output miss. Inconclusive records retain at most eight actual
ownership-barrier locations, plus expandable preservation and release/transfer
alternatives. These alternatives are explanatory hypotheses, not validated
executions or extra warnings. Direct SA001 findings now include allocation
trace evidence; a complete source exit-path trace remains future work.
