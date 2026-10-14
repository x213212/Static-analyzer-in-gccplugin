# 1.0.0-rc1 validation record

This document records the measurements used to evaluate the refactored
analyzer.  It is deliberately a mixed result: the proof-only configuration
had a lower observed target-rule false-positive rate than GCC 16.2 native on
this C corpus, but it did not match native analyzer recall, and configured
process cost differed between the measured corpus regimes.  These numbers are
evidence for the frozen artifacts below, not a claim that arbitrary programs
have zero false positives or constant overhead.

## Frozen inputs

The measurements were produced on 2026-09-04 UTC with GCC/G++ 16.2.0 and the
following SHA-256 identities:

| Artifact | SHA-256 |
| --- | --- |
| `Compiler/newplugin.so` | `2a7fd43d5908346f13be1ba5c60b2f04d9104ff1c6f394658898781802b414c0` |
| formal Juliet runner | `c0d15b20290322dfe7a6fb8238ba88aef6423b34061e45c993073d19c6dcbf3b` |
| official Juliet manifest | `11e03dd07abdf057d63dc99474faf45574f9334083d05a28835e495ed8bd30bb` |
| selected C source set | `7a63e79dc90beae7b53f8890ef6b83571e047bd1ebcab8d9573e84914c5801c7` |
| experimental C++ source set | `be904cd6ffe506b42bc5105f8df7eb799f062d87111e3027fa7ba403a738b460` |
| extracted-corpus runner | `d6ffa621c5b748e0b976bf5bd45191bc9824107fc3822d147921b525cf76992a` |
| extracted oracle | `d88c53961bf252b4d79f5bb0095a2eea0f9a913c90bc4587883f629714b9143b` |
| selected extracted source set | `bfcba7495d8580470970344dbb89953775ffccfc4a274e47c7490b7d2646b791` |

Both runners verified that their manifest/oracle and selected sources were
unchanged before and after each measured run.  The formal Juliet runner also
reverified its own runner hash after each run; the extracted-corpus runner hash
was recorded as input identity but was not reverified after execution.
Baseline, plugin, and native analyzer processes used the same common
optimization flags.  Their order was round-robin counterbalanced across all
six permutations.

## Formal C accuracy

The supported headline consists of all 1,234 selected C testcase groups
(1,714 source TUs and 3,428 bad/good TU variants).  Plain GCC, the plugin, and
GCC `-fanalyzer` completed every variant, so common-paired coverage was 100%.
The primary score is target-rule detection within the testcase group; exact
manifest-site localization is a separate secondary score.

| Tool | TP | FP | TN | FN | Precision | Recall | F1 | FPR | Balanced accuracy |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| plugin, proof-only | 178 | 0 | 1,234 | 1,056 | 100.00% | 14.42% | 25.21% | 0.00% | 57.21% |
| GCC 16.2 `-fanalyzer` | 725 | 85 | 1,149 | 509 | 89.51% | 58.75% | 70.94% | 6.89% | 75.93% |

The rule-level breakdown is:

| Rule / CWE | Plugin TP / total | Plugin FPR | GCC TP / total | GCC FPR |
| --- | ---: | ---: | ---: | ---: |
| SA001 / CWE401 leak | 100 / 868 (11.52%) | 0.00% | 515 / 868 (59.33%) | 9.79% |
| SA002 / CWE415 double free | 78 / 228 (34.21%) | 0.00% | 156 / 228 (68.42%) | 0.00% |
| SA003 / CWE416 use after free | 0 / 138 (0.00%) | 0.00% | 54 / 138 (39.13%) | 0.00% |

No plugin SA001--SA003 finding of any rule occurred on a good C variant.  GCC
had 85 target-rule false positives and 221 good variants with at least one
memory-lifetime alert when cross-rule alert burden was included.  The observed
plugin result therefore supports the intended low-noise policy on this corpus,
but the 14.42% recall and 25.21% F1 mean it does **not** beat GCC in
target-rule case detection on this corpus.

The largest target-rule inconclusive reasons among plugin false negatives were
`opaque-ownership-transfer` (608 testcase groups),
`unsupported-path-condition` (146), `opaque-call-dereference` (54),
`path-feasibility-budget` (60), and `reallocation-status-unproven` (18).
Reasons can overlap.  All 324 multi-TU C groups produced no plugin true
positive because summaries are currently TU-local; on the 910 single-TU
groups the plugin found 178 (19.56%).  In particular, many Juliet UAF sinks
pass a freed pointer to an external test-support printer.  Without the callee
body or a must-dereference/no-escape contract, proof-only correctly abstains
instead of treating a possible dereference as a proved one.

