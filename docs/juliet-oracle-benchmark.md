# Juliet manifest-oracle benchmark

`tests/compare_juliet_oracle.py` measures the plugin and GCC 16.2
`-fanalyzer` against the bundled Juliet ground truth. It is deliberately
separate from the legacy robustness runner: a translation unit compiling is a
build result, not evidence that a diagnostic is correct.

## Ground truth and evaluation unit

The bundled oracle is:

```text
Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/manifest.xml
```

The shorter path `Test/benchmarks/Juliet_Test_Suite/manifest.xml` does not
exist in this repository.

For CWE-401, CWE-415, and CWE-416, the manifest describes:

| CWE | Testcase groups | Source translation units | Manifest flaw sites |
| --- | ---: | ---: | ---: |
| CWE-401 | 1,658 | 2,704 | 1,658 |
| CWE-415 | 962 | 1,644 | 962 |
| CWE-416 | 459 | 520 | 459 |
| Total | 3,079 | 4,868 | 3,079 |

There are 1,990 single-TU groups and 1,089 multi-TU groups. Twelve generated
`main.cpp`/`main_linux.cpp` files under the CWE directories are not testcase
sources in `manifest.xml`; the harness excludes and records them instead of
turning them into invented negative examples.

Language is an essential benchmark dimension:

| Language | Testcase groups | Source translation units | Interpretation |
| --- | ---: | ---: | --- |
| C | 1,234 | 1,714 | fair GCC 16.2 native-vs-plugin headline |
| C++ | 1,845 | 3,154 | experimental capability result |

GCC 16.2's own manual states that `-fanalyzer` is only suitable for C in this
release. The report therefore makes the common-paired C subset the headline
comparison. It still preserves and displays C++ results, but the larger C++
subset is never allowed to decide which analyzer wins the supported-language
comparison. Pass `--language c` or `--language c++` to schedule only that
language; a C++-only run deliberately has no native-vs-plugin headline.

The evaluation unit is one manifest `<testcase>` group in one variant:

- bad/positive: compile every member source with `-DOMITGOOD`;
- good/negative: compile every member source with `-DOMITBAD`.

This separation is essential. Most Juliet sources contain both good and bad
functions when neither macro is defined, and helper files without a manifest
`<flaw>` are not automatically clean programs.

## Diagnostic matching

The target-rule mapping is fixed:

| Juliet target | Plugin rule | GCC 16.2 SARIF `ruleId` |
| --- | --- | --- |
| CWE-401 | `SA001` | `-Wanalyzer-malloc-leak` |
| CWE-415 | `SA002` | `-Wanalyzer-double-free` |
| CWE-416 | `SA003` | `-Wanalyzer-use-after-free` |

The plugin run emits its normal diagnostics and a per-TU JSON report. The
harness reads the finding's primary location and all JSON trace locations. The
GCC run uses `-fdiagnostics-format=sarif-stderr`; the harness reads primary,
related, and code-flow locations from SARIF. File URIs are decoded before
matching.

Three views are retained because they answer different questions:

1. **Target-CWE case detection (primary matrix).** A bad variant is a TP when
   the target rule occurs anywhere in the testcase group. A good variant is an
   FP when it emits that testcase's target rule.
2. **Any-lifetime-warning alert burden.** Positive credit still requires the
   target rule, but any mapped `SA001`–`SA003` warning on a good variant counts
   as an alert requiring triage. A cross-rule warning can reveal incidental
   behavior not labeled by Juliet, so this is reported as strict alert burden,
   not an assertion that every such warning is semantically false.
3. **Exact-site localization (secondary matrix).** A bad variant is a TP only if
   a target-rule finding has primary or trace evidence whose file basename and
   line exactly equal a manifest `<flaw file,line>` site. A target warning at a
   different line is recorded as unmatched and the bad variant remains an FN.
   This is localization evidence rather than headline accuracy because some
   Juliet flaw lines point to comments such as “No deallocation”; a compiler
   cannot attach a GIMPLE diagnostic to that comment line and may correctly
   report the allocation or function exit instead.

