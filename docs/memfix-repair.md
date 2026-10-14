# Planned MemFix-style repair stage

> **Implementation status: planned.** The plugin currently detects and reports
> memory-lifetime defects; it does not yet synthesize, apply, or verify source
> repairs. Its in-process Z3 integration is used for path feasibility, not for
> MemFix exact-cover selection. Nothing in this document should be read as a
> claim that automatic repair already exists.

## Purpose and boundary

MemFix is an automatic repair method, not merely a detector. Conceptually it
ignores the program's original deallocation statements, computes all safe and
unsafe deallocation candidates, and chooses a globally compatible set. The
chosen set can insert a missing deallocation, omit a redundant deallocation,
or move one by combining an omission with an insertion.

This repository can adopt that repair model only after a proof-only finding
has been produced. Exact cover does not improve pointer analysis and must
never turn `may alias` into object identity.

The historical `bestfreepaths/bestfreepaths.py` is not a MemFix solver. It
checks several hand-written Boolean path vectors against hand-written path
conditions. It has no candidate-selection variables, object-state incidence
matrix, exact-one coverage constraints, extended-use conflicts, or MaxSAT
objective.

## Staged implementation

The implementation should be split so that an early useful result does not
pretend to be a safe source rewriter.

### Stage 1: report-only repair planning (MVP)

The first implementation computes a bounded exact-cover plan and shows the
semantic edits in HTML/JSON and compiler notes. It does **not** modify GIMPLE,
write a source patch, or claim that a displayed snippet is directly
applicable.

The MVP is restricted to:

- C code in one function and one translation unit;
- direct GCC-recognized `malloc`, `calloc`, `aligned_alloc`, `strdup`, or
  `strndup` allocation paired with the builtin `free` contract;
- a stable, non-derived owner represented by a source `VAR_DECL` or
  `PARM_DECL`;
- a complete, acyclic object-flow slice with no exhausted bound;
- ordinary normal CFG edges and direct calls; and
- proof-only SA001, SA002, or SA003 findings.

It supports these semantic transformations:

| Finding | Planned transformation |
| --- | --- |
| SA001 memory leak | Insert one or more `free(owner);` operations after the final uses and before the relevant exits. |
| SA002 double free | Retain exactly one selected release and remove an original redundant release. |
| SA003 use after free | Remove an early release and insert a release after the last required use; this is one atomic move plan. |
| SA004 thread leak | No MemFix repair. Thread lifecycle requires a separate resource protocol. |

An MVP plan is reported as `PLAN_ONLY`. It may show, for example, "insert
`free(p);` before the return represented by this GIMPLE/source location", but
must not contain byte edits or GCC fix-it hints.

### Stage 2: source patch artifact

After source-range capture and round-trip checks exist, the same selected plan
may produce a unified diff and GCC fix-it hints. A patch is still not applied
to the user's file. A plan may become `STATICALLY_CERTIFIED` only when both its
memory-safety proof and every source edit are complete.

### Stage 3: verified application

An external driver applies the patch to a temporary copy, recompiles it with
the same compiler arguments, reruns proof-only analysis, and only then offers
an atomic in-place update. Only this stage may report `REBUILD_VERIFIED`.

## Explicitly unsupported repairs

The repair language only inserts or removes deallocation operations. It does
not synthesize conditions or temporaries, rewrite allocations, delete uses,
or assign `NULL` to pointers.

The following cases are excluded from the MVP and cannot produce a certified
repair:

- `realloc`, including apparently guarded calls: success ends the old
  lifetime, failure preserves it, and a correct repair can require a new
  conditional;
- returned, globally stored, shared, or otherwise escaped ownership;
- unresolved or indirect calls, opaque ownership transfer, recursion,
  assembly, `setjmp`/`longjmp`, EH, abnormal/fake CFG edges, or concurrency;
- `may alias`, unknown alias, an interior pointer, incomplete Memory SSA, or
  truncated scalar/provenance traversal;
- allocation/release candidates in a CFG SCC where one static edit may execute
  more than once for an allocation epoch;
- custom allocator/deallocator spelling conventions such as `xmalloc` or
  `xfree` without a proved contract;
- C++ `new`, `new[]`, `delete`, and `delete[]` until source new-expression,
  array form, complete type, and destructor semantics are preserved; and
- macros, system headers, generated source, or edits whose source range is not
  unique.

