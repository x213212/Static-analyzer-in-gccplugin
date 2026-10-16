# Static analyzer architecture

The default engine is a bounded, read-only consumer of GCC's GIMPLE, scalar
SSA, virtual SSA, CFG, points-to, alias, and dump infrastructure. It does not
build a second compiler or copy GCC's private analyzer exploded graph.

The current source tree has one analysis engine. The historical implementation
headers and adapter were removed on 2026-10-17; there is no fallback detector.
Old engine/debug arguments are rejected during plugin initialization.

## Module boundary

```text
Compiler/src/plugin.cc          plugin lifecycle and SIMPLE_IPA_PASS registration
Compiler/src/options.cc         validated one-time argument parsing
Compiler/src/gimple_flow.cc     forward scalar-SSA immediate-use traversal
Compiler/src/gimple_value.cc    backward pointer provenance expansion
Compiler/src/memory_value.cc    virtual-SSA reaching pointer-store resolution
Compiler/src/alias_oracle.cc    GCC alias evidence and proof classification
Compiler/src/edge_alias.cc      allocation-relative PHI-edge ownership proof
Compiler/src/ownership_call.cc  GCC builtin/attribute/new/delete classification
Compiler/src/ownership_summary.cc TU-local body-bound ownership summaries
Compiler/src/provider_transport.cc bounded GCC LTO provider metadata transport
Compiler/src/local_confinement.cc transitive fresh-local pointer confinement
Compiler/src/lifetime_engine.cc finite CFG typestate for allocations and threads
Compiler/src/feasibility.cc     bounded, demand-driven CFG/SMT refinement
Compiler/src/analysis_budget.cc analyzer-wide deterministic/time budgets
Compiler/src/library_model.cc   opt-in SDK call signatures and ownership effects
Compiler/src/resource_protocol.cc bounded local output-resource obligations
Compiler/src/framework_lifecycle.cc registered initializer/cleanup obligations
Compiler/src/report.cc          GCC diagnostics and HTML/JSON reports
Compiler/src/analyzer.cc        modern phase orchestration
mk/*.mk                        separate build/report/test/benchmark tasks
tools/static_analyzer_cc.py     collision-safe project compiler launcher
tools/merge_static_analyzer_reports.py deterministic project report merger
```

The versioned build directory is keyed by the selected G++ executable and full
compiler version. This prevents GCC 10 and GCC 16 objects from being linked
into one plugin. The shared object also records the runtime library directory
selected by that compiler, which is required when a side-by-side compiler
loads the plugin with `dlopen`. Production objects use hidden visibility and a
linker version script exposes only `plugin_init` and
`plugin_is_GPL_compatible`; internal C++ implementation symbols are not part
of the dynamic plugin ABI.

## Optional library resource protocols

The source modules build into the same ABI-matched `newplugin.so`. They are
internal C++ modules, not separately loadable plugin extensions. New SDK
profiles currently require editing and rebuilding the plugin; no public
module ABI or runtime JSON model loader is implemented.

`library_model.cc` classifies SDK calls; `resource_protocol.cc` checks local
output-parameter ownership; `framework_lifecycle.cc` composes registered
initialization and cleanup callbacks. `analyzer.cc` orchestrates these checks,
while `feasibility.cc`, `analysis_budget.cc`, and `report.cc` retain their shared
solver, limits, and output responsibilities. `librarymodel=none` is the default.

The initial `ffmpeg` profile validates actual external function signatures
and assembler identities. Loaded source definitions, weak bindings,
nonmatching declarations, and abnormal call forms reject the call model.
No production rule matches testcase paths, source line numbers, bug IDs,
private resource names, or the filter's registration name. This is an SDK
contract selected by the user, not a theorem about an unavailable binary.

Output-resource checks require a local nonvolatile pointer cell with a
dominating NULL initialization. The initial dictionary model accepts ordinary
nonnull insertion with flags zero; deletion and ownership-taking flags are
outside the model. Returns of ownership, observable pointer publication,
opaque consumers, inline assembly, and possible releases block a local leak
witness. Local cell scans stop at 4,096 statements / 16 resources, CFG walks
at 512 processed or pending states, in addition to the shared CPU/work limits.

