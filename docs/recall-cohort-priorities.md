# Recall cohorts and implementation priorities

## Current Phase-17 project differential — 2026-10-15

The complete same-input C LTO replay detects 1,123 of 1,234 groups, with
target FP0, versus GCC16's 974 / FP48. There are 111 remaining misses:
80 CWE401, 12 CWE415, and 19 CWE416. GCC still detects 55 of these; the
plugin alone detects 204 groups. Case detection does not mean exact primary
location agreement: those counts are plugin293 and GCC398.

Evidence: `artifacts/phase17-conditioned-borrow/final-full-c/comparison.json`.
The expandable remaining-case report is
`artifacts/phase17-conditioned-borrow/triage/triage.html`.

| Observed blocker | Misses | GCC detects |
| --- | ---: | ---: |
| CWE401 global publication, flows45/68: no target candidate | 40 | 0 |
| CWE401 unsupported path condition, flow12 | 20 | 16 |
| CWE416 no target candidate, by-reference flows63/64 | 10 | 8 |
| CWE401 opaque contained-pointer transfer | 8 | 4 |
| CWE401 possible-release alias not proved | 6 | 5 |
| CWE401 possible release plus realloc status | 6 | 5 |
| CWE415 no target candidate, flow17 | 6 | 6 |
| CWE415 unsupported path condition, flow12 | 6 | 6 |
| CWE416 path feasibility budget, flow12 | 4 | 4 |
| CWE416 callee dereference not proved, flow17 | 3 | 1 |
| CWE416 unsupported path condition, flow12 | 2 | 0 |

These disjoint reason sets partition misses; they are observed blockers,
not complete semantic diagnoses. The largest 40-case class stores the
allocation in a global pointer. That differs from SA001's current
function-exit-without-release-or-escape obligation. A still-reachable global
can be a legitimate cache; automatically calling every global store a leak
would change the policy and manufacture warnings. Further coverage needs
an explicit whole-program lifetime/overwrite obligation and provider proof.

Among the 55 GCC-only groups, unsupported random-return conditions are a
larger actionable class than global publication. Opaque stateful `rand()`
results must not be treated as independent exact choices. Other next work
is allocation epochs in eventful loops, exact contained-pointer access, and
strdup/container provenance. Solver limits remain unchanged.

The Phase-17 source guard refinement improves two independent synthetic
sets to 626/626 leaks detected and 0/142 safe-case alarms. It does not cause
a large Juliet jump: Phase-16's 1,122 result lost one case to its function
CPU budget, and that case completes in the Phase-17 replay. This one-case
difference is not evidence of a new semantic recovery theorem.

## Historical Phase-3 same-input project differential

The complete Phase-3 C project run used each testcase's real translation
units plus `testcasesupport/io.c`, with no runner-injected contracts:
`artifacts/phase3-project-full-c/comparison.json`. All 1,234 groups completed.
Plugin TP641 / target FP0 versus native GCC TP974 / target FP48 is a
project-input comparison, not an update to the isolated-TU raw score.
For CWE401, all 400 plugin detections overlap GCC; GCC alone detects 266.
Replaying those missing groups and their actual GIMPLE gives this **disjoint**
root-cause partition:

| Missing mechanism | GCC-only leak groups |
| --- | ---: |
| Char/stdout ownership with possible callback retention/release | 162 |
| Contained pointers in local variables, arrays, by-reference and by-value aggregates | 48 |
| Fresh-return summaries with terminating failure branches / realloc input specialization | 24 |
| Eventful loop exit proofs / realloc state | 16 |
| Stateful random-return path conditions | 16 |

These are opportunities, not measured recoveries. In particular, the initial
32-group "no caller allocation candidate" category overlaps the char/I/O
barrier: two strdup bodies already had a fresh alternative, and eight char
groups belong in the 162 I/O class above. Of the remaining 24 source-return
groups, 16 use malloc/calloc and eight additionally need null-realloc input
specialization. Merely allowing every `noreturn` call in normal-return
summaries is unsound: `consume_or_exit(p, 0)` may never return to the alleged
second free. A return-conditioned effect is not an invocation return witness.