Later versions may accept a unary deallocator named by an explicit GCC
`malloc(DEALLOCATOR, INDEX)` contract or a complete TU-local body summary.
Removal of a custom deallocator additionally requires proof that eliminating
the call does not discard unrelated observable behavior.

## Repair state and certificate gates

The current five-bit joined lifetime state is suitable for bounded detection,
but it is not an exact-cover incidence matrix. A repair-specific, bounded
disjunctive analysis must retain states equivalent to:

```text
<object, may, must, mustNot, patch, patchNot>
```

An object identity includes at least its allocation site, bounded call
context, and lifetime epoch. Candidate generation is asymmetric:

- same SSA value or must-quality base provenance may add a candidate to
  `patch`;
- any may-only alias, possible later use, repeated execution, unsafe argument
  evaluation, or uncertain ownership adds it to `patchNot`; and
- `Candidates = Safe - Unsafe` is computed globally, not separately per
  warning.

A repair certificate requires every gate below:

1. The originating finding is proof-only and has a complete feasible witness.
2. Allocation success/failure and the matching deallocator contract are
   modeled.
3. Scalar SSA, PHI-edge provenance, Memory SSA, call summaries, and CFG
   traversal are complete and within their bounds.
4. Every selected deallocation argument is a non-derived base pointer that
   must name the object in every state it covers.
5. No relevant ownership escapes through a return, store, unknown call, or
   shared state.
6. All normal exits and all relevant lifetime epochs are represented; no
   unsupported control transition has been discarded.
7. Candidate safety and unsafety sets are complete, including uses introduced
   by evaluating another candidate's pointer expression.
8. Z3 returns `sat`, not `unknown`, and the selected model passes an
   independent C++ matrix checker.
9. For a source patch, every edit has a unique non-macro source range, edits do
   not overlap, and the source-content hash matches.
10. For an applied repair, the patched copy rebuilds and its touched scope has
    neither a new finding nor a new inconclusive result.

The certificate is evidence relative to this explicitly bounded C/GIMPLE
model. It is not a mathematical claim about arbitrary C programs, undefined
behavior, dynamic linking, or unobserved translation units.

## Exact-cover and extended-safety encoding

Let:

- `Rexit` be the reachable terminal object states that still require a
  deallocation;
- `Ci` be one canonical candidate `(program point, access path,
  deallocator)`;
- `Si` be the Boolean selecting `Ci`;
- `M(Ci)` be the states safely deallocated by `Ci`; and
- `U(Ci)` be the states whose objects may be used while evaluating `Ci`'s
  access path.

For each terminal state, exact-one coverage is required:

```text
for every r in Rexit:
    sum(Si for which r is in M(Ci)) == 1
```

This combines MemFix's at-least-one leak constraint with its at-most-one
double-free constraint. Extended safety adds:

```text
if M(Ci) intersects U(Ck):
    not Si or not Sk
```

Candidates in the global unsafe set are fixed false. Physical source edits
are canonicalized before solving: two logical candidates that would create
the same source edit share one selection variable, and overlapping alternative
edits are mutually exclusive.

An original editable `free` is represented by a candidate at its current
location:

```text
selected original candidate   -> keep it, edit cost 0
unselected original candidate -> remove it, edit cost 1
selected new candidate        -> insert it, edit cost 1
```

The hard constraints are ordinary SAT/pseudo-Boolean constraints. Selection
quality uses lexicographic optimization:

1. minimize changed source statements;
2. minimize newly inserted releases;
3. maximize early-release benefit; and
4. minimize a stable source-location tie-breaker.

The installed Z3 4.8.12 C++ API supports this through `z3::optimize`, integer
`z3::ite` sums, `minimize`, and `maximize`. As in `feasibility.cc`, the GCC
`optimize` macro must be undefined before including `z3++.h`. The solver must
receive a timeout through `z3::params`. This version has no convenient
`z3::optimize::reason_unknown()` method, so an unknown result is recorded with
a stable generic reason and never yields a plan.

Using lexicographic integer objectives is preferable to relying on newer
grouped `add_soft` overloads that are absent from Z3 4.8.12. It implements the
same Partial-MaxSAT policy while retaining GCC 10/16 build compatibility.

## Sparse candidate construction

Generating a candidate after every source statement is unnecessary and
expensive. The repair analysis should consider only allocation, use, release,
ownership-transfer, and exit boundaries. Between two such events, choose the
earliest source-mappable point whose `M`, `U`, and edit class are unchanged.

