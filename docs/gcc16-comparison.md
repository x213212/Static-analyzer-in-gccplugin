# GCC 16.2 native `-fanalyzer` comparison

This repository compares analyzer *results* on the same C or C++ source with
matching GCC 16.2 language drivers.  One compiler process loads the
GCC-16.2-built plugin; a
separate process runs GCC 16.2's native `-fanalyzer`.  GCC plugins are tied to
the compiler build that supplied their headers, so the runner rejects a plugin
compiler that is not GCC 16.2.x and the plugin's own ABI check remains enabled.

This small comparison uses the reviewed `tests/manifest.json` expectations as
a regression oracle. Its TP/FP/TN/FN, precision, recall, F1, and binary-accuracy
fields are algebraic summaries of agreement with those checked-in regression
expectations. They are not independent evidence of real-world analyzer
accuracy and must not be presented as such. Formal accuracy and false-positive
claims come from the grouped bad/good Juliet manifest-oracle runner; formal
configured-cost claims require its serial repeated-run protocol.

## Reference baseline

As of 2026-09-04, the latest GCC 16 release is **GCC 16.2**, released on
2026-08-07 as a regression-fix release.  The comparison program accepts only
16.2.x executables (including a vendor suffix), records the full version and
banner in JSON, and rejects 16.0, 16.1, 16.3, and other compiler releases.

GCC 16's analyzer is a path-sensitive, interprocedural bug finder based on
coverage-guided symbolic execution.  GCC's own manual explicitly says it is
neither sound nor complete, so a difference is evidence to investigate—not
proof that either analyzer is correct.

GCC 16 changes relevant to this project include:

- a reimplementation of the simulated-memory representation;
- a rewrite of the representation of user code used by the analyzer;
- initial use of GCC `value_range` information to eliminate some false
  positives;
- initial support for small C++ examples and exceptions, with documented scale
  limits.  C++ cases in the curated suite are therefore retained as an
  experimental slice, while the manifest-oracle winner is decided on C.

Official sources:

- [GCC 16 release series](https://gcc.gnu.org/gcc-16/)
- [GCC 16 changes, including static-analyzer changes](https://gcc.gnu.org/gcc-16/changes.html)
- [GCC 16.2 static analyzer options](https://gcc.gnu.org/onlinedocs/gcc-16.2.0/gcc/Static-Analyzer-Options.html)
- [GCC 16.2 diagnostic output formats](https://gcc.gnu.org/onlinedocs/gcc-16.2.0/gcc/Diagnostic-Message-Formatting-Options.html)

## Stable diagnostic mapping

The comparison is based on machine-readable identifiers, never translated or
human-readable warning text.

| Plugin | GCC 16 SARIF `ruleId` | Meaning |
|---|---|---|
| `SA001` | `-Wanalyzer-malloc-leak` | allocated object is leaked on a feasible path |
| `SA002` | `-Wanalyzer-double-free` | deallocator can be called twice on a pointer |
| `SA003` | `-Wanalyzer-use-after-free` | pointer is used after its deallocator |
| `SA004` | no direct mapping | joinable pthread lifetime is a plugin-specific check |

The official GCC 16.2 `-fanalyzer` option list has no direct pthread
create-without-join diagnostic.  Thread cases are still compiled by both tools,
but their comparison status is `NOT_SUPPORTED`, not a misleading match.

GCC 16 removed the old `-fdiagnostics-format=json` output.  The runner therefore
uses `-fdiagnostics-format=sarif-stderr` and reads each SARIF result's `ruleId`.
Every other `-Wanalyzer-*` rule is retained in `analyzer_rule_ids`, even when it
does not map to one of the plugin's four current diagnostics.

## Run

Build the plugin with GCC 16.2's matching C++ driver and plugin headers, then
run both sides with GCC 16.2:

```sh
make -C Compiler PLUGIN_CXX=/path/to/g++-16.2

python3 tests/compare_gcc_analyzer.py \
  --plugin-compiler /path/to/gcc-16.2 \
  --plugin-cxx /path/to/g++-16.2 \
  --plugin Compiler/newplugin.so \
  --gcc16 /path/to/gcc-16.2 \
  --gcc16-cxx /path/to/g++-16.2 \
  --manifest tests/manifest.json \
  --output artifacts/gcc16-comparison
```

When `--gcc16` is omitted, the runner uses `--plugin-compiler` for the native
side too.  Use `--require-gcc16` in CI when absence of the reference compiler
must fail the job.

The resulting `accuracy` object is retained for machine compatibility, but in
this report it means regression agreement with the reviewed manifest. Use
`make compare-gcc16-juliet` for the supported C accuracy/coverage cohort,
`make compare-gcc16-juliet-timing JULIET_TIMING_RUN=N` for serial timing runs,
and `make compare-gcc16-juliet-cxx` for the explicitly experimental C++ slice.

Each manifest case is run in its own directory and each tool gets a fresh
compiler process:

- the plugin side selects C or C++ from the manifest, uses `-O1`, IPA
  points-to analysis, and the same optimization-preservation switches as the
  isolated plugin unit tests;
- the GCC 16 side uses the matching language mode, `-O0`, and `-fanalyzer`.
  Disabling optimization is intentional: the GCC manual notes that optimization can remove program
  states before the analyzer sees them;
- both sides compile only (`-c`) and use separate object and diagnostic files.

The terminal table distinguishes `MATCH`, `DIFFERENT`, `NOT_SUPPORTED`,
`ERROR`, and `SKIP`.  The complete report is
`artifacts/gcc16-comparison/comparison.json`; per-case plugin logs and GCC SARIF are
stored beside it.

Exit status is nonzero for plugin expectation regressions or compiler execution
errors.  A missing GCC 16 produces an explicit `SKIP`; it becomes exit status 2
when `--require-gcc16` is present.

## Reading a difference

Do not optimize toward agreement blindly.  For each `DIFFERENT` case, compare:

1. whether the allocation escapes through an opaque call;
2. whether aliases identify the same allocation or merely may overlap;
3. whether a PHI combines mutually exclusive paths;
4. whether both release operations are executable on one CFG path;
5. whether a derived pointer, `realloc`, or an interprocedural summary changed
   ownership.

Those are the places where this plugin's GIMPLE/SSA alias expansion and GCC
16's symbolic region model make materially different decisions.