## Configured process cost

Timing is end-to-end compiler-process wall time, including startup,
diagnostics, and the plugin's required JSON report.  It is not isolated
algorithm CPU time.  The fair value is a ratio of summed durations over the
same three-way-OK TU variants, with a plain GCC process as baseline.

Three complete serial (`--jobs 1`) C runs produced:

| Run | Plugin / baseline | Native / baseline | Native / plugin |
| --- | ---: | ---: | ---: |
| 1 | 1.145579 | 1.061987 | 0.927031 |
| 2 | 1.147144 | 1.059135 | 0.923280 |
| 3 | 1.144153 | 1.061416 | 0.927687 |
| median | **1.145579** | **1.061416** | **0.927031** |

Thus the median overhead on the many short C TUs was 14.56% for the plugin and
6.14% for native `-fanalyzer`; the plugin process cost was about 7.87% higher
than native.  A separate eight-worker accuracy run gave a similar direction
(14.17% versus 6.39%), but its process timings are not the release timing
headline.

The three serial runs over the 100 larger historical extracted models showed a
different size regime:

| Statistic (three-run median) | Ratio |
| --- | ---: |
| plugin / baseline | 1.091291 |
| native / baseline | 2.938650 |
| native / plugin | 2.673900 |

On those models the plugin added about 9.13% over baseline, while native
`-fanalyzer` added about 193.87% and cost 2.67 times as much as the plugin.
This positive-only corpus is not an accuracy set: allocation-site-localized
target recall was 3/100 for the plugin and 26/100 for native, and precision/FPR
are undefined.

The result is therefore not “no slowdown.”  The plugin has bounded work and
was faster than native on the larger extracted TUs, while native was faster on
the tiny Juliet C units.  Fixed shared-library/Z3 loading and per-TU report
work are plausible contributors, but these end-to-end measurements do not
isolate component costs or establish that translation-unit size caused the
difference.

## C++ experimental slice and robustness

The C++ run is explicitly experimental because GCC 16.2 documents its native
analyzer comparison surface as C.  Both tools nevertheless completed all
1,845 groups (6,308 TU variants).  The plugin recorded TP=348, FP=0,
TN=1,845, FN=1,497 (precision 100%, recall 18.86%); native recorded TP=984,
FP=580, TN=1,265, FN=861 (precision 62.92%, recall 53.33%).  Its eight-worker
configured-cost ratios were 1.0844 plugin/baseline and 1.3068 native/baseline;
these are exploratory, not serial release timing.

The isolated robustness runner compiled all 4,980 discovered inputs (100
extracted and 4,880 Juliet TUs) with the plugin: 4,980 PASS, zero build failure,
zero plugin crash, and zero timeout.  The assertion suite passed 85/85 with
both GCC 16.2 and GCC 10, and the bounded provenance, alias cache, Memory-SSA,
CPU/work-budget, feasibility-budget, and 30-diamond path-scaling smokes passed.
The robustness run used `threadmod=1` and `report=0`, whereas the formal
comparison runners used `threadmod=0` and JSON reporting.  It establishes
load/build stability for that separate configuration, not same-configuration
accuracy or timing evidence.

## Reports

The local HTML/JSON evidence is generated under `artifacts/`:

- `final-juliet-c-accuracy-rc1/`
- `final-juliet-c-timing-rc1-run1/`, `run2/`, and `run3/`
- `final-gcc16-extracted-fair-rc1/`, `run2/`, and `run3/`
- `final-juliet-cxx-experimental-rc1/`
- `final-robustness-all-gcc16-rc1/`
- `final-tests-gcc16-1.0.0-rc1/`
- `final-tests-gcc10-1.0.0-rc1/`

These directories are intentionally build artifacts rather than checked-in
source.  Only the directories listed above are evidence for this release;
other pre-existing `final-*` directories are historical and must be excluded
from a release bundle.  Reproduce the formal C and experimental C++ reports as
described in [`juliet-oracle-benchmark.md`](juliet-oracle-benchmark.md).
Use the direct corpus runner documented in
[`gcc16-corpus-comparison.md`](gcc16-corpus-comparison.md) for extracted and
robustness reports; the three extracted timing runs require `--jobs 1` and a
distinct `--output` path per run.  Reproduce assertion reports with the
matching-compiler test procedure in the README's
[`Tests and reports`](../README.md#tests-and-reports) section.