The candidate/state bipartite graph should be decomposed into connected
components and solved independently. Candidates with identical coverage,
conflicts, and edit behavior are equivalent; retain the deterministic
lowest-cost representative.

For the MVP, the candidate access path is a plain source owner variable, so
`U(Ci)` is empty. The `U` machinery is nevertheless part of the model and is
required before dereferenced or field access paths are enabled.

## GIMPLE-to-source mapping

GIMPLE is the source of CFG, SSA, alias, and lifetime evidence, but late
GIMPLE locations alone are not sufficient to rewrite C source safely. They
can refer to a macro expansion, cover only a call token rather than a complete
statement, or combine several lowered statements at one location.

Stage 2 should register the GCC 10/16 `PLUGIN_PRE_GENERICIZE` callback and
capture a compact, read-only source index before lowering:

- function and declaration UIDs;
- complete statement ranges and AST parent kind;
- whether a deallocation call is a standalone full expression;
- callee and argument declarations;
- macro/system-header state; and
- a normalized token fingerprint.

The late repair plan maps a GIMPLE event back with a key such as:

```text
function UID + spelling file/line/column + callee UID + token fingerprint
```

The match must be unique. `get_range_from_loc`,
`from_macro_expansion_at`, and `in_system_header_at` provide the GCC line-map
checks. A source hash guards against edits made after compilation.

Insertion is allowed only at a statement boundary known to be a direct child
of a compound statement, or immediately before the closing brace of a known
compound scope. This avoids changing an unbraced `if`, loop, label, or
`else` association. Removal of a standalone `free(p);` should replace the
full expression with `(void)0;` rather than leave syntactically dangerous
empty text.

Once ranges are proved, `rich_location::add_fixit_insert_before` and
`rich_location::add_fixit_replace` can show GCC-native fix-it hints. The
plugin should also emit its own deterministic unified diff.

The analyzer must not silently mutate GIMPLE as its primary repair workflow.
Doing so would produce a changed object file while leaving the reported source
buggy, and the current whole-unit SIMPLE IPA pass is deliberately read-only.

## Proposed command-line interface

The raw plugin default remains off so ordinary compilation pays no source-map
or exact-cover cost:

```text
-fplugin-arg-newplugin-repairmode=off|suggest|patch
-fplugin-arg-newplugin-repairpath=/path/to/result.memfix.patch
-fplugin-arg-newplugin-repairtimeoutms=250
-fplugin-arg-newplugin-repairmaxedits=16
```

- `off`: no repair state or source index.
- `suggest`: compute plans and include them in diagnostics/HTML/JSON.
- `patch`: Stage 2 only; additionally write a unified diff, without applying
  it.

`make analyze` may explicitly select `suggest` so its report answers "how do I
fix this?" while the general plugin default stays fast. Until Stage 2 exists,
`patch` must be rejected as unsupported rather than silently behaving like
`suggest`.

`patch` is incompatible with exploratory policy, disabled/missing Z3, and the
legacy engine. Invalid combinations should fail option parsing.

Source application belongs to a separate command, for example:

```text
tools/memfix_apply.py --report analysis.json --verify --in-place
```

It must require an explicit in-place flag and a reproducible compiler argv;
the plugin must not launch a recursive compiler itself.

## Proposed report schema

Adding repair data requires report schema version 3. Findings and repair
plans remain separate so an unsupported repair cannot hide a valid finding.
A representative shape is:

```json
{
  "repair": {
    "implementation_status": "planned",
    "mode": "suggest",
    "solver": {
      "backend": "z3",
      "result": "sat",
      "optimal": true,
      "elapsed_ms": 7
    },
    "summary": {
      "plans": 1,
      "certified": 0,
      "abstentions": 0
    },
    "plans": [{
      "repair_id": "MR-...",
      "status": "PLAN_ONLY",
      "finding_ids": ["SA001-..."],
      "semantic_edits": [{
        "operation": "insert_deallocation",
        "deallocator": "free",
        "argument": "p",
        "gimple_anchor": "before-exit"
      }],
      "source_edits": [],
      "certificate": {
        "model_version": 1,
        "alias_policy": "must-only",
        "reachable_states": 3,
        "safe_candidates": 4,
        "unsafe_candidates": 2,
        "exact_cover": true,
        "extended_uaf_constraints": true,
        "budget_exhausted": false,
        "constraint_sha256": "..."
      },
      "verification": {"status": "not-run"}
    }],
    "abstentions": []
  }
}
```