The 48 local-memory groups use four general lowering forms (flows63/64/66/67),
12 groups each. A borrow of `&pointer` does not prove a borrow of `pointer`:
the corresponding good sinks may execute `free(*pointer_address)`. Eight
groups also have realloc-state ambiguity. Work therefore targets a bounded
transitive confinement theorem, not clearing all local-spill uncertainty.

The eventful loop investigation also exposed an incomplete release-event
set: a loop-carried PHI can fail exact allocation-epoch proof even though a
later `free` may release the allocation. A stronger exit search must retain
those possible releases as witness barriers, not interpret missing must-alias
evidence as evidence of no release. The first shared-loop experiment recorded
12 target false positives and is explicitly rejected for promotion:
`artifacts/phase5-one-trip-project-c/comparison.json`.

The subsequent valid-string-entry theorem, independently of ownership,
increased UAF from85 to93 in a full C project replay, with total TP649 and
target FP0. Native GCC remained TP974 / FP48. Evidence:
`artifacts/phase4-live-precondition-full-c/comparison.json`, plugin SHA-256
`b9f13f617f1809d9f2f9cc229d0a21413133e3643417c46deec4bf7ceab134cf`.
This does not justify a universal stdio borrow theorem: the durable
`stdio_callback_ownership.c` example uses a real GNU cookie stream callback
to free the original puts input while remaining live at call entry.

The following sections retain the earlier frozen-cohort experiment; their
input mode and fingerprints must not be mixed with these project results.

## Frozen target

`tests/cohorts/rc2-call-effects.txt` contains the 462 single-TU C groups that
were false negatives with opaque ownership/access in
`artifacts/final-juliet-c-accuracy-rc2/comparison.json`.
SHA-256: `f9e1472c7f7b0589dd5055878f8dcc28cd15374e2cf5b20fce6ed99ff7f416fa`.
The cohort comprises 408 CWE401 groups and 54 CWE416 groups. It was chosen
from shared failure mechanisms before applying the contracts.

The raw baseline has 960 false-negative groups. Its disjoint triage partition
is 462 single-TU opaque-call groups (48.1%), 324 multi-TU groups (33.8%) and
174 other single-TU groups (18.1%). The latter include 102 in flows09–14, 58
in flows21/42/45 and 14 in flow17. These are input/cohort partitions, not a
claim that every multi-TU miss has one cause or is recoverable by import.
Of the 469 raw groups detected only by GCC, 318 (67.8%) lie in the selected
462-group cohort; its other 144 groups are missed by both tools. This supports
prioritizing the largest verified coherent intervention, not claiming an
exhaustive maximum over every possible algorithmic change.

All 462 groups were replayed using real GCC 16.2 GIMPLE call edges. Every
group reaches a separately compiled Juliet support function (`printLine`,
`printWLine`, or `printStructLine`). No name-based inference is authorized.
Copy operations additionally affect 90 char and 90 wchar leak groups.

## Measured intervention

The opt-in library header supplies trusted argument effects justified by the
support implementation/library semantics. It does not automatically import
source proofs. The preliminary 462-group experiment is recorded in
`artifacts/phase2-call-effects-cohort-assisted/comparison.json`, DSO SHA-256
`1d82cdf0a5aa457f47cc14db6419c5015e288ebee9fa777b55e0812e29cfb0d7`.

| Outcome on frozen cohort | CWE401 | CWE416 | Total |
| --- | ---: | ---: | ---: |
| Recovered by plugin with contracts | 282 | 27 | 309 |
| Still missed | 126 | 27 | 153 |
| Both tools detect | 220 | 0 | 220 |
| Plugin with contracts only | 62 | 27 | 89 |
| Native GCC only | 98 | 0 | 98 |
| Neither tool | 28 | 27 | 55 |

