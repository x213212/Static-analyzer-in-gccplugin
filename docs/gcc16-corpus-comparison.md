# GCC 16.2 corpus robustness, localized recall, and configured cost

`tests/compare_benchmark_analyzers.py` runs plain GCC, the repository plugin,
and GCC 16.2's built-in `-fanalyzer` over the same discovered corpus. It is
intended for the 100 extracted MemFix-derived models, all 4,880 bundled Juliet
translation units, or a filtered subset of either.

This runner is separate from `tests/compare_gcc_analyzer.py`. That smaller
comparison checks reviewed unit-test expectations and may use a plugin built
for an older GCC. The corpus runner requires a plugin that can actually load
into GCC 16.2 and uses the same GCC 16.2 driver and common flags for all three
modes. It measures robustness, positive-only localized recall, and end-to-end
configured compiler-process cost. It does not produce a general accuracy or
isolated analyzer-algorithm speed result.

## What is compared

Every selected source is compiled three times, in independent compiler
processes and separate per-case directories:

1. The plain baseline compiles without either analyzer.
2. The plugin mode loads a content-addressed plugin snapshot with
   `doublefreecheck=1`, `memoryleakcheck=1`, `useafterfree=1`, `threadmod=0`,
   `report=1`, and `reporthtml=0`. Stable `[SA001]`, `[SA002]`, and `[SA003]`
   diagnostics are counted from stderr and checked against its JSON report.
   JSON generation is part of the configured process cost; optional HTML
   rendering is deliberately excluded.
3. Native mode adds `-fanalyzer`, `-fanalyzer-verbosity=0`, and
   `-fdiagnostics-format=sarif-stderr`. Results are identified only by SARIF
   `ruleId`, never by localized warning prose.

All three modes use the same language driver, object-generation mode, include
paths, and common flags:

```text
-fdiagnostics-color=never -O1 -fipa-pta -fno-inline
-fno-tree-dce -fno-tree-dse -fno-tree-fre -pthread
```

Only the analyzer-specific switches above differ. Cases are sorted by stable
case ID and assigned round-robin across all six baseline/plugin/native
execution orders, so order counts differ by at most one. This distributes
cold/warm process position instead of permanently favoring one mode and records
the selected order in JSON.

The common memory-lifetime mapping is:

| Meaning | Plugin warning | GCC 16.2 SARIF `ruleId` |
| --- | --- | --- |
| memory leak | `SA001` | `-Wanalyzer-malloc-leak` |
| double free | `SA002` | `-Wanalyzer-double-free` |
| use after free | `SA003` | `-Wanalyzer-use-after-free` |

`SA004` has no direct GCC mapping and is disabled in this corpus comparison.
Its pthread typestate is tested by the reviewed regression suite instead.

Reported durations are elapsed end-to-end compiler-process time, including
startup and each mode's required diagnostics/report work. The baseline makes
incremental analyzer cost visible, but the result remains configured process
cost rather than isolated algorithm CPU time.

## GCC 16.2 and plugin ABI checks

The runner rejects every compiler whose `-dumpfullversion -dumpversion` result
is not **16.2.x**. A vendor suffix such as `16.2.0-vendor1` is accepted, but
16.0, 16.1, 16.3, and a generic non-16.2 `gcc` are rejected. If both C and C++
occur in the selected cases, their full version and target triple must match.

Before corpus execution, the runner compiles a tiny source with the plugin in
every selected frontend. This exercises the plugin's own
`plugin_default_version_check` and catches a stale GCC 10/11 plugin, an
unloadable shared-library dependency, or a C++ frontend incompatibility.
Preflight logs are written below `OUTPUT/preflight/`.

The input `.so` is SHA-256 hashed and copied to
`OUTPUT/plugin-snapshot/<hash-prefix>/` before preflight or analysis. Every
case command uses that immutable path, so rebuilding `Compiler/newplugin.so`
during a long run cannot mix plugin versions. The original path, snapshot
path, and full hash are recorded in JSON.

The extracted target oracle is SHA-256 hashed too. The checked-in
`tests/extracted_oracle.json` was migrated from all 100 historical
`*_spec.json` files at revision `bb14c2d`; it records 156 reviewed allocation
sites together with current fixture hashes and source-line text. Fixture drift
is a configuration error until the affected target is reviewed explicitly.
The report also records the runner SHA-256 and a content fingerprint of the
selected source set. Source and oracle fingerprints are recomputed after all
compiler processes finish; detected input drift prevents publication of a
comparison report.

Build the plugin with GCC 16.2 plugin headers before running the comparison:

```sh
make -C Compiler PLUGIN_CXX=g++-16
g++-16 -dumpfullversion -dumpversion
gcc-16 -dumpfullversion -dumpversion
```

Both version commands must report 16.2.x. The package supplying
`gcc-plugin.h` must also be the matching GCC 16.2 package.

## Commands

Start with a three-case smoke test:

```sh
python3 tests/compare_benchmark_analyzers.py \
  --suite extracted --limit 3 \
  --cc gcc-16 \
  --plugin Compiler/newplugin.so \
  --output artifacts/gcc16-corpus-smoke
```

Run all 100 extracted models:

```sh
python3 tests/compare_benchmark_analyzers.py \
  --suite extracted \
  --cc gcc-16 \
  --plugin Compiler/newplugin.so \
  --jobs 8 --timeout 60 \
  --output artifacts/gcc16-corpus-extracted
```

Run the complete 4,980-translation-unit comparison:

```sh
python3 tests/compare_benchmark_analyzers.py \
  --suite all \
  --cc gcc-16 --cxx g++-16 \
  --plugin Compiler/newplugin.so \
  --jobs 8 --timeout 60 \
  --output artifacts/gcc16-corpus-all
```

`--suite`, `--family`, and `--cwe` are repeatable. `--match` applies a
substring filter to the repository-relative source path, and `--limit` is
applied last to the deterministically sorted selection. Examples:

```sh
# One extracted project.
python3 tests/compare_benchmark_analyzers.py \
  --suite extracted --family openssl --cc gcc-16

# One Juliet CWE, narrowed to matching paths.
python3 tests/compare_benchmark_analyzers.py \
  --suite juliet --cwe CWE416_Use_After_Free --match socket \
  --cc gcc-16 --cxx g++-16
```

The defaults can also be set with `GCC16_CC` and `GCC16_CXX`. C++ is checked
only when the final selection contains a C++ translation unit.

## Failure isolation and evidence

Each compiler-mode attempt has exactly one status:

- `OK`: the compiler exited successfully and, for GCC `-fanalyzer`, stderr was
  valid SARIF.
- `ERROR`: a normal nonzero compile, plugin-load, runner, or SARIF parse error.
- `TIMEOUT`: that compiler process exceeded `--timeout` and its process group
  was killed.
- `CRASH`: a signal exit or compiler-crash signature such as an internal
  compiler error or segmentation fault.

A baseline or analyzer error does not suppress either peer attempt. Work is
parallelized across cases with `--jobs`, while each mode uses its own object
path and retains its raw output, command, duration, and log. One bad case
therefore does not stop the remaining corpus. The program exits 1 after the
reports are written if any mode was `ERROR`, `TIMEOUT`, or
`CRASH`; configuration and ABI failures exit 2 before corpus work.

The output root contains:

```text
comparison.json
comparison.html
plugin-snapshot/<sha-prefix>/<plugin>.so
preflight/plugin_abi_probe_*.log
cases/<suite>/<group>/<case-id>/baseline/compiler.log
cases/<suite>/<group>/<case-id>/plugin/compiler.log
cases/<suite>/<group>/<case-id>/plugin/analysis.json
cases/<suite>/<group>/<case-id>/gcc16-fanalyzer/compiler.log
cases/<suite>/<group>/<case-id>/gcc16-fanalyzer/diagnostics.sarif
```

## Reading the evidence correctly

### Extracted models: positive-target recall only

All 100 extracted files are positive target models. Their directory category
defines one target warning:

| Category | Target | Full-corpus count |
| --- | --- | ---: |
| `leak` | `SA001` | 50 |
| `double_free` | `SA002` | 24 |
| `use_after_free` | `SA003` | 26 |

For each tool and category, the report includes selected targets, `OK`
analyses, failures, same-rule warnings emitted anywhere, localized target
detections, unlocalized warning cases, recall over every selected target, and
recall conditional on an `OK` analysis. A target is detected only when that
rule's primary location, plugin trace, or GCC SARIF codeflow contains a
reviewed allocation site. The primary target recall is:

```text
detected targets / all selected positive targets
```

An `ERROR`, `TIMEOUT`, or `CRASH` is therefore not silently removed from the
primary denominator. It is broken out as `not_analyzed` so an operational
failure cannot make recall look better.

There are no category-matched negative examples in this 100-file corpus.
Consequently, **precision, false-positive rate, specificity, F1, and binary
accuracy are undefined**. The JSON writes `precision: null`, and the HTML says
“positive-only” rather than presenting a fabricated value. Use the paired
Juliet oracle runner for precision and false-positive rates, and the small
`tests/manifest.json` suite for focused regression assertions.

### Juliet: no per-translation-unit accuracy claim

Juliet cases frequently split one logical good or bad program across `a`, `b`,
and later translation units. This runner intentionally compiles translation
units independently, so inferring truth from a filename fragment or counting
each unit as a complete positive/negative test would be inaccurate.

The Juliet section therefore reports only:

- translation-unit coverage and all four execution statuses;
- plugin warning counts and GCC SARIF analyzer-rule distributions;
- total, mean, median, p95, and maximum process duration;
- the same summaries per CWE and three-way-OK configured cost.

It does not report Juliet precision, recall, or good/bad accuracy. That work is
implemented separately by `tests/compare_juliet_oracle.py`, which reconstructs
official manifest groups and compiles paired `OMITGOOD`/`OMITBAD` variants.

## Reading configured cost correctly

Both JSON and HTML include total, mean, median, nearest-rank p95, and maximum
elapsed milliseconds. The headline population contains only translation units
where baseline, plugin, and native analyzer all have status `OK`. For this
three-way common population the report provides:

```text
plugin_duration / baseline_duration
gcc16_fanalyzer_duration / baseline_duration
gcc16_fanalyzer_duration / plugin_duration
```

It also reports each analyzer's per-TU and summed elapsed-time delta over the
plain baseline. Ratios of summed durations avoid giving tiny translation units
the same weight as expensive ones. A ratio describes the cost of these exact
compiler modes; it must not be restated as an isolated analyzer-algorithm speed
claim.

With `--jobs > 1`, independent compiler processes contend for host resources.
The sum of their individual durations and the separately recorded parallel-run
wall time therefore answer different questions. For timing evidence, use an
otherwise idle host, `--jobs 1`, multiple complete runs, and publish the run
conditions with the generated report.

Keep the machine, GCC 16.2 build, plugin and oracle hashes, filters, timeout,
background load, and thermal/power policy fixed. Do not use a timeout-inflated
all-attempt total as a cost result; use the three-way-OK section and inspect all
three status columns first.