The initial filter protocol requires a read-only `AVFilter` registration,
one input pad with `config_props`, and loaded static configuration/cleanup
bodies. Additional initialization callbacks, unknown private-context
consumers, assembly, and oversized bodies reject the protocol. It uses typed
`AVFilterContext.priv` / `AVFilterLink.dst` relationships to match allocated
private fields to their cleanup, regardless of those fields' names. It scans
at most 4,096 registrations and 2,048 statements per callback, and bounds
success-return expansion to eight routes with depth sixteen.

Every profile finding is `MODEL_REVIEW`, confidence `conditional`, with its
contract in `model_scope` and the solver result in `path_verdict`. An
infeasible path suppresses the candidate. An unknown path stays explicitly
unknown; it is never relabeled `PROVEN`. Frozen benchmark measurements must
not silently incorporate these optional reviews. JSON
separates `model_review_total` / `non_model_total`; model reviews are included
in `total` and SA001, so consumers requiring proven findings must filter by
status. HTML does not offer a generic `free()` patch for a library review.

## Pass placement

The analyzer is a `SIMPLE_IPA_PASS` requiring `PROP_ssa`. GCC 10 and GCC 16
both place the unique late-IPA pass named `simdclone` immediately after the
late IPA `pta`, so the plugin registers immediately before `simdclone` instance
1. This selects the post-PTA whole-unit boundary without also matching the
unrelated `pta` nested inside the OpenACC pass group. A per-translation-unit
one-shot guard remains as defense against vendor pipeline changes. Exact
`static_pass_number` values are dump-manager IDs by plugin-registration time
and must not be confused with dump suffixes such as `pta2`.

Every supported GCC release builds its own `.so` from that release's installed
plugin headers and retains `plugin_default_version_check`. The detailed GCC
10/16 source audit is in [gcc16-api-audit.md](gcc16-api-audit.md).

LTO additionally registers an early regular IPA metadata pass before GCC's
`analyzer`/ICF passes. Initial compilation generates bounded body facts; the
diagnostic consumer stays after late PTA. GCC's own declaration streamer
transports a private, unregistered attribute with a versioned `TREE_LIST`
payload. The payload contains the original full function type, assembler
identity and per-formal ownership/access facts, not SSA addresses or report
locations. `TREE_VEC` cannot be used here: stock GCC declaration SCC validation
does not support it in this position. No GCC rebuild or borrowed built-in
LTO section is required.

Metadata is removed before normal optimization, including fat-LTO early IPA,
then reattached only for streaming. Attribute spines are copy-on-write and
payloads are explicitly GC-marked. Records are capped at4096 and keyed by
the exact surviving declaration/node, never transferred by symbol spelling.
Consumers recheck binding, availability, full signature and assembler
identity; weak, interposable, adjusted/thunk/clone and stale targets remain
unknown. A real loaded body takes precedence over an earlier summary.
The initial wire format carries parameter fates/access only, not fresh-return
or exact-return-argument theorems. User source contracts are disabled while
producing these portable body facts.

Load the ABI-matched plugin at **both** LTO source compilation and final link.
A link-only invocation cannot recover a summary never generated by a producer.
Partitioned consumers write distinct `.ltrans-PID.json`/`.html` files, avoiding
cross-process replacement; producer-only processes do not write empty analysis
reports. `tools/merge_static_analyzer_reports.py` can combine a run's report
directory. These reports do not make the entire link a single points-to domain.

## GCC is the alias source of truth

The implementation uses GCC's installed middle-end interfaces:

- `SSA_NAME_PTR_INFO` and `pt_solution` expose points-to evidence.
- `ptr_derefs_may_alias_p` proves no-alias or conservatively returns may-alias.
- `dump_points_to_solution` records the solution in the GCC pass dump.
- `FOR_EACH_IMM_USE_STMT` and `FOR_EACH_IMM_USE_ON_STMT` traverse forward SSA
  uses without manually following GCC's use-list representation.
- `SSA_NAME_DEF_STMT`, typed assignment accessors, and PHI accessors expand
  pointer definitions backward with depth/cycle bounds.
