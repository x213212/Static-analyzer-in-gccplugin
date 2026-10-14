# GCC GIMPLE Static Analyzer Plugin

A GCC plugin for detecting memory and resource lifetime errors while compiling
C and C++ code. The default engine follows real GIMPLE SSA value flow, asks GCC's
points-to/alias oracle for evidence, computes a bounded typestate fixed point
over the CFG, and emits stable diagnostics plus HTML and JSON reports. The
default `proof-only` policy emits a finding only when the modeled alias,
ownership, event ordering, and path constraints are all proved.

Current measurements, remaining cases, and model limits are recorded in
[current progress](docs/current-progress-2026-10-15.md).

| Rule | Meaning |
| --- | --- |
| `SA001` | a heap allocation can reach function exit without release or escape |
| `SA002` | the same allocation can be released twice on one feasible execution |
| `SA003` | the same allocation, including an interior pointer, can be used after release |
| `SA004` | a definitely joinable pthread can reach exit without join or detach |

The pthread typestate is implemented, but default proof-only currently records
SA004 candidates as `INCONCLUSIVE` because `pthread_create` success/failure is
not yet correlated with the returned status. Exploratory mode can emit it;
lock-order/deadlock analysis is not implemented.

The refactored engine is the default. The historical detector remains behind
`legacyengine=1` for compatibility, but it is not used for test assertions.

## Quick start

Install a compiler and its matching plugin development headers. On Debian or
Ubuntu, for example:

```sh
sudo apt install gcc-12 g++-12 gcc-12-plugin-dev
```

The build selects an installed G++ whose plugin directory contains
`gcc-plugin.h`:

```sh
make plugin
```

To select one explicitly:

```sh
make plugin PLUGIN_CXX=g++-12
```

GCC plugin ABI is version-specific. A plugin built from GCC 12 headers must be
loaded by GCC 12, not GCC 10 or GCC 16.

Z3 is optional. When `pkg-config z3` is available, the build enables bounded
SMT path refinement. Without it, proof-only mode records path-dependent
candidates as `INCONCLUSIVE`; CFG may-reachability alone never becomes a
finding. `analysispolicy=exploratory` is the explicit fail-open research mode.
With Z3 enabled, `libz3` remains a direct shared-library dependency.  The
plugin constructs its translation-unit context lazily and creates a fresh
solver only for a query containing an exactly encodable predicate; this
reduces solver work but does not remove loader cost.

## Analyze one source file

```sh
make analyze SOURCE=path/to/input.c
```

Compilation emits normal GCC diagnostics and writes:

```text
artifacts/analyze/analysis.html
artifacts/analyze/analysis.json
```

The HTML report contains stable rule IDs, source locations, trace evidence,
`PROVEN`/`EXPLORATORY` status, confidence, manual repair guidance, and a
separate bounded `INCONCLUSIVE` section. JSON is the machine-readable contract
used by tests and tools. Inconclusive candidates do not emit GCC warnings and
are not included in SA001--SA004 finding counts. `automatic_patch` is currently
always `false`; the constrained MemFix-style repair stage is designed in
[memfix-repair.md](docs/memfix-repair.md), but is not misrepresented as an
implemented source patcher.

For build systems using `-Werror`, add
`-fplugin-arg-newplugin-reportonly=1`. The same findings and traces are retained
in HTML/JSON, with `analysis.diagnostics=report-only`, without issuing compiler
warnings or notes. This does not disable or downgrade GCC's own diagnostics.
The default warning mode is unchanged. Keep `report=1` when using this mode.