Stage 2 adds byte/source ranges, replacement text, and source SHA-256. HTML
shows one of three unambiguous labels:

- `PLAN ONLY — no source edit produced`;
- `STATICALLY CERTIFIED — not applied`; or
- `REBUILD VERIFIED`.

Unsupported and bounded cases are listed as abstentions with stable reasons
such as `realloc-unsupported`, `ownership-escaped`, `alias-not-must`,
`source-map-ambiguous`, or `repair-budget`.

## Validation workflow

Before emitting even a plan, C++ code independently evaluates the Z3 model:

- every terminal state has exactly one selected covering candidate;
- no unsafe candidate is selected;
- all `M/U` conflicts are respected; and
- physical edits are canonical and non-overlapping.

Stage 2 additionally applies edits to an in-memory or temporary source copy
and verifies its token ranges and source hash.

Stage 3 must:

1. apply the complete edit transaction to a temporary copy;
2. compile with the same compiler, target, optimization, preprocessor, and
   plugin arguments, with repair generation disabled;
3. rerun proof-only lifetime analysis;
4. confirm that the target finding is absent;
5. reject any new finding or touched-scope inconclusive result;
6. confirm that a second repair run proposes no further edit; and
7. atomically replace the requested source only after every check passes.

Project tests and sanitizers are useful additional evidence, but they do not
replace the static certificate.

## Performance limits

Repair runs after detection and has a separate budget. It must not consume
the feasibility budget needed to decide diagnostics. Recommended initial hard
caps are:

```text
repairable findings per translation unit: 32
reachable repair states per translation unit: 512
canonical candidates per translation unit: 512
candidate/state incidence cells: 65,536
extended-safety conflict pairs: 32,768
solver time per connected component: 50 ms
total repair wall time per translation unit: 250 ms
source edits per plan: 16
repair memory per translation unit: 16 MiB
```

Every state/candidate/source-index loop must periodically consult the global
deadline. Exceeding any limit produces an abstention for the affected
component and no partial plan. No solver is invoked for a clean translation
unit or when `repairmode=off`.

These bounds prevent repair from becoming unbounded; they do not imply that
enabled repair has zero runtime cost. Detection-only benchmarks and
repair-enabled benchmarks must therefore be reported separately.

## Test matrix

| Area | Required case and result |
| --- | --- |
| SA001 | One exit inserts one semantic release; two leaking exits require two; a safe common post-dominator prefers one. |
| SA002 | Two releases select one and remove the other; a release that is sole coverage on another path must not be removed. |
| SA003 | Early release plus later use becomes an atomic remove/insert plan; absence of a safe later point abstains. |
| Global cover | Two allocations joined by a PHI are solved together; per-warning independent choices must not conflict. |
| Extended safety | A `free(p)`/`free(*p)` example rejects an ordering whose selected patch dereferences freed storage. |
| Alias | Same-value and must-alias can form `M`; may-alias, derived-pointer, and unknown evidence cannot. |
| Escape/calls | Return, global store, opaque/indirect call, recursive summary, and shared ownership all abstain. |
| Reallocation | Guarded and unguarded `realloc` both report `realloc-unsupported` and produce no plan. |
| Loops | A candidate that can execute twice for one static epoch produces no certified plan. |
| Contracts | GCC builtin allocation/free works; local functions merely named `xmalloc`/`xfree` do not. |
| Source mapping | Macro, system-header, ambiguous location, unbraced control body, overlapping edit, UTF-8 column, and source-hash drift are rejected safely. |
| Solver | SAT, UNSAT, unavailable Z3, disabled SMT, timeout, and every matrix cap have stable results; only SAT can plan. |
| Model checker | A deliberately corrupted selection is rejected independently of Z3. |
| Verification | Rebuild failure, new finding, new inconclusive, and non-idempotent repair roll back; a valid patch reaches `REBUILD_VERIFIED`. |
| Compatibility | The plugin and repair-disabled suite build and pass under GCC 10 and GCC 16.2. |
| Performance | Clean translation units make zero repair solver calls; adversarial matrices stop at the TU deadline without a compiler timeout. |

Golden repair fixtures must contain reviewed expected semantic plans and,
after Stage 2, exact diffs. The historical `_spec.json` files identify
allocation sites and allocator/deallocator pairs; they are not expected-patch
oracles. The paper's 37/100 extracted-program result is a repair-success
measurement, not detector precision and not a promise that every case is
repairable.
