# Current engine only — 2026-10-17

`main` now builds version `1.0.0-rc3` with one bounded modular analyzer.
The historical implementation is removed, including its 14 implementation
headers, adapter, old-only options, and hand-written benchmark Makefile.
No forwarding headers or alternative legacy engine are retained in the source
tree. Existing Git history and the pre-existing backup branch are unchanged.

## Core responsibilities

| Removed implementation | Current core responsibility |
| --- | --- |
| `Algorithm.h` | `src/analyzer.cc` orchestrates the analysis phases |
| `analyze.h`, `check.h` | `lifetime_engine.cc`, `edge_alias.cc`, `feasibility.cc` check lifetime events and feasible paths |
| `collect.h`, `getinfo.h`, `mapping.h` | `ownership_call.cc`, `ownership_summary.cc`, `provider_transport.cc` classify calls and build body-bound ownership facts |
| `new_imm_use.h`, `trace.h` | `gimple_flow.cc`, `gimple_value.cc`, `memory_value.cc` expand SSA and memory provenance with explicit bounds |
| `dfs.h` | `cfg_evidence.cc` and `feasibility.cc` consume GCC CFG evidence |
| `unit.h`, `debug.h`, `performance.h` | `report.cc` and `analysis_budget.cc` own diagnostics, reports and CPU/work accounting |
| `define.h`, `gimplearray.h` | Typed interfaces under `include/static_analyzer/` replace the old shared globals and linked tables |

This maps responsibilities, not a promise that every historical heuristic
was ported. Hard-coded editor integration and unbounded recursive heuristics
are intentionally removed. The current call graph report is selected with
`reportcallgraph=1`. Optional SDK reviews and unknown-call assumption experiments
retain their distinct conditional status; they do not become source-body proofs.

The removed `fprintf2` implementation is no longer compiled or callable.
Production code compiles with `-Werror=return-type` and has no warning-suppressed
legacy translation unit. Historical GCC13.3 crash behavior is not represented
as having been retested on that compiler.

## Build structure

| File | Responsibility |
| --- | --- |
| `Makefile` | Default target and task-module includes |
| `mk/config.mk` | Compiler selection, output directories and shared flags |
| `mk/build.mk` | Build, help and clean |
| `mk/reports.mk` | Single-source analysis and project report merging |
| `mk/tests.mk` | Python/compiler tests and option validation |
| `mk/smoke.mk` | Alias, memory, path-scaling and budget smoke checks |
| `mk/benchmarks.mk` | Isolated corpus and native analyzer comparisons |
| `Compiler/Makefile` | Explicit current-engine object list and plugin link |
| `Compiler/toolchain.mk` | GCC ABI, build isolation, optimization and Z3 setup |

`make` builds the plugin; `make test` runs tests explicitly. Shell recipe
failures stop the task. `make clean` removes compiler build outputs while
retaining reports and measurements, and does not require an installed compiler.
Report-only and Python tasks likewise do not probe a compiler unnecessarily.

The plugin ABI still has to match the GCC version loading it. Supporting
separate GCC10/GCC16 builds uses the same current engine, not the deleted engine.

## Removed command-line options

Remove `legacyengine`, `debugoutput`, `debugmod`, `traceallfucntion`,
`contextsentive`, `relatemod`, `retnotheapobjskipcheck`, `prebranchexit`, and
`vscode_extensionmod` from external build commands. Both `=0` and `=1` are
rejected as unknown arguments. Maintained tools no longer pass these options.

```sh
make plugin PLUGIN_CXX=g++-16
make check-options PLUGIN_CXX=g++-16
make test PLUGIN_CXX=g++-16
```

Default proof policy, CPU/work/state limits, report schemas, rule meanings,
and the modern regression expectations are preserved. Frozen measurements
from 2026-10-15 retain their original binary fingerprints and are not silently
relabelled as measurements of the new build.
