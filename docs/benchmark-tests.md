# Legacy benchmark testing

The repository contains two different kinds of tests.  Keep them separate:

- `tests/manifest.json` is the small assertion suite.  It checks reviewed
  `SAxxx` findings and is suitable for regression gating.
- `Test/benchmarks` is a build/robustness corpus.  It contains 100 reduced
  real-project files and 4,880 Juliet translation units.  It is too large and
  too mechanically generated to use as a source of hand-written assertions.

`tests/run_benchmarks.py` compiles every selected source in its own process and
directory.  It first compiles without the plugin, then repeats the same compile
with the plugin.  Results therefore have precise meanings:

- `PASS`: the source compiles, and (when supplied) the plugin completes.
- `BUILD_FAIL`: the source itself does not compile with the selected compiler.
- `PLUGIN_CRASH`: the baseline compile passed but the plugin compile failed or
  timed out.

The runner does not link cases.  This is deliberate: many Juliet variants are
split across `a`, `b`, and later translation units, while GCC IPA passes still
run during an object-only compile.  C is compiled as GNU C11 and C++ as GNU
C++14.  Juliet gets its official `testcasesupport` include directory.  The
extracted files reference their bundled compatibility headers with explicit
relative paths; those directories must not be added with `-I`, because each
family also ships a file named `stdio.h` which would shadow the host C library.

## Commands

Audit all 100 extracted files without a plugin:

```sh
python3 tests/run_benchmarks.py \
  --suite extracted --cc gcc-10 --cxx g++-10 \
  --output artifacts/benchmarks/extracted-baseline
```

Run them through the GCC 10 build of this plugin:

```sh
python3 tests/run_benchmarks.py \
  --suite extracted --cc gcc-10 --cxx g++-10 \
  --plugin Compiler/newplugin.so \
  --output artifacts/benchmarks/extracted-plugin
```

Smoke-test ten Juliet sources before a full 4,880-file run:

```sh
python3 tests/run_benchmarks.py \
  --suite juliet --cc gcc-10 --cxx g++-10 \
  --plugin Compiler/newplugin.so --limit 10 \
  --output artifacts/benchmarks/juliet-smoke
```

Run a single CWE, family, or pathname fragment with `--cwe`, `--family`, or
`--match`.  Add `--verbose` for one line per source.  Every run writes an atomic
`report.json` plus separate baseline/plugin logs under `cases/`.  The runner
copies the plugin into a content-addressed snapshot first, so rebuilding the
working-tree `.so` during a long Juliet run cannot mix two plugin versions.

The plugin must be built with headers from the same GCC major version used to
load it.  A GCC 10 plugin cannot be used to assess GCC 16; rebuild it against
GCC 16 plugin headers and pass `--cc gcc-16 --cxx g++-16` for that run.

## Why the historical Makefiles fail

`Test/Makefile.legacy` hand-lists every extracted source, links each case even
though compile-only analysis is sufficient, prefixes benchmark recipes with
`-` (so GNU Make hides failures), and mixes all diagnostics into `check.log`.

The bundled Juliet Makefiles hard-code `gcc-11.1`/`g++-11.1`, comment out the
C++ compile rule, filter out platform cases, and use aggregate targets that can
hide which translation unit failed.  They are retained as upstream material,
not as the repository's test driver.

The discovery invariants can be checked without invoking GCC:

```sh
python3 -m unittest tests.test_benchmark_manifest
```

## Validated corpus snapshot

On 2026-09-04 the full corpus was compiled with GCC/G++ 10.5.0 and plugin
snapshot SHA-256
`ca8d7444f25bef645eb7f63b3a28d60753a17e691596297477a49ef3e380cfb7`:

| Corpus | Sources | PASS | BUILD_FAIL | PLUGIN_CRASH |
| --- | ---: | ---: | ---: | ---: |
| Extracted projects | 100 | 100 | 0 | 0 |
| Juliet CWE401/415/416 | 4,880 | 4,880 | 0 | 0 |
| Total | 4,980 | 4,980 | 0 | 0 |

The Juliet inventory is 1,714 C and 3,166 C++ files: 2,710 CWE401, 1,648
CWE415, and 522 CWE416 translation units.  `PASS` here is a build and plugin
robustness result.  It does **not** claim that every emitted or missing finding
matches Juliet's good/bad ground truth; accuracy metrics must be calculated as
a separate evaluation.

The extracted corpus originally had 21 source-level build failures.  Repairs
were limited to compatibility scaffolding: missing declarations and system
headers, duplicate reduced-project typedef/enum/struct definitions, OpenSSL
`STACK_OF`/X509 chain stubs, a duplicated OpenSSH compatibility fragment, and
tmux's BSD-only `sysctl` include.  The vulnerability-bearing control flow was
left intact.