Target-CWE FP was 0 for plugin and 55 for GCC. The plugin also produced
33 good-variant cross-rule alerts (57 SA001 findings in CWE416 programs).
At least the baseline goodG2B family has explicitly documented incidental
leaks. Keep this alert burden visible; target FP0 does not prove that every
diagnostic in arbitrary code is correct.

## Largest remaining causes

| Primary reason | Groups | Native GCC detects |
| --- | ---: | ---: |
| Unsupported path condition | 126 | 84 |
| Path feasibility budget | 20 | 11 |
| Unproved realloc status | 4 | 3 |
| Callee access not proved | 3 | 0 |

The 126 guard failures include 108 leak groups, 18 per flow09–14:
external const reads 36, mutable global reads 36, external constant-return
calls 18, and external random-return calls 18. Their providers exist in
`testcasesupport/io.c`. The other 18 are char/wchar UAF flow05/07/08/09–14.
Six of those already have a usable original guard but lack PHI projection for
the nonnull access predicate. Loop17 is a separate 21-group family; 14 leak
groups hit the loop cap because AvoidingSearch lacks the eventful one-trip
support already available to SymbolicSearch.

Priority is validated provider summaries that carry ownership/access,
constant returns and global facts together. A source-derived external summary
must identify the provider, ABI and build inputs and establish binding; a
mutable-global fact additionally needs a clobber proof. Unknown external
returns are not unconstrained *exact* values: inventing feasible outcomes can
manufacture findings. PHI predicate projection and shared loop summaries are
the next smaller, general improvements.

The contract intervention demonstrates a large recoverable class, but the
raw full-corpus score remained 274 TP in the preliminary experiment. Raw
improvement therefore requires automatic information propagation, not more
benchmark declarations. Multi-TU provider import is not implemented yet.

## First automatic provider slice

Start with the struct family, where the real support body reads pointer fields
and passes only scalar values onward. This avoids assuming that an unknown
library call preserves or accesses a forwarded pointer. Of its 198 candidate
groups, contracts recover 124 leak and 11 UAF groups. Provider constant-return
and immutable-global facts target another 24 leak and 3 UAF groups in
flows11/13/14. Thus 162 groups form a first automatic replay target: 135 already
shown recoverable with contracts plus 27 additional candidates, **not** 162
measured automatic detections.

Required shared mechanisms, in order:

1. Infer body ownership and access guarantees. Current body summaries only
   record `may_dereference`; that is insufficient to establish `must_access`
   or `access_if_nonnull`.
2. Export and import summaries bound to the actual linked provider, lowered
   ABI, compiler/plugin/build inputs and dependency hashes. Compile database
   entries alone do not establish link binding. Unverified weak/interposable
   definitions, stale summaries and incomplete analysis remain unknown.
3. Carry proved constant returns and immutable initializers through that same
   binding. Mutable globals still require whole-program clobber evidence.

Char/wchar providers also forward pointers into libc. Importing their source
bodies alone does not resolve those effects. Expand using genuine GCC builtin
identities or explicitly identified library models, never helper-name matching
or treating a `format`/`access` attribute as a must-access proof.

Keep isolated-TU raw results separate from project/provider mode. Compare GCC
with the same provider visibility in any project-mode experiment; additional
source inputs must not be presented as an algorithm-only raw improvement.

## Reproduce

```sh
python3 tests/compare_juliet_oracle.py \
  --language c --cc /root/toolchains/gcc-16.2/bin/gcc \
  --cxx /root/toolchains/gcc-16.2/bin/g++ --plugin Compiler/newplugin.so \
  --group-id-file tests/cohorts/rc2-call-effects.txt \
  --plugin-contract-header tests/contracts/juliet_support_effects.h \
  --jobs 8 --output artifacts/call-effects-replay
```

Omit the contract header for raw analysis. Reports retain source, manifest,
cohort, header, runner and DSO hashes. Parallel timing is exploratory; compare
serial repeats before making speed claims.
