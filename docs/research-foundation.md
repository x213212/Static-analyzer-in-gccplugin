# Research foundation: the 2022 thesis, MemFix, and this GCC plugin

## Identification verdict

The user-supplied PDF has now been inspected directly.  It is **not** the
MemFix paper.  It is the 104-page master's thesis:

> 伍哲弘. **支援記憶體解除配置之錯誤的靜態分析器**
> (*A Static Analyzer for Memory Deallocation Errors*).  Department of
> Computer Science and Information Engineering, National Chung Cheng
> University, September 2022.  Advisor: Prof. Peng-Sheng Chen.

The PDF metadata says it was created on 2022-08-11 10:51:21 CST, exactly
matching the remembered download filename.  Its SHA-256 is
`8378d546bcfa964985a94decac49825aab9e9a10181538d7c1627f7640c838f4`.
This is strong primary evidence that the thesis describes this repository's
original analyzer, not merely a paper with a similar benchmark.

The thesis explicitly describes the same implementation lineage:

- a GCC plugin running against GIMPLE after `ipa-pta`;
- interested-point discovery for allocations and calls;
- recursive program slicing with `FOR_EACH_IMM_USE_FAST`,
  `SSA_NAME_DEF_STMT`, and a manually constructed `VAR_DECL` table;
- cross-function tags such as "may return heap objects" and "may free heap
  objects", followed through a call stack;
- `ptr_derefs_may_alias_p` to relate actual and formal pointer values;
- CFG successor graphs, DFS, and dominator checks for early exits; and
- leak, double-free, and use-after-free checks based on the collected GIMPLE
  table and free-statement order.

MemFix is still important, but in the correct role.  The thesis cites:

> Junhee Lee, Seongjoon Hong, and Hakjoo Oh. **MemFix: Static
> Analysis-Based Repair of Memory Deallocation Errors for C**. ESEC/FSE
> 2018, pages 95--106. DOI
> [10.1145/3236024.3236079](https://doi.org/10.1145/3236024.3236079).

MemFix is the **benchmark and repair-method source**, not the identity of the
local PDF and not the direct source of the plugin's GCC traversal algorithm.

The strongest evidence is not merely that the subject is similar:

- The thesis and repository README explicitly say that their experiments use the
  [benchmarks provided by MemFix](https://github.com/x213212/Static-analyzer-in-gccplugin/blob/21a6277/README.md#experiment).
- The archived corpus has exactly 100 model programs, 20 from each of
  Binutils, Git, OpenSSH, OpenSSL, and Tmux. This is the exact corpus described
  in the [authors' paper](https://prl.korea.ac.kr/papers/fse18.pdf), including
  the same project split and memory-leak/double-free/use-after-free categories.
- Three archived CIL translations even contain calls named `MemFix_dump()`, a
  direct tool-specific marker in addition to the corpus-level match.
- The repository added a Z3 experiment shortly before the README first named
  MemFix. Solver use is consistent with MemFix's design, although the local
  script is not an implementation of MemFix's exact-cover encoding.

The thesis itself distinguishes the methods: its analyzer uses GCC SSA,
points-to data, program slicing, call-stack traversal, dominators, and DFS;
its related-work section describes MemFix as an Andersen-style alias analysis
plus symbolic/solver-backed repair system.  MemFix reliably explains the
corpus and several research concepts, but it does not explain away the
thesis's original GCC API and GIMPLE implementation choices.

Primary and bibliographic sources:

- [Author-hosted full paper](https://prl.korea.ac.kr/papers/fse18.pdf)
- [Software Analysis Lab publication list](https://prl.korea.ac.kr/publications/)
- [ESEC/FSE 2018 program entry](https://2018.fseconference.org/details/fse-2018-research-papers/27/MemFix-Static-Analysis-Based-Repair-of-Memory-Deallocation-Errors-for-C)
- [Korea University publication record](https://pure.korea.ac.kr/en/publications/memfix-static-analysis-based-repair-of-memory-deallocation-errors/)
- [DBLP record](https://dblp.org/rec/conf/sigsoft/LeeHO18.html)

The old artifact URL referenced by the paper, `prl.korea.ac.kr/MemFix/`, no
longer serves the artifact. The copy committed to this repository is therefore
important provenance evidence.

## Repository evidence and date reconstruction

| Date | Repository evidence | Interpretation |
| --- | --- | --- |
| 2020-12-14 | Commit [`84657a6`](https://github.com/x213212/Static-analyzer-in-gccplugin/commit/84657a6e3ad87f430684533eb695b0fec0fda0cd) already contains immediate-use traversal, SSA definition lookup, and GCC alias queries. | The core GCC traversal predates the explicit MemFix evidence. |
| 2021-03-21 | Commit [`bb14c2d`](https://github.com/x213212/Static-analyzer-in-gccplugin/commit/bb14c2d463f6c245d12615ca22f40c62948593c4) adds `Test/benchmarks (1).tar`, the extracted C/CIL files, and `_spec.json` files. | The MemFix corpus was already present. CIL files include `MemFix_dump()` markers. The tar SHA-256 is `4eb3cf28310195027b59ff6a2d6a0d446e0f596939fa37d1052f70bf6446a0a1`. |
| 2021-08-11 | Commit [`d0739a3`](https://github.com/x213212/Static-analyzer-in-gccplugin/commit/d0739a375beb76de0300de871f929422dccb1ddf) is named `fix trace path by opensource`. | There was benchmark-specific work on **August 11, 2021**. The remembered year may be off by one. |
| 2022-07-07 | Commit [`affd1a8`](https://github.com/x213212/Static-analyzer-in-gccplugin/commit/affd1a8c2b364982309be3aaf5d0ec01b36a5a75) adds `bestfreepaths/bestfreepaths.py` and calls it a symbolic-execution/Z3 experiment. | Strong evidence of solver/path experimentation, but not by itself proof of exact cover. |
| 2022-07-29 | Commit [`d73692a`](https://github.com/x213212/Static-analyzer-in-gccplugin/commit/d73692aa09b92a01d12a615d16e3aaf62e564a5d) adds the explicit MemFix benchmark credit to README. | Direct attribution appears less than two weeks before 2022-08-11. |
| 2022-08-11 | The supplied 104-page thesis PDF metadata records `2022-08-11 10:51:21 CST`; the nearest repository commits are 2022-08-09 and 2022-08-15. | The filename timestamp is genuine PDF creation metadata, even though there is no same-day Git commit. |

The historical `_spec.json` files are also revealing. They identify target
allocation sites and allocator/deallocator pairs, for example an allocation
line using `malloc` with `free` as its deallocator. They are benchmark target
metadata, not expected-patch or expected-diagnostic files.

## What the 2022 thesis actually contributes

The thesis organizes the original detector into three stages:

1. Find allocation/call "interested points" in GIMPLE.
2. Slice outward from each point using SSA use-def links plus a manually
   populated `VAR_DECL` relation table, then classify functions by whether
   they may return or release heap objects.
3. Count/order related frees and use CFG reachability, DFS, and dominators to
   report leaks, double frees, and uses after free.

That architecture contains several durable ideas: allocation-site-centered
slicing, demand-driven analysis, call summaries, and separating a cheap
coarse phase from expensive path checks.  The refactor preserves those ideas
but replaces the fragile parts:

| Thesis-era mechanism | Refactored mechanism |
| --- | --- |
| case-by-case `TREE_CODE` expansion and a global `VAR_DECL` statement bag | typed GIMPLE accessors, scalar SSA immediate uses, PHI-edge witnesses, and bounded provenance |
| treating a successful `ptr_derefs_may_alias_p` query as an alias relationship | `same / must / may / no / unknown`; GCC's `true` remains only may-alias |
| direct `free` count (`0/1/2+`) | allocation-epoch typestate over the CFG |
| DFS/dominator ordering tests | finite event-product reachability plus demand-driven bounded SMT refinement |
| manual memory-tree patterns | Memory SSA `VUSE/VDEF`, `ao_ref`, `walk_aliased_vdefs`, and must-kill evidence |
| user breakpoints to contain analysis size | explicit depth/state/CFG/solver budgets that return `unknown` on exhaustion |
| console/debug output | stable `SA00x` diagnostics, official GCC dumps, and HTML/JSON reports |

The thesis's Juliet table counts 588 selected **bug-bearing variants** and
reports that the plugin detects all 588 versus GCC 12.1 detecting 525.  It does
not compile paired good variants or publish TP/FP/TN/FN, so that table is
recall evidence rather than a valid false-positive or overall-accuracy
comparison.  Its 100 extracted-program table reports 51 detections for the
original plugin, 31 for GCC 12.1, and 37 MemFix repairs.  Those three numbers
measure different outcomes and must not be presented as a single precision
ranking.  The new Juliet oracle harness was added specifically to make the
comparison paired and auditable.

## What MemFix actually does

MemFix is primarily an **automatic program-repair algorithm**, not just a bug
detector or an alias analysis. It takes a whole-program view and conceptually
removes/ignores the original deallocation statements. It then performs two
steps:

1. A disjunctive typestate-style static analysis collects deallocation
   candidates and evidence that candidates are unsafe.
2. An exact-cover SAT encoding chooses a globally compatible set of `free`
   statements. A partial MaxSAT objective prefers fewer and earlier frees.

This distinction matters for the plugin: MemFix's first step contains useful
ideas for reliable detection, while its exact-cover step becomes useful only
when the plugin starts synthesizing fixes.

### Object state and typestate

For each abstract heap object, represented by its allocation site, MemFix
tracks an object state of the form:

```text
<o, may, must, mustNot, patch, patchNot>
```

- `o`: abstract object/allocation site.
- `may`: access paths that may point to `o`.
- `must`: access paths that definitely point to `o`.
- `mustNot`: access paths that definitely do not point to `o`.
- `patch`: program-point/access-path pairs that can safely free `o`.
- `patchNot`: pairs that may be unsafe.

The analysis maintains `must ⊆ may`, `may ∩ mustNot = ∅`, and
`patch ∩ patchNot = ∅`. Access paths include pointer variables,
dereferences, and, in the implementation, structure fields and chains of
dereferences. Their length is bounded so the domain remains finite. These
definitions and invariants are in
[Section 3.2 of the paper](https://prl.korea.ac.kr/papers/fse18.pdf).

This is more informative than a single yes/no alias flag. In particular,
`mustNot` is explicit negative evidence, and uncertainty is not silently
promoted into identity.

### Candidate transfer rules

At a program point `c`, MemFix divides possible candidate frees into three
important sets:

```text
G = {(c, p) | p in must}
U = {(c, p) | p in may \ must}
D = patch ∩ G
```

- `G` is safe positive evidence: `p` definitely names the object.
- `U` is uncertainty: `p` might name the object, so this candidate is placed
  in `patchNot` rather than accepted.
- `D` finds a candidate that can be encountered repeatedly and therefore may
  double-free the object.

When an object may be used at `c`, earlier candidate frees are moved from
`patch` to `patchNot`; otherwise inserting one of those frees could create a
use-after-free. A new safe candidate can be generated after the use. The full
transfer definition also assumes precomputed `mayptsto`, `mayalias`, and
`mustalias` queries.

The crucial lesson for reducing false positives is asymmetric evidence:
**may-alias is sufficient to invalidate a proposed repair, but is not
sufficient to prove that two values are the same allocation.**

### Flow, path, and context sensitivity

| Dimension | MemFix property | Important qualification |
| --- | --- | --- |
| Flow | Flow-sensitive least fixed point over the CFG. | Facts belong to a particular program point. |
| Branch/path | Disjunctive: different object states from different branches remain separate unless the states become equal. | The paper does not attach SMT path predicates or prove every branch condition feasible. It is path-distinguishing, not a general symbolic executor. |
| Main repair analysis | Fully context-sensitive except recursion, using call strings. | Recursion is the explicit exception. |
| May points-to / may-alias pre-analysis | Flow-sensitive, context-insensitive. | The authors identify this context insensitivity as a practical precision limitation. |
| Must-alias pre-analysis | Flow- and context-sensitive. | This stronger proof is what makes a candidate safe. |
| Heap abstraction | Allocation sites plus bounded access paths. | Arrays are collapsed by the implementation's pointer analysis; inductive structures lose precision. |

Calling the method simply "path-sensitive" hides an important distinction.
It preserves alternative branch states, but the paper does not describe an SMT
solver that rejects infeasible branch-condition combinations. The repository's
Z3 experiment is closer to the latter idea.

### Exact cover and extended safety

At the CFG exit, MemFix unions all safe and unsafe evidence:

```text
Candidates = Safe - Unsafe
```

For each candidate `c`, `M(c)` is the set of reachable object states that the
candidate safely deallocates. It chooses a candidate subset `C` such that:

- every reachable object state is covered by some selected candidate, which
  prevents a leak; and
- the `M(c)` sets for selected candidates are pairwise disjoint, which prevents
  the same object state from being freed twice.

That is an exact-cover problem. The paper encodes candidate selection as SAT.
Its implementation adds partial-MaxSAT soft objectives to minimize inserted
frees and prefer earlier deallocation.

MemFix also handles a subtle second-order UAF in generated patches. If a patch
is `free(*x)`, evaluating its argument dereferences the object named by `x`.
The extended formulation computes `U(c)`, the object states used while
evaluating candidate `c`, and forbids selecting `c1` and `c2` when
`M(c1) ∩ U(c2)` is nonempty. This prevents choosing `free(x)` before
`free(*x)` when the latter would dereference freed storage.

### Scope and measured limitations

The authors evaluated 210 selected Juliet cases and reported successful repair
for all 210. They also evaluated exactly the 100 five-family model programs in
this repository and repaired 37:

| Family | MemFix result |
| --- | ---: |
| Binutils | 7/20 |
| Git | 4/20 |
| OpenSSH | 12/20 |
| OpenSSL | 9/20 |
| Tmux | 5/20 |
| Total | 37/100 |

These are **repair success rates**, not detector precision or recall. They
must not be used as a direct baseline for this plugin's warning counts.

The paper identifies the following limitations:

- Repair is restricted to inserting/removing explicit deallocation calls;
  fixes requiring a conditional, temporary value, copy, or ownership-changing
  allocation are outside the repair language.
- `realloc` is unsupported. It can deallocate implicitly and needs conditional
  reasoning.
- Collapsing array elements and using context-insensitive may-points-to facts
  lose too much precision for some examples.
- Function pointers, recursion, linked/inductive structures, and path/state
  explosion limit applicability and scalability.

These limitations explain why blindly reproducing MemFix would not meet the
current goal of robust, low-false-positive GCC diagnostics.

## The repository Z3 experiment is not MemFix exact cover

`bestfreepaths/bestfreepaths.py`, added by commit `affd1a8`, creates Boolean
path conditions (`q1` through `q5`) and checks a series of basic-block option
vectors with `Solver.push()`, `add()`, and `check()`. Unsatisfiable options are
reported by their block records.

It does **not** create:

- one selection variable for each candidate `free`;
- an incidence relation between candidates and reachable object states;
- the "at least one and at most one" constraints needed for exact cover; or
- a MaxSAT objective for minimum/earliest frees.

Therefore the most accurate interpretation is:

- the Python file is an early **path-feasibility filtering** experiment;
- MemFix uses SAT/MaxSAT for **globally safe patch selection**; and
- both can eventually coexist, but one cannot substitute for the other.

## What should strengthen this plugin

The highest-value transfer from MemFix is its evidence discipline and
per-object dataflow, not its solver encoding.

### P0: reliable object identity and diagnostic gating

1. Identify an object by at least `(allocation-site, bounded call context,
   lifetime epoch)`. A new allocation or successful reallocation starts a new
   lifetime even if the same SSA name or address expression is reused.
2. Keep alias evidence multi-valued, for example:
   `same-value > must-alias > may-alias > unknown/no-alias`.
3. Treat GCC's conservative `ptr_derefs_may_alias_p` result as may/no evidence,
   never as a must-alias proof. Only identical SSA/provenance or other
   must-quality evidence should directly match an allocation with a `free` for
   high-confidence double-free/UAF warnings.
4. Preserve negative evidence: no-alias, must-not, escaped, clobbered, and
   unknown should affect confidence instead of disappearing during recursive
   expansion.
5. Require feasible CFG ordering for diagnostics:
   allocation → free → use/free, or allocation → exit for leaks.
   Alias overlap alone is not an error trace.

This is consistent with the current architecture's
[alias evidence ordering](architecture.md#gcc-is-the-alias-source-of-truth).

### P1: replace statement-bag recursion with object dataflow

1. Run a worklist/fixed-point analysis over CFG edges for each abstract object.
   A useful detector typestate is `Allocated`, `Freed`, `MaybeFreed`, and
   `Escaped/Unknown`, with bounded disjunction at PHIs and branch joins.
2. Use GCC's typed GIMPLE and SSA APIs for register/value flow. Use Memory SSA
   plus `ao_ref`, `walk_aliased_vdefs`, or related GCC walkers for indirect
   stores, loads, fields, and memory clobbers. Immediate SSA uses alone do not
   recover values stored through memory.
3. Add call summaries for allocation, deallocation, ownership transfer,
   borrowed arguments, escape, and return values. Key summaries by call site or
   bounded call string; solve recursion by fixed point/widening rather than
   unlimited recursive expansion.
4. Normalize `MEM_REF`, `COMPONENT_REF`, `ARRAY_REF`, casts, pointer arithmetic,
   and PHIs into bounded access paths. Retain constant fields and indices/ranges
   when GCC can distinguish them.
5. Model `realloc` as a conditional lifetime transition: on failure the old
   object remains allocated; on success the old lifetime ends and the returned
   pointer names a new lifetime. This intentionally goes beyond MemFix.

### P1: path feasibility without solver explosion

Use dominators, post-dominators, reachability, edge predicates, and constant
folding first. Invoke SMT only for ambiguous, diagnostic-relevant paths, such
as proving that an allocation-to-free-to-use trace is satisfiable. Keep a
strict timeout and return `unknown`; never convert timeout/unknown into a
high-confidence warning.

The historical Z3 experiment is a useful seed for this layer. Its output needs
to be connected to concrete CFG edges and object-state events, rather than a
hand-written vector of block flags.

### P2: exact cover only when auto-repair exists

Once detection and object identity are stable, an optional repair stage can
adopt MemFix's candidate matrix:

1. Generate candidates only from must-quality object identities.
2. Record may-only conflicts, later uses, repeated execution, escape, and
   argument-dereference dependencies as negative constraints.
3. Solve exact-one coverage of reachable lifetimes.
4. Optimize with MaxSAT for minimal edits, early release, and perhaps source
   locality.
5. Validate a proposed patch by rerunning the analyzer on the transformed IR or
   source before presenting it.

Exact cover does not itself improve alias truth. Feeding it imprecise alias
facts only makes a globally consistent answer to the wrong model.

### Separate generalized-resource typestates

The allocation/free machinery can later be generalized to resources such as
locks, files, and threads, but their transition systems differ. In particular,
lock correctness needs acquire/release balance, ownership, nesting/order, and
inter-thread semantics. MemFix makes no claim about pthread or lock analysis,
so it should not be cited as the basis for those rules.

## Related methods that can strengthen the redesign

The supplied PDF settles the provenance question: it is the project's 2022
thesis.  The following papers are therefore not alternative identities for the
PDF; they are technically useful comparison points for strengthening the
implementation beyond the thesis-era statement-table algorithm.

| Candidate | Why it resembles this repository | Why it is not a stronger provenance match |
| --- | --- | --- |
| [Predator: A Verification Tool for Programs with Dynamic Linked Data Structures](https://www.fit.vut.cz/person/vojnar/public/Publications/predator-svcomp-12.pdf), TACAS 2012, [DOI](https://doi.org/10.1007/978-3-642-28756-5_45) | A C++ GCC plugin that consumes low-level GIMPLE, models heap objects/pointers, and reports leaks, invalid dereferences, and double frees. Its [project page](https://www.fit.vut.cz/research/group/verifit/public/tools/predator/) confirms the plugin architecture. | Predator uses separation-logic-inspired symbolic heap graphs and shape analysis. No Predator, Code Listener, symbolic-memory-graph, or separation-logic attribution appears in this repository. |
| [Static Memory Leak Detection Using Full-Sparse Value-Flow Analysis (SABER)](https://yuleisui.github.io/publications/issta12.pdf), ISSTA 2012, [DOI](https://doi.org/10.1145/2338965.2336784) | Tracks allocation-to-free value flow over def-use chains, including address-taken memory, with field/flow/context sensitivity. This is close to the plugin author's description of recursively expanding GIMPLE values. | Implemented in Open64, uses a sparse value-flow graph, and targets leaks rather than the three unified error classes. There is no repository citation. |
| [Spatio-Temporal Context Reduction (CRed)](https://yuleisui.github.io/publications/icse18a.pdf), ICSE 2018, [DOI](https://doi.org/10.1145/3180155.3180178) | Correlates alias (spatial) and feasible free-to-use order (temporal), then uses multi-stage filtering and demand-driven path sensitivity to suppress UAF false alarms. | Implemented in LLVM 3.8, targets UAF specifically, and is not cited by the repository. |
| [SAVER: Scalable, Precise, and Safe Memory-Error Repair](https://prl.korea.ac.kr/papers/icse20.pdf), ICSE 2020, [DOI](https://doi.org/10.1145/3377811.3380323) | MemFix's direct successor; replaces whole-program path/state burden with object-flow graphs, graph labeling, slicing/local analysis, and selective path sensitivity. It is useful for a scalable redesign. | It consumes another analyzer's reports for repair, has a different benchmark set, and the repository names MemFix rather than SAVER. |

The resulting attribution is therefore:

> The supplied thesis is the primary description of this repository's original
> GCC/GIMPLE analyzer.  It explicitly uses MemFix's benchmark corpus and draws
> on MemFix's analysis/repair ideas. Predator remains a useful comparison by
> GCC-plugin architecture, SABER by sparse value flow, and CRed by
> UAF/false-positive refinement.

### GCC documentation may be the direct implementation source

The original API combination may have come from GCC itself rather than a
paper. GCC's official
[SSA operand documentation](https://gcc.gnu.org/onlinedocs/gccint/SSA-Operands.html)
describes virtual SSA operands for aliased memory and demonstrates
`FOR_EACH_IMM_USE_STMT`. The 2019
[GCC static-analyzer RFC](https://gcc.gnu.org/pipermail/gcc-patches/2019-November/534668.html)
describes an IPA pass over GIMPLE SSA with data state machines and explicitly
lists double-free, leaks, and use-after-free. Both predate the repository's
first commit and are at least as plausible as a paper for the basic GCC API
selection.

Other alias-analysis papers, including the must-alias analysis cited by
MemFix, may inform the implementation but have no comparable repository
provenance.

## Practical conclusion

The defensible research lineage is:

```text
MemFix benchmark + object-state discipline
          |
          +--> must/may/must-not alias evidence
          +--> disjunctive CFG typestate
          +--> safe/unsafe candidate separation
          `--> exact cover (future repair stage only)

GCC SSA + points-to + Memory SSA
          |
          `--> implementation-grade value/memory traversal

bounded CFG/SMT feasibility
          |
          `--> suppress infeasible diagnostic traces
```

For the current refactor, object identity, negative alias evidence, Memory SSA,
context-aware summaries, and feasible event order should come before patch
synthesis. That ordering directly targets false positives while retaining a
clear route to future leak/UAF/double-free repair.