Every different-CWE finding remains available under `wrong_rule_findings` and
is surfaced in the alert-burden table instead of silently disappearing.

Multi-TU members are compiled independently and then OR-aggregated at group
level. This measures the analyzers as they actually run in this repository; it
does not claim cross-TU analysis. Results are always split into `single_tu` and
`multi_tu` strata so that unsupported cross-file flows cannot be hidden by an
aggregate number.

## Separate source-real LTO project mode

`tests/compare_juliet_project.py` applies the same official oracle to actual
linked testcase programs. It compiles every testcase member plus the real
`testcasesupport/io.c` separately, enables the supplied `INCLUDEMAIN`, and
uses `OMITGOOD` / `OMITBAD` for the positive/negative variants. Baseline,
plugin and native GCC receive identical source/provider inputs and shared
`-O1 -flto -flto-partition=none` flags. Analyzer switches apply only at link.
No source amalgamation, forced ownership contracts, visibility changes or
`-fwhole-program` assumptions are injected.

```sh
python3 tests/compare_juliet_project.py \
  --cc /root/toolchains/gcc-16.2/bin/gcc \
  --plugin Compiler/newplugin.so \
  --group-id-file tests/cohorts/rc2-call-effects.txt \
  --jobs 8 --output artifacts/project-call-effects
```

Omit the group-id file for all supported C groups. A fresh output directory
is required. Results include common-paired and all-selected accuracy, per-CWE
scores, exact-site localization, cross-CWE good-program alert burden, and
unassessed groups. A provider-only warning is retained but cannot give a
testcase a TP unless its primary/trace evidence names a member of that group.
Both tools use that attribution policy. Missing/malformed reports and failed
compilation/linking do not count as negative findings.

One immutable plugin snapshot is reused across groups. Selected sources, the
entire bundled support directory, compiler driver, oracle and runners are
hashed before/after the run. System headers/libraries, compiler subprograms
and environment are not snapshotted. All commands, phase outputs and timings
remain in each group's `bad/project.json` and `good/project.json` artifacts.
Three-mode execution order rotates; timings include each mode's compilation
plus linking and remain exploratory, especially with parallel workers.

Project-mode results are never merged into isolated-TU raw or contract-assisted
headlines. `none` ensures the current late-IPA pass can see loaded provider
bodies; arbitrary LTO partitioning does not provide that guarantee. This mode
does not establish large-project memory/scaling behavior or implement reusable
cross-partition summary streaming.

For a project without an accuracy oracle, `tools/compare_lto_project.py`
provides the same source-real workflow with observed diagnostics and JSON/HTML,
explicitly without TP/FP claims. Supply each real TU with `--source`, shared
compiler extras with repeated `--compile-arg`, and link extras with
`--link-arg`; use `--cc` to select the C or C++ driver.

## Toolchain and ABI controls

Both analyzer modes use the same language driver for a given source. The
harness rejects anything other than a reported `16.2.x` version and requires C
and C++ drivers to have the same version and target triple.

Before any testcase is scheduled, the harness loads the snapshotted plugin in
each selected frontend, compiles a probe, and validates that the plugin wrote a
parseable JSON report. A mismatched GCC plugin ABI is therefore a hard
configuration error, not a corpus failure.

For every real run, the plugin JSON findings must exactly agree with the
stable rule/location diagnostics emitted through GCC stderr. A discrepancy is
an `ERROR`, never an implicit zero-finding result. The report also records
whether Z3 was compiled in and whether SMT refinement was enabled.

Each TU variant gets a plain baseline compile in addition to the two analyzer
compiles. If that baseline fails, neither analyzer is credited with coverage.
This distinguishes fixture/build failures from plugin or `-fanalyzer` crashes
and timeouts. Baseline, plugin, and native analyzer are all real compile-only
commands that generate an object; none is a post-processing pass over an
already-produced `.o` file.

The common flags are identical for baseline, plugin, and native analyzer
processes:

```text
-O1 -fipa-pta -fno-inline -fno-tree-dce -fno-tree-dse -fno-tree-fre -pthread
```

The plain command adds neither `-fplugin` nor `-fanalyzer`. The plugin process
adds only its plugin/report arguments; the native process adds `-fanalyzer`,
`-fanalyzer-verbosity=0`, and SARIF output. C uses GNU C11 and C++ uses GNU
C++14. Thus each overhead denominator uses the same source, variant macro,
frontend, standard, include paths, optimization pipeline flags, and `-c`
object-generation mode as its analyzer numerator.

## Running it

For a separately labeled library-contract experiment, the runner accepts
`--plugin-contract-header tests/contracts/juliet_support_effects.h`. Only the
plugin receives this header. JSON/HTML/console mark the run
`contract-assisted` and `raw_headline_eligible=false`; its speed includes the
additional preprocessing cost and must not be called the same-input raw
comparison. Header path and SHA-256 are recorded and checked before/after.
The bundled header is manually justified from Juliet support/library semantics;
it is not an automatically inferred cross-TU summary.

`--group-id-file PATH` selects exact newline-separated oracle group IDs, with
blank lines and `#` comments allowed. Duplicates, unknown IDs and conflicting
filters fail. The file and its SHA-256 accompany the report and are verified
before/after execution. These controls permit a frozen false-negative cohort
to be replayed without silently changing its population.

Use the three top-level targets to keep accuracy, timing, and experimental C++
results in separate artifacts:

```bash
# Supported C accuracy and coverage; parallelism is configurable.
make compare-gcc16-juliet JULIET_ACCURACY_JOBS=8

# Release timing: run serially into independent output directories.
make compare-gcc16-juliet-timing JULIET_TIMING_RUN=1
make compare-gcc16-juliet-timing JULIET_TIMING_RUN=2
make compare-gcc16-juliet-timing JULIET_TIMING_RUN=3

# C++ capability only; never folded into the native-vs-plugin C headline.
make compare-gcc16-juliet-cxx JULIET_ACCURACY_JOBS=8
```

Set `GCC16_PREFIX`, `GCC16_CC`, or `GCC16_CXX` on any target when GCC 16.2 is
installed elsewhere. `compare-gcc16-juliet-timing` always passes `--jobs 1`;
the run identifier prevents one repetition from overwriting another.

For a custom run, invoke the runner directly. This is the equivalent explicit
single-worker C command:

```bash
python3 tests/compare_juliet_oracle.py \
  --language c --jobs 1 \
  --cc /root/toolchains/gcc-16.2/bin/gcc \
  --plugin Compiler/newplugin.so \
  --output artifacts/juliet-oracle-c-run-1
```

When not using the Make target, run the complete C corpus more than once into
independent output directories and report run-to-run dispersion:

```bash
for run in 1 2 3; do
  python3 tests/compare_juliet_oracle.py \
    --language c --jobs 1 \
    --cc /root/toolchains/gcc-16.2/bin/gcc \
    --plugin Compiler/newplugin.so \
    --output "artifacts/juliet-oracle-c-run-${run}"
done
```

Start with a bounded smoke test:

```bash
python3 tests/compare_juliet_oracle.py \
  --limit 3 --jobs 1 --timeout 30 \
  --output artifacts/juliet-oracle-smoke
```

Useful selection controls:

```bash
# One or more CWEs
python3 tests/compare_juliet_oracle.py --cwe 415 --cwe 416

# Supported C headline only, or experimental C++ only
python3 tests/compare_juliet_oracle.py --language c
python3 tests/compare_juliet_oracle.py --language c++

# Group id, CWE, or member path substring
python3 tests/compare_juliet_oracle.py --match malloc_free_char_52

# Deterministic first N groups after filtering
python3 tests/compare_juliet_oracle.py --cwe 401 --limit 100
```

The default output directory is
`artifacts/juliet-oracle-comparison`. It contains:

- `comparison.json`: complete machine-readable oracle, executions, findings,
  locations, classifications, metrics, and timing;
- `comparison.html`: summary tables plus per-group classifications and links
  to evidence logs;