- `gimple_phi_arg_edge`, `gimple_phi_arg_def`, and
  `gimple_phi_arg_location` keep value provenance, control provenance, and
  source provenance on the same incoming edge.
- `get_symbol_constant_value` supplies GCC-approved constants for safe,
  translation-unit-local static symbols used in path predicates; mutable or
  interposable storage remains unknown.
- `walk_stmt_load_store_addr_ops` distinguishes an actual dereference through
  a pointer from an unrelated memory operand on the same statement.

Alias is evidence-graded rather than Boolean:

```text
same SSA value > provenance-backed must alias > may alias > unknown / no alias
```

`ptr_derefs_may_alias_p == true` is never promoted to must-alias. A base
deallocation requires ownership-grade provenance. An interior pointer may
identify the same allocation for use-after-free, but is not accepted as a
valid owning pointer passed to `free`.

A release through a PHI is proved relative to one allocation event.  Starting
at the allocation statement's basic block, `edge_alias.cc` performs a bounded
CFG reachability walk and reads each PHI arm with `gimple_phi_arg_edge` and
`gimple_phi_arg_def`.  Incoming edges that cannot execute after that allocation
are irrelevant; every reachable arm must recursively resolve through only
value-preserving copies/casts to the same non-derived allocation.  A reachable
null/different arm disproves the release, while a cycle, unsupported GIMPLE, or
depth/value/CFG exhaustion remains `unknown`.  Thus a conditional allocation
followed by `free(PHI<NULL, allocation>)` is recognized without incorrectly
accepting `free(PHI<NULL, allocation>)` when both arms remain possible after
the allocation.  If the allocation block and PHI block are in the same CFG
strongly connected component, the proof also remains unknown: a loop backedge
can carry the prior dynamic allocation epoch even when its static defining
statement is identical.

Failure to prove exact release identity does not prove that no release occurs.
Possibly aliasing, unmodeled `free`/`realloc` calls remain barriers in the final
leak witness. They never become definite release events in typestate. A leak
is reported only if a proved path avoids both definite and possible releases;
otherwise unresolved alias/epoch evidence stays inconclusive. Precisely
modeled same-block conditional-release incoming edges are preserved in that
same witness, just like conditional return edges. This matters when a loop
certificate enables a path search that previously stopped at a loop cap.

C++ is analyzed after the frontend has lowered it to GIMPLE.  The ownership
adapter uses GCC's `FUNCTION_DECL` metadata and builtin codes for ordinary
`operator new`, `operator new[]`, `operator delete`, and `operator delete[]`;
it does not recursively interpret C++ AST syntax or infer ownership from a
demangled spelling.  Placement/custom forms without allocator metadata remain
unknown, because their extra arguments can mean that no new storage was
created.

## Scalar SSA and Memory SSA

Scalar SSA gives directed copy, cast, PHI, conditional, and pointer-offset
edges. A `MEM_REF` address is not the pointer value loaded from that address.
Pointer-valued loads therefore use a separate bounded virtual-SSA query:

1. Initialize an `ao_ref` for the load reference.
2. Audit VUSE/VDEF/virtual-PHI predecessors with explicit depth and state caps.
3. Use GCC's `walk_aliased_vdefs` to enumerate reaching clobbers.
4. Classify stores with `stmt_may_clobber_ref_p_1` and the stronger
   `stmt_kills_ref_p`/exact-reference evidence.
5. Return a value only when every reaching path has one unique, direct,
   must-quality pointer store and no entry, call, asm, volatile, cycle, or
   resource-limit ambiguity.
6. Resume immediate-use expansion from the loaded SSA name.

Results are `complete`, `ambiguous`, or `incomplete`; only `complete` enters a
must-alias proof. Each pointer load is resolved once per function and reused
across allocation sites. Top-level pointer provenance is memoized for the
duration of that function, and one recursive expansion memoizes the shared SSA
sub-DAG by `(tree, remaining depth, derived-pointer)`. Active-cycle-tainted
answers are never cached as ordinary results. Node, work, origin, local-memo,
and function-cache caps append an explicit unknown origin and set
`complete=false` on exhaustion. The local and function-level provenance
tables are hashed by exact GCC tree identity plus remaining-depth/derived
state; their finite caps and proof semantics are unchanged.