An optional FFmpeg SDK profile is available with
`-fplugin-arg-newplugin-librarymodel=ffmpeg` (default: `none`). It checks
output-parameter resources and registered filter initialization/cleanup
callbacks. These results are explicitly `MODEL_REVIEW` with conditional
confidence, a recorded `model_scope`, and a separate `path_verdict`. They are
not source-body proofs; `unknown` paths remain visible for manual review.
`summary.model_review_total` separates them from ordinary findings. This
profile can be combined with `reportonly=1` for build integration. See the
[module boundaries](docs/architecture.md#optional-library-resource-protocols)
and [real-project results](docs/realworld-validation-2026-10-15.md).

The [real-project validation](docs/realworld-validation-2026-10-15.md) separates
successful builds, measured compiler cost, upstream-confirmed leak fixes,
missed defects, and unresolved ownership. Zero warnings alone is not an
accuracy or false-positive score.

The equivalent direct invocation is:

```sh
gcc-12 \
  -fplugin=Compiler/newplugin.so \
  -fplugin-arg-newplugin-useafterfree=1 \
  -fplugin-arg-newplugin-reportpath=/tmp/my-analysis \
  -O1 -fipa-pta -fno-inline -pthread \
  -c input.c -o input.o
```

This writes `/tmp/my-analysis.html` and `/tmp/my-analysis.json`. Without
`reportpath`, reports use GCC's resolved output/dump stem, so parallel sources
with the same basename but different object paths do not overwrite each other.
Each report file is published by an atomic rename.

Unknown external ownership can be inspected using
`tools/callee_assumptions.py`. Its HTML buttons select three separately
compiled results: unknown, assumed borrow, and assumed release. The plugin
arguments are `calleemodel=unknown|borrow|release`, `calleesymbol=<exact asm
symbol>`, and `calleearg=<zero-based operand index>`. An assumption applies
only to an unresolved external call; a loaded source body takes precedence.
Assumption-mode findings are labeled `ASSUMPTION_BASED` and receive no credit
in the confirmed benchmark score.

`tools/trace_binary_callees.py` optionally examines the selected x86-64 ELF
provider after linking, using a GNU ld `--cref` map. It caches bounded
disassembly evidence by binary/build/tool hashes. This inspection remains
report evidence: seeing a call to `free` does not prove ownership of a
particular caller argument or runtime symbol binding.

The plugin runs inside the same GCC process that is producing `input.o`; it
does not analyze an already-built object file.  Its late GIMPLE pass reuses
the compiler's existing SSA, CFG, IPA points-to, and Memory-SSA information,
so there is no second source parse or separately constructed whole-program
IR.  That makes the integration lightweight, but not free: ownership
summaries, alias queries, and candidate-only SMT checks still add compile
time.  The Juliet comparison therefore records a plain-GCC baseline as well
as plugin and native `-fanalyzer` timings. Because the pass intentionally runs
after late IPA points-to, it analyzes the GIMPLE that survives the selected
optimization pipeline. An allocation that GCC has proved completely dead and
removed before this point is outside that invocation's report; reproducible
detector tests disable the relevant DCE/DSE/FRE transforms so the intended bug
event remains present.

### Analyze a normal project build

`tools/static_analyzer_cc.py` is a conservative compiler launcher. It injects
the production proof-only configuration only when the driver command contains
exactly one translation unit; preprocess, dependency-only, version, and link
commands pass through unchanged. Multi-source and conflicting plugin commands
are rejected rather than assigned an ambiguous report name.

For one command:

```sh
python3 tools/static_analyzer_cc.py \
  --compiler /root/toolchains/gcc-16.2/bin/gcc \
  --plugin "$PWD/Compiler/newplugin.so" \
  --report-dir "$PWD/artifacts/project-tu-reports" -- \
  -O2 -c src/example.c -o build/example.o
```

For a Make-based C project, the same launcher can be supplied as `CC` (and a
second invocation using G++ as `CXX`):

```sh
make -C /path/to/project \
  CC="python3 $PWD/tools/static_analyzer_cc.py --compiler gcc \
      --plugin $PWD/Compiler/newplugin.so \
      --report-dir $PWD/artifacts/project-tu-reports --"
make project-report
```

The launcher derives a deterministic collision-resistant report stem from the
source, output, compiler arguments, plugin path, and options. It writes JSON
only for each translation unit, avoiding redundant HTML work during a large
build. The second command recursively merges those JSON files into
`artifacts/project-report/report.html` and `report.json`. The merger preserves
report provenance and keeps findings separate from bounded inconclusive
coverage gaps.

## Alias model

Alias is not represented as one Boolean:

```text
same SSA value > provenance-backed must alias > may alias > no alias / unknown
```

- Each allocation statement defines an abstract object identity.
- `SSA_NAME_DEF_STMT`, PHIs, copies, casts, conditionals, and pointer offsets
  are expanded backward with cycle and depth bounds. PHI arguments retain
  their exact `gimple_phi_arg_edge`, so an incoming value is not detached from
  the control-flow edge on which it exists.
- A pointer returned after a proved release is a dangling-pointer use/escape.
  If lowering merges source returns into an artificial `GIMPLE_RETURN`, the
  report reads `gimple_phi_arg_location` from the same feasible PHI edge used
  by the lifetime proof instead of guessing a source line.
- `FOR_EACH_IMM_USE_STMT` and `FOR_EACH_IMM_USE_ON_STMT` expand uses forward.
- `SSA_NAME_PTR_INFO`, `pt_solution`, and `ptr_derefs_may_alias_p` provide
  GCC's points-to/no-alias evidence after the `pta` pass.
- Allocation and release calls are classified from GCC declarations and
  built-in codes (`DECL_IS_MALLOC`, `DECL_IS_OPERATOR_NEW_P`,
  `DECL_IS_OPERATOR_DELETE_P`, and `BUILT_IN_*`) rather than demangled-name
  guesses. Ordinary `new`/`new[]` and `delete`/`delete[]` participate in the
  same GIMPLE ownership model; placement new is deliberately excluded. In
  proof-only mode, `xmalloc`/`xfree` spelling is not ownership evidence and a
  plain no-alias `malloc` attribute is insufficient unless it names a release
  contract the engine can close.
- Pointer loads are followed through virtual SSA with GCC's
  `walk_aliased_vdefs`, `ao_ref`, `stmt_may_clobber_ref_p_1`, and
  `stmt_kills_ref_p`.  A load inherits provenance only from a unique,
  must-killing pointer store; calls, volatile accesses, ambiguous stores,
  entry reachability, cycles, and traversal limits stay unknown.
- Actual dereferences are classified with
  `walk_stmt_load_store_addr_ops`; the mere presence of a statement VUSE is
  not treated as proof that every pointer operand was dereferenced.
- `may alias` is never promoted to `must alias`. A deallocation needs
  ownership-grade provenance before it changes an object's lifetime.
- An interior pointer identifies the same allocation for UAF, but is not
  accepted as the base pointer for `free` ownership.
- Unknown definitions and clobbers remain unknown instead of being guessed.

Complete alias analysis for arbitrary C programs is undecidable. The
implementation records proof quality rather than hiding uncertainty.

See [architecture.md](docs/architecture.md) for the GCC API boundary and
[research-foundation.md](docs/research-foundation.md) for the relationship to
the original 2022 thesis, MemFix, Predator, SABER, CRed, and GCC's analyzer.

## Avoiding path explosion

For each allocation or thread site, the primary analysis solves a finite
typestate lattice over CFG blocks until a fixed point; branch states join and
loops converge. Candidate feasibility queries explore selected paths within
explicit state, constraint, and time limits.

SMT is demand-driven. Only a candidate sequence such as
`allocation → free → free/use` is checked. Leak candidates ask whether a
feasible allocation-to-exit path can avoid every matching release. The solver
uses reverse reachability, incremental constraints, strict state/check caps,
and timeouts. A fresh solver and symbol map isolate every query; one lazy Z3
context is reused within that translation unit. Unsupported or inexact
predicates never construct a solver.

Before splitting a symbolic state, three narrow structural proofs reduce path
growth. Event-neutral true/false diamonds are joined at their common
successor. Canonical unsigned counted loops are skipped only when their normal
exit is mathematically proved reachable before modular wrap; the runtime-bound
case is limited to `i = C; i < B; ++i` with an invariant same-width unsigned
SSA bound. A separate one-trip theorem does not skip an eventful body: it
traverses the real body and provenance edges exactly once, then follows the
proved latch-to-exit transition. Each recognizer requires ordinary CFG edges
and rejects relevant events where abstraction would hide them. A loop visit
cap marks only that branch unknown, so an exact sibling can still prove a
finding.

In the default proof-only policy, only a complete `SAT` witness permits a
warning. Unsupported GIMPLE, timeout, missing solver support, a disabled
solver, or an unproved loop returns `unknown` and is reported only as
`INCONCLUSIVE`. This changes the tradeoff deliberately: fewer uncertain
warnings and potentially lower recall. The implementation does not claim a
mathematical zero-false-positive guarantee for arbitrary C/C++, which is not
decidable in general.

The non-SMT work is bounded too. GIMPLE provenance expansion memoizes the
shared SSA sub-DAG within one query and caches top-level values per function;
node, work, origin, and cache limits turn exhaustion into explicit unknown
origins. A translation-unit/function budget also accounts for ownership
summary construction, allocation-to-statement matching, and CFG fixed points.
The default caps are 2 seconds of process CPU time/2,000,000 work units per TU
and 100 ms of process CPU time/250,000 work units per analyzed function.
Descheduling and I/O waits do not consume the CPU quota, so parallel builds do
not lose coverage merely because another compiler process ran first. These are
last-resort anti-timeout guards, not proof shortcuts: exhaustion can reduce
recall but cannot create a finding.

A narrow source-body theorem also records the selector condition under which
a conditional deallocator preserves its pointer argument. The leak query
must satisfy that condition together with the caller's path constraints.
It accepts exact literals or independent public-entry integral parameters,
rejects opaque extra effects and loops, and retains unknown outcomes when
the proof is unavailable. A mixed `live|freed` summary alone never selects
a preserving outcome.

Standard-library models check exact declarations and ABI types. Visible
implementations, including aliases, override library models. Output models
use an ordinary-stdout profile that excludes hidden runtime interposition
and is disabled for observed custom streams, formatters, stdout mutation,
or assembly. The JSON report records this profile and its scope; detection
counts are conditional on those library assumptions.

MemFix uses SAT differently: exact cover and Partial MaxSAT select repair
locations. That is useful for a future repair stage, but cannot make inaccurate
alias facts correct.

## Tests and reports

`make` builds the plugin and runs the reviewed assertion suite:

```sh
make
# or
make test
```

Every case runs in a fresh compiler process and is checked four ways:

- stable GCC diagnostic IDs;
- counts and, where declared, exact primary/trace-role source lines in the
  plugin JSON report;
- exact separation of findings from expected inconclusive reason counts;
- successful generation of the per-case HTML report (plus one explicit
  JSON-only output-mode test).

Aggregate results are written to:

```text
artifacts/tests/report.html
artifacts/tests/report.json
```

The 150-case suite covers direct and negative leak/double-free/UAF cases,
derived pointers, PHIs/copies, dangling returns in C and C++, pthread release,
allocation epochs in loops, constant and runtime-bound counted-loop proofs,
exact one-trip unfolding, total body-bound constant-return summaries,
correlated guards refined by SMT, source-derived conditional borrow guards,
C++ pointer/integer provenance, opaque
ownership transfer, callee may-dereference abstention, user-named deallocator
contracts, JSON-only reports, and bounded high-fanout expansion. It also
contains a 30-diamond CFG with over one billion theoretical paths.
`make path-scaling-smoke` asserts that this case really issues solver queries,
collapses all 30 neutral diamonds, stays below 65 symbolic states and 9 solver
checks, does not truncate, and still diagnoses a feasible twin.

Additional stress targets verify the two independent anti-explosion layers:

```sh
make gimple-value-smoke
make feasibility-budget-smoke
make analysis-budget-smoke
make analysis-budget-cpu-smoke
```

## Benchmark corpus

Run all 100 reduced Binutils/Git/OpenSSH/OpenSSL/Tmux model programs:

```sh
make benchmarks-extracted
```

Run those plus all 4,880 discovered Juliet CWE401/CWE415/CWE416 translation
units:

```sh
make benchmarks-all
```

The harness first compiles each source without the plugin, then with a
content-addressed plugin snapshot. It distinguishes source `BUILD_FAIL` from
`PLUGIN_CRASH` and writes aggregate HTML/JSON plus isolated logs. `PASS` here
means build/plugin robustness, not diagnostic precision or recall. See
[benchmark-tests.md](docs/benchmark-tests.md).

The original hand-written benchmark Makefile is retained as
`Test/Makefile.legacy`; `make legacy-test` is a compatibility target only.

## Compare with native GCC 16.2 `-fanalyzer`

Build the plugin with GCC 16.2 headers and load it into that exact compiler:

```sh
make compare-gcc16
```

This small reviewed suite is useful for regression diagnosis. Its checked-in
expectations measure regression agreement only; TP/FP/TN/FN-style summaries
from this suite are not independent accuracy evidence. The formal
accuracy/false-positive comparison uses the bundled official Juliet manifest:

```sh
make compare-gcc16-juliet
```

That target runs the supported C accuracy/coverage cohort and may use parallel
workers. Use the serial timing target repeatedly on an otherwise idle host for
release-quality configured-cost evidence, and keep C++ visibly experimental:

```sh
make compare-gcc16-juliet-timing JULIET_TIMING_RUN=1
make compare-gcc16-juliet-timing JULIET_TIMING_RUN=2
make compare-gcc16-juliet-timing JULIET_TIMING_RUN=3
make compare-gcc16-juliet-cxx
```

Across the C and experimental C++ targets, the oracle contains 3,079 testcase
groups evaluated as paired programs: bad is compiled with `-DOMITGOOD`, good
with `-DOMITBAD`, and multi-translation-unit testcases stay grouped. Both sides
use the same GCC 16.2 language driver for each group. Plugin diagnostics
come from its JSON traces; native diagnostics come from GCC SARIF.  The report
contains TP/FP/TN/FN, precision, recall, F1, specificity, false-positive rate,
balanced accuracy, MCC, coverage, and paired timing.  Exact manifest-site and
same-case detection scores are reported separately.

The C runner selects 1,234 groups; its fair headline uses the subset for which
both bad/good variants completed under both analyzers. GCC 16.2's manual
documents `-fanalyzer` as suitable only for C in that release. The separate
C++ target evaluates 1,845 groups only as an experimental capability slice.
Coverage and end-to-end recall retain each full selected population. A
separate alert-burden view counts any `SA001`–`SA003` warning on a good variant,
so cross-rule warnings cannot disappear from the false-positive discussion.

To compare robustness and timing on the older 100 extracted models, or on all
discovered source translation units:

```sh
make compare-gcc16-extracted
make compare-gcc16-all
```

The extracted models contain only positive examples, so their target-warning
hit rate is recall-like evidence, not precision or a false-positive measure.
Credit also requires the warning's primary/trace/codeflow evidence to reach a
reviewed allocation line in `tests/extracted_oracle.json`; an unrelated
same-rule warning elsewhere in the file is reported as unlocalized, not a hit.
The corpus runner uses the same optimization flags for plain GCC, the plugin,
and native `-fanalyzer`, rotates their execution order, and reports analyzer
overhead only for three-way-successful translation units.

Select an explicit GCC 16.2 installation with:

```sh
make compare-gcc16-juliet GCC16_PREFIX=/path/to/gcc-16.2
```

The runners reject versions other than 16.2.x and preflight plugin ABI loading;
they never compare a GCC 12 plugin run against a GCC 16 native run. GCC has no
direct counterpart to the plugin pthread rule, so pthread is excluded from the
Juliet memory-lifetime score. See
[juliet-oracle-benchmark.md](docs/juliet-oracle-benchmark.md) and
[gcc16-comparison.md](docs/gcc16-comparison.md). The frozen `1.0.0-rc2`
accuracy, coverage, and three-run timing results are recorded without a winner
claim in [release-validation-1.0.0-rc2.md](docs/release-validation-1.0.0-rc2.md).

## Plugin arguments

Boolean values are `0` or `1`:

| Argument | Default | Purpose |
| --- | ---: | --- |
| `memoryleakcheck` | `1` | enable `SA001` |
| `doublefreecheck` | `1` | enable `SA002` independently |
| `useafterfree` | `0` | enable `SA003` |
| `threadmod` | `1` | enable `SA004` |
| `smt` | `1` | use Z3 when compiled in |
| `report` | `1` | write the machine-readable JSON report |
| `reporthtml` | `1` | also render the per-TU HTML report |
| `reportonly` | `0` | retain findings in JSON/HTML without compiler warnings; useful with `-Werror`; keep `report=1` |
| `librarymodel` | `none` | `none` or `ffmpeg`; optional SDK contracts produce separate conditional `MODEL_REVIEW` findings |
| `legacyengine` | `0` | also execute the historical detector |
| `debugmod` | `0` | legacy debug compatibility |
| `debugoutput` | `0` | legacy debug-output compatibility |
| `traceallfucntion` | `0` | legacy traversal compatibility |
| `contextsentive` | `1` | legacy context option compatibility |

`analysispolicy=proof-only` is the default. It requires a complete feasible
witness and keeps unknown results out of compiler diagnostics.
`analysispolicy=exploratory` explicitly restores fail-open candidate reporting
for experiments and recall studies; its output must not be presented as the
high-confidence result set.

`reportpath=<stem>` selects the output stem. The parser rejects unknown
arguments, duplicate arguments, invalid Boolean values, and any policy value
other than the two spellings above.

For formal internal evidence, enable GCC's dump stream:

```sh
/path/to/matching/gcc -fplugin=Compiler/newplugin.so \
  -O1 -fipa-pta -fdump-ipa-all-details -c input.c
```

The `*.static_analyzer*` dump records value-flow edges, points-to sets, alias
classifications, bounded-engine statistics, and SMT verdicts. `make smoke`
verifies these records.

## Current scope

The default detector is currently strongest for intraprocedural C and lowered
C++ lifetime flows. It handles scalar SSA, edge-correlated PHIs, conditional
`realloc` success/failure lifetime epochs, and conservative pointer-load
provenance through Memory SSA. Translation-unit-local, body-bound ownership
summaries cover proved fresh returns, argument returns, release/escape fates,
and callee may-dereference facts, including acyclic wrapper chains. A
proof-only interprocedural UAF still requires invocation-level proof rather
than treating a may-dereference fact as must. Recursive summaries,
unknown ownership transfer, and opaque memory effects remain unknown. Lock
ordering, general external binary ownership certificates, broader Memory SSA
merge reasoning, and repair synthesis remain explicit follow-on stages.

## License

[MIT](LICENSE) © x213212