- `plugin-snapshot/`: content-addressed plugin used by the run;
- `preflight/`: ABI/report probes;
- `groups/`: isolated baseline, plugin, JSON-report, and GCC SARIF artifacts.

## Reported accuracy

For the common-paired C headline and its per-CWE/per-scope breakdown, each
language, and explicitly labeled all-language exploratory slices, the report
contains:

- TP, FP, TN, and FN;
- precision, recall, F1;
- specificity and false-positive rate;
- balanced accuracy and Matthews correlation coefficient;
- target-case, any-lifetime-alert-burden, and exact-site matrices;
- matched target findings, unmatched target findings, and cross-rule findings.

The head-to-head matrix uses only the common paired subset for which both
tools completed both variants, preventing crashes on difficult samples from
improving one side's apparent precision. Tool-specific views are retained for
diagnosis. Coverage is never implicit: the report includes group-variant
and TU-variant counts for `OK`, `BUILD_FAIL`, `ERROR`, `CRASH`, `TIMEOUT`, and
`NOT_RUN`.

`end_to_end_site_recall` and `end_to_end_case_detection_recall` keep all
selected positive groups in the denominator. Consequently a build failure,
crash, timeout, parse failure, or missed diagnostic cannot improve those
numbers by disappearing from the evaluated subset.

## Paired timing

A timing pair is one TU variant whose baseline, plugin, and GCC native analyzer
processes all completed successfully. The report includes:

- baseline, plugin, and GCC process total/mean/median/p95/max time;
- plugin and GCC incremental milliseconds relative to the same baseline;
- each analyzer's ratio of summed wall time to summed baseline wall time;
- each analyzer's total incremental percentage over baseline;
- per-pair analyzer/baseline ratio distributions and slower/faster/tie counts;
- summed GCC/plugin duration ratio and per-pair ratio distribution;
- counts of pairs won by each analyzer;
- the same timing split by CWE and single-/multi-TU scope.

All three measured modes rotate deterministically across the six possible
orders per TU variant. This spreads warm-cache, thermal, and process-position
effects instead of giving the plain baseline a permanent first-run penalty.
For a multi-TU testcase the harness still invokes the same three commands for
every member TU, and only then sums paired TU durations; it never compares one
group-level analyzer process with several baseline processes.

The main overhead statistic is a ratio of sums:

```text
analyzer_over_baseline = sum(analyzer wall ms) / sum(plain GCC wall ms)
incremental_percent = 100 * (sum(analyzer wall ms) - sum(plain GCC wall ms))
                          / sum(plain GCC wall ms)
```

Using a ratio of sums prevents very small TUs from dominating the result, as
can happen with an arithmetic mean of individual ratios. Negative one-shot
deltas are preserved as timing noise rather than silently clamped to zero.

These are end-to-end compiler-process wall times, including plugin JSON
serialization (`reporthtml=0`) and native SARIF serialization. The threaded runner does not
infer per-child CPU time from process-wide counters, because concurrent child
processes would make that attribution invalid. For publication-quality timing,
use an otherwise idle host, `--jobs 1`, multiple complete runs, and report
run-to-run dispersion. Parallel wall time is retained separately for
operational capacity planning.

## Interpretation limits

- GCC 16.2's supported-language headline is C-only. C++ results are explicitly
  experimental in this release.
- Exact manifest lines are a localization oracle, not always an executable
  statement. Do not present exact-site recall without the case-detection view.
- Independent compilation does not provide cross-TU value flow. The multi-TU
  stratum is an end-to-end capability result, not proof that every FN is an
  alias-analysis defect.
- The good/bad macros isolate Juliet's intended target variant. Cross-rule
  warnings on good variants are measured as alert burden, but need manual
  audit before being called semantic false positives.
- One timed process per mode is suitable for a comprehensive comparison but
  not a microbenchmark. Repeated single-job runs are required for tight timing
  confidence intervals.
- A successful compile contributes to robustness/coverage; it does not by
  itself contribute a TP or TN.