Local pointer stores are not automatically ownership escapes. A separate
whole-function confinement theorem can prove that fresh objects created in
that function are never released or exported through local containers. It
checks every body statement and follows actually bound pointer/aggregate-bearing
callees transitively, with memoized results and fixed call/type/work limits.
Indirect calls, releases, nonlocal pointer/aggregate stores, pointer returns,
representation serialization/type erasure, unknown bodies, recursive call
cycles and budget failures reject the theorem. This is NOT a borrow contract
for arbitrary incoming pointers or a must-alias fact for a pointee. A borrow
of `&p` alone never proves a borrow of `p`.

## Translation-unit ownership summaries

Before diagnostics, `ownership_summary.cc` builds summaries for body-bound
functions in the current translation unit. It computes the finite normal-exit
fates of each pointer parameter (`live`, `freed`, `escaped`, `unknown`) and
classifies return alternatives as fresh ownership, `NULL`, an exact argument,
or unknown. Call-graph dependencies are solved callee-first; recursive SCCs,
abnormal/EH edges, signature mismatches, missing bodies, unsupported GIMPLE,
and any bound exhaustion stay unresolved.

A caller therefore treats a wrapper as an allocation only when every modeled
normal return is proved fresh-owned in proof-only mode. A callee establishes a
release only when its parameter fate is exactly `freed`; a mixed
`live|freed` result is never collapsed to must-free. These summaries are
local to the GIMPLE bodies currently available to the GCC pass. Ordinary
compilation therefore has no separately compiled ownership knowledge. In an
explicit LTO project link with `-flto-partition=none`, genuine provider bodies
can be visible and queries still require GCC's actual call-edge binding proof;
there is no name-based external summary import. Summary construction runs only
when at least one memory rule is
enabled; a thread-only invocation resets/skips it, and an invocation with all
modern rules disabled skips the modern engine.

Calls also consume GCC-maintained flags, exact builtin identities and, with
GCC 16, `attr_fnspec`. Ownership effects are indexed by the actual operand:
`memcpy(owned, literal, n)` cannot borrow alias evidence from another operand.
`noescape` alone is insufficient to prove no deallocation (`free` has that
property); only stronger evidence is accepted. Access permissions are distinct
from mandatory access. Exact returned-argument facts from GCC
`gimple_call_return_flags` or bound summaries extend both the immediate-use
walk and backward value expansion. An otherwise borrowing/pure call with an
unmodeled used result can still return ownership, including an integer
representation, so it remains unknown. Pointer-to-integer casts also suspend
leak proof where the pointer walker cannot follow the representation.

Body-derived access guarantees use an access cut over the real CFG. GCC's
immediate-use expansion identifies candidate sites, while
`walk_stmt_load_store_addr_ops` and `get_ref_base_and_extent_hwi` establish an
actual positive-width access through an exact formal pointer. CLOBBERs,
address-only uses, zero-width transfers and unproved TARGET_MEM_REF addresses
are not events. Mixed PHIs do not become exact simply because one incoming
witness names the formal. Both may/must dimensions reuse one SSA expansion.
The access-avoiding prefix graph must be acyclic and every path must hit an
event. Unknown conditions retain all successors; only an exact formal/null
comparison can be pruned under a nonnull premise. A pre-access opaque call,
terminal bypass or budget exhaustion leaves access unknown. Nested guaranteed
callee accesses compose in callee-first summary order, with `body` provenance.
This requires no per-function name rule or explicit contract.

Explicit declaration attributes `static_analyzer_borrow(i)`,
`static_analyzer_must_access(i)` and
`static_analyzer_access_if_nonnull(i)` supply trusted contracts. Borrowing
precludes release and capture except through the return value; return alias
needs independent proof. Indices are 1-based written source parameters,
excluding C++ `this`. Constructors/destructors and other unmodeled artificial
layouts are rejected. These contracts are axioms, not verified function-body
facts. Direct contract-dependent findings retain `user-contract` trace roles.
The nonnull requirement is encoded in the same solver path as the allocation,
release, use and PHI witness. A missing borrow promise never follows from an
access promise. An unloaded external provider needs a validated LTO body
summary or an explicit user contract; an ordinary declaration is insufficient.
Loaded source-defined stdio wrappers can also compose exact GCC builtin
valid-string entry requirements. Those requirements do not establish that
bytes are physically read before every I/O failure, or that stdio cannot
invoke callbacks which free or retain the source buffer.

