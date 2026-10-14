# Changelog

All notable changes to the modern GCC GIMPLE static analyzer are documented in
this file.

## Unreleased - 2026-10-15

Restore GCC 10/16 compilation of the pending PHI/nonnull contract change.
Mixed body ownership effects now block the paths that execute their calls;
a separately proved caller path may still establish a leak. Unknown callee
outcomes are never selected as a concrete borrow assumption.

Leak feasibility handles exact allocation/NULL and independent integral
entry-parameter zero/nonzero prefixes without constructing Z3. Harder
conditions replay that prefix into the same bounded solver. Fixed-width
integral AND/OR/XOR expressions are encoded exactly, including Boolean
conjunctions lowered by GCC. Existing state, query, time and loop caps remain.

Inconclusive ownership reports identify up to eight related source locations
and explain the release/transfer and preservation assumptions separately.
These model alternatives remain INCONCLUSIVE and do not increase finding
counts. Direct leak findings include allocation trace evidence.

Exact source-body conditional deallocator guards now compose with caller
constraints. Only a proved preserving branch can remove its call barrier;
unknown external results and conflicting guards retain conservative behavior.
Standard narrow/wide library models check ABI types and give visible bodies
and aliases priority. Bounded local-pointer confinement, contained-pointer
access, and fresh-return/null-realloc specialization recover additional flows.
Ordinary-stdout assumptions are recorded and custom callbacks remain outside
that profile. Optional unknown-call assumption buttons and post-link binary
inspection produce separately labeled evidence, never confirmed-score credit.

Optional FFmpeg API and registered-callback resource checks are separate
modules within the same plugin. `librarymodel=ffmpeg` is opt-in; its four
reviews on three upstream-confirmed affected-file pairs remain
`MODEL_REVIEW/conditional`, with complete path verdicts still `unknown`.
All three fixed files have zero alerts. These reviews are not credited as
proven detections or folded into the frozen Juliet score. Automated source
repair is not implemented.

`reportonly=1` retains the same JSON/HTML findings without plugin compiler
warnings, allowing integration with `-Werror` while preserving GCC's own
diagnostics. Reports include expandable source, trace and upstream repair
evidence. Local editor logs and CMake caches are no longer tracked; ignore
rules exclude common credential files and local analysis output.

The 150-case suite passes on GCC 10 and GCC 16; all 138 Python checks pass.
Two independently generated ownership sets contain 768 programs: all 626
known leaks are detected and 142 safe cases have no leak alarms. This is
bounded synthetic coverage. The frozen Phase-17 complete 1,234-group C LTO project replay is
1,123 TP / target FP0 versus native 974 TP / FP48, with 111 misses remaining.
Exact-site detection remains 293 versus native 398; 55 groups are still
detected only by GCC. Detailed evidence and model limits are recorded in
`docs/current-progress-2026-10-15.md`. No complete-coverage or universal
zero-false-positive claim is made. The current optional-profile binary has
not repeated that full-corpus replay.

## 1.0.0-rc2 - 2026-09-05

This release candidate improves recall through general GIMPLE/CFG proofs
rather than testcase-specific patterns.  Feasibility now collapses strictly
proved finite event-neutral counted loops, exactly unfolds the supported
one-trip eventful loop shape, keeps a loop-cap failure local to its branch,
and resolves body-bound call predicates only when every reachable path in a
call-free CFG DAG returns the same exact integer constant.  Unsupported,
cyclic, exceptional, volatile, abnormal, wrapping, or resource-bounded forms
remain `INCONCLUSIVE` under `proof-only`.
Safe translation-unit-local static guard constants are read through GCC's
`get_symbol_constant_value` API rather than a hand-written initializer walk.

`SA003` now covers a proved dangling pointer returned after release.  Its
allocation, release, return edge, and path constraints are checked as one
witness.  When GCC lowers source returns into a PHI-fed artificial
`GIMPLE_RETURN`, the report obtains the source line from the matching
`gimple_phi_arg_location`; it does not infer a neighboring line.

The hot path now reuses one lazily constructed Z3 context per translation
unit while retaining a fresh solver and symbol map per query, avoids solver
construction for unsupported/inexact predicates, uses dense block-index
typestate storage when GCC block indices are suitably compact, uses hashed
provenance memoization, and skips ownership work when its rules are disabled.
The shared object hides internal C++ symbols and exports only GCC's two plugin
entry symbols.  Z3 is still directly linked when enabled; this release does
not claim lazy shared-library loading.

Call effects now combine argument-specific ownership, independent access
guarantees, and exact return provenance. GCC call flags/fnspec/builtin identity
can prove restricted effects; `ERF_RETURNS_ARG` and bound identity summaries
extend both directions of SSA traversal. Explicit source attributes allow
trusted borrow/must-access/nonnull-guarded-access contracts. Their C++ indices
exclude `this`; unsupported artificial parameter layouts are rejected.
Unknown returned aliases and pointer-to-integer representation escapes do not
establish leak proofs. PHI returns preserve conditional ownership-transfer
edges through both typestate and leak feasibility.

Contract-assisted Juliet runs are opt-in and carry separate labels and input
hashes. They are not raw native-vs-plugin accuracy measurements.

Body-bound access summaries now prove access cuts over GCC's CFG, including
nonnull-guarded and nested accesses. Unknown branches are retained; bypasses,
cycles and opaque calls before access reject the theorem. GCC's memory-operand
walker and exact positive access extents exclude address-only, zero-width and
CLOBBER pseudo-accesses. May/must analysis shares one immediate-use expansion.

The reviewed assertion manifest contains 122 isolated C/C++ cases, including
exact primary/trace-line checks and adversarial loop, total-return, PHI,
exception, and dangling-return variants.  Formal GCC 16.2 comparison and
serial timing evidence are recorded in
`docs/release-validation-1.0.0-rc2.md`.  These finite-corpus results do not
guarantee zero false positives, complete alias resolution, or zero overhead
for arbitrary C/C++ programs.

## 1.0.0-rc1 - 2026-09-05

This is the first release candidate for the modern bounded analysis engine. It
replaces the historical detector as the default with explicit GIMPLE/SSA value
flow, GCC alias evidence, finite lifetime typestates, translation-unit-local
ownership summaries, bounded path feasibility, and machine-readable HTML/JSON
reporting. The historical engine remains an opt-in compatibility path.

The release candidate reports `SA001` memory leaks, `SA002` double frees,
`SA003` use after free, and conservative `SA004` pthread-lifetime candidates.
The default `proof-only` policy records unsupported or resource-bounded cases
as `INCONCLUSIVE` rather than promoting incomplete evidence to a finding.

MemFix-style automatic repair is not implemented in this release.
`automatic_patch` remains `false`; the in-process Z3 integration refines path
feasibility and is not a MemFix exact-cover or MaxSAT patch selector.

The reviewed manifest suite and small GCC comparison are regression evidence,
not independent measurements of real-world accuracy. Formal accuracy and
configured compiler-process timing claims must be taken from a generated
Juliet manifest-oracle report, together with its toolchain identity, plugin and
oracle hashes, coverage, common-paired population, flags, job count, and run
conditions. This changelog intentionally publishes no benchmark number without
that report artifact.