Conditional borrow refinement retains the original mixed ownership summary.
A separate certificate requires an exactly bound original void-function
body, a single zero/nonzero guard on an entry parameter, one builtin release
of the exact pointer parameter, ordinary acyclic control flow, and no other
effects except pointer-preserving copies. The preserving guard is projected
to a literal or independent integral parameter at a public caller entry.
Private arbitrary inputs, opaque calls, loops, mismatched signatures,
aliases/clones, and defined replacements of `free` reject this certificate.
The avoiding-exit query may remove only certified call barriers and must
satisfy every projected guard together with caller constraints. Contradictory
guards prune the witness; UNKNOWN never becomes a leak. Certificates are
cached by loaded body/signature identity and remain within the existing
summary and function budgets.

Feasibility also has a smaller call-result theorem used only while encoding a
branch condition. A body-bound, non-interposable integral callee can be
replaced by a constant only after an exhaustive reachable-CFG proof shows a
call-free ordinary-edge DAG in which every maximal path reaches synthetic
EXIT through a normal `GIMPLE_RETURN` of the same exact precision, signedness,
and integer value. Calls, asm, volatile operations, cycles, fake/EH/abnormal
edges, non-returning sinks, exceptions, and unsupported statements reject the
summary. A budget interruption is not cached as a permanent rejection.

## Lifetime and path feasibility

Each allocation is solved independently over a five-bit typestate set
(`unseen`, `live`, `freed`, `escaped`, and conditional `reallocated`), while a
definitely joinable pthread handle uses its own four-bit state set. Basic-block
input states are joined and a
worklist runs to a fixed point, so loops and diamonds do not enumerate complete
paths. Allocation sites act as lifetime epochs; release, realloc, escape, and
use transfer functions update finite state. The ordinary block-only solver
uses a GCC basic-block-index vector when indices are bounded and suitably
dense (at most 262,144 slots and no more than
`live_blocks * 8 + 256`), with a sparse map fallback. Only conditional PHI
releases and PHI ownership returns retain the more expensive incoming-edge-keyed
state. Leak feasibility also excludes the exact edges that transfer the
allocation through a PHI return; neutral loop/diamond abstraction cannot skip
these edges. A NULL return alternative alone is not proof of a leak.

Returning the exact allocation after a proved release is modeled as a
dangling-pointer use/escape under `SA003`. The allocation, selected PHI arm,
release, return, and path constraints form one feasibility witness. When the
final `GIMPLE_RETURN` is artificial, the diagnostic location comes from that
same arm's `gimple_phi_arg_location`; the analyzer does not synthesize a line
number.

Potential double-free, use-after-free, and leak-path findings are refined only
on demand. The feasibility layer first applies CFG reachability and reverse
reachability, then optionally sends supported branch predicates to an
incremental Z3 solver with strict query, state, and time limits. One Z3
context is constructed lazily per translation unit, while every query keeps a
fresh solver and symbol map; unsupported or inexact predicates do not
construct a solver. Z3 remains directly linked when compiled in, so this is
not lazy shared-library loading. Under the
default proof-only contract, only `FEASIBLE` with a complete supported witness
can emit a finding. `INFEASIBLE` discards the candidate; timeout, unsupported
GIMPLE, missing/disabled Z3, stale CFG evidence, and bounded-loop/resource
exhaustion become report-only `INCONCLUSIVE` records. The explicit
`analysispolicy=exploratory` mode may retain unknown candidates for recall
experiments.

The symbolic traversal applies three proof-preserving reductions before it
forks:

- A two-way `GIMPLE_COND` whose empty or one-block event-neutral arms rejoin at
  the same block is collapsed when all edges are ordinary and no required PHI
  edge is crossed.
- A canonical one-block event-neutral unsigned counted loop is collapsed only
  after its unique normal exit is proved reachable without modular wrap. The
  dynamic-bound theorem is intentionally limited to an invariant same-width
  unsigned `i = C; i < B; ++i` recurrence.
- A canonical eventful loop is unfolded exactly once only when GCC PHI values
  and `wide_int` arithmetic prove that the first guard enters the body and the
  latch value makes the second guard exit. Its real body statements and
  provenance edges are still traversed.

Fake, EH, abnormal, volatile, wrapping, loop-carried-bound, unsupported, or
larger shapes retain the bounded search. Hitting the visit cap marks that
branch unknown but does not stop an exact sibling branch, so one difficult
path cannot starve a separate proof.

## Soundness and resource caps

Every potentially lossy traversal has a finite default bound. Hitting a proof
bound makes the affected evidence unknown or incomplete; proof-only mode must
not turn that incomplete evidence into a positive diagnostic. Pure cache caps
only stop retaining new cache entries and do not change the computed evidence.
The `1.0.0-rc2` defaults are:

| Layer | Default caps | Exhaustion behavior |
| --- | --- | --- |
| Forward scalar-SSA uses | depth 64; 10,000 reachable values; 4,096 use sites; 32,768 work items; 8 distinct witnesses per value | Marks the expansion or its witnesses truncated. |
| Backward pointer provenance | depth 64; 4,096 nodes; 32,768 work items; 128 origins; 2,048 local memo entries; 2,048 top-level function-cache entries | Appends an unknown origin and sets `complete=false`. |
| GCC alias-query cache | 8,192 entries | Stops caching new results; the current query is still evaluated. |
| Allocation-relative edge alias | depth 64; 10,000 values; 4,096 CFG blocks | Returns unknown and cannot establish an owning release. |
| Memory SSA | depth 64; 256 virtual-SSA states | Returns an incomplete/ambiguous load result. |
| Ownership summaries | 256 return alternatives; 256 parameters; 10,000 blocks; parameter solver worklist limited to `32 * blocks + 128` steps | Leaves the body-bound summary unresolved. |
| Body-bound integer-return summary | 512 cached callees; 1,024 reachable blocks; 8,192 statements; 4,096 edges | Rejects that predicate expansion; a budget-aborted scan is not cached. |
| Conditional-release refinement | 256 PHI arguments; 4,096 recorded conditional releases | Skips that bounded conditional-release refinement. |
| Conditional-borrow certificates | 256 cached callees; 8 CFG blocks; 8 parameters; 64 statements; 32 caller guards, included in the query's existing 256-constraint cap | Keeps the original ambiguous call barrier. |
| Feasibility, per query | 8,192 CFG product states; 4,096 symbolic states; 2,048 solver checks; 256 constraints; 2 visits per block; 25 ms per solver check; 100 ms elapsed per query | Returns `unknown`; proof-only records an inconclusive reason. |
| Feasibility, per translation unit | 256 queries; 262,144 CFG states; 65,536 symbolic states; 32,768 solver checks; 2,000 ms elapsed | All later affected queries return `unknown`. |
| Analyzer-wide budget | 2,000 ms process CPU and 2,000,000 work units per TU; 100 ms process CPU and 250,000 work units per analyzed function; 4,096 summary nodes | Exhaustion is sticky for the affected scope and records bounded `INCONCLUSIVE` coverage. |
| Inconclusive report storage | 64 serialized samples; 4,096 deduplication keys per TU | Counts omitted samples; dedupe saturation makes the reported aggregate an explicit lower bound. |

The analyzer-wide time budget uses the compiler process's CPU clock, not wall
time. Descheduling and I/O waits therefore do not consume it. CPU reads are
sampled every 256 work units and forced at phase/finding gates. By contrast,
the feasibility timeouts in the table use elapsed `steady_clock` deadlines so
one solver query cannot stall compilation indefinitely.

Analyzer-wide exhaustion is serialized with the prefix `analysis-budget-` and
one of `tu-cpu-time`, `function-cpu-time`, `tu-work`, `function-work`, or
`summary-node-limit`. The current stable `INCONCLUSIVE` reason vocabulary also
includes:

- feasibility: `path-feasibility-timeout`, `path-feasibility-budget`,
  `unsupported-path-condition`, `solver-unavailable`, `solver-disabled`, and
  `path-feasibility-unknown`;
- value and ownership evidence: `value-flow-truncated`,
  `value-flow-witnesses-truncated`, `inline-asm-ownership-unknown`,
  `reallocation-status-unproven`, `ambiguous-callee-ownership`,
  `opaque-ownership-transfer`, `conditional-summary-allocation`,
  `allocator-release-contract-unproven`, `callee-dereference-not-proven`, and
  `opaque-call-dereference`;
- pthread evidence: `thread-create-success-unproven`;
- defensive empty-reason fallback: `analysis-unknown`.

## Diagnostics, reports, and dumps

Compilation uses stable `SA001`–`SA004` rule IDs. SA001–SA003 can emit
proof-only GCC warnings; SA004 remains report-only inconclusive in that policy
until `pthread_create`'s successful return is correlated with the handle (its
finite typestate is still exercised, and exploratory mode may emit it). At
`PLUGIN_FINISH_UNIT`, the report layer writes the machine-readable JSON report
and, by default, its HTML view with source locations, counts, engine metadata,
allocation/release trace points, confidence status, and rule-specific manual
remediation. `reporthtml=0` suppresses only the HTML rendering and preserves the
JSON contract; corpus comparison runners use this JSON-only mode to avoid
charging optional HTML rendering to the plugin measurement. Core proof-only
findings are marked
`PROVEN/high`; exploratory output is visibly marked
`EXPLORATORY/uncertain`. Opt-in SDK reviews retain their separate
`MODEL_REVIEW/conditional` status even under the proof-only core policy.
`automatic_patch=false` is explicit until the
separately designed MemFix-style repair pipeline exists.
Finding counts and inconclusive counts are separate. Serialized inconclusive
samples are capped at 64 and deduplication keys at 4096 per translation unit
to bound memory and report I/O. If the latter cap is reached, JSON sets
`dedupe_saturated` and reports `events_after_dedupe_cap`; the aggregate is then
an explicit lower bound rather than pretending to be complete.

By default the report stem comes from GCC's resolved dump/output name, and
files are published with atomic renames. For project builds,
`tools/static_analyzer_cc.py` injects the fixed proof-only option set only into
unambiguous single-TU driver commands and assigns a SHA-256-derived report
stem. `tools/merge_static_analyzer_reports.py` reads those bounded per-TU JSON
documents, deduplicates deterministically while retaining provenance, and
writes one self-contained project HTML plus JSON report. Neither tool merges
inconclusive records into findings.

Proof-only is a confidence policy over this analyzer's finite model, not a
claim that false positives are impossible for arbitrary C/C++. Undecidable
program behavior, undefined behavior, missing translation units, inline
assembly, dynamic linking, and incorrect external contracts remain outside
what a normal GCC plugin can prove.

For formal internal evidence, use GCC's dump stream:

```sh
gcc -fplugin=Compiler/newplugin.so \
  -O1 -fipa-pta -fdump-ipa-all-details -c input.c
```

Dynamic passes are registered after command-line dump parsing, so the
individual plugin pass switch is not reliable; `-fdump-ipa-all-details` is the
supported invocation. Its `*.static_analyzer*` file records value-flow and
memory-flow results, points-to sets, alias classifications, fixed-point
statistics, constant-return decisions, abstracted-loop/diamond counts,
exact-one-trip unfolds, solver construction/check counts, verdicts, and
finding totals. `make smoke` verifies these records.

## Deliberate limits

The default object-only analysis has translation-unit-local body summaries.
Opt-in LTO provider transport carries bounded parameter facts as described
above; it does not supply arbitrary external library bodies. Unknown calls and ownership
protocols, recursion, `setjmp`/`longjmp`, concurrency interleavings, and opaque
memory effects can force an unknown result. GCC 16.2's official analyzer
extension seam can register state
machines and known functions under `-fanalyzer`, but its private exploded graph
is not a stable lifecycle API for a normal plugin pass.
