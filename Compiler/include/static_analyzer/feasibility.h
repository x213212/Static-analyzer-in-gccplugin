#ifndef STATIC_ANALYZER_FEASIBILITY_H
#define STATIC_ANALYZER_FEASIBILITY_H

#include "coretypes.h"

#include <string>
#include <vector>

namespace static_analyzer {
namespace feasibility {

// INFEASIBLE is the only verdict that callers may use to suppress a finding.
// UNKNOWN deliberately means "no proof": a solver timeout, an unsupported
// GIMPLE predicate, or a bounded loop must never be mislabeled INFEASIBLE.
// The active analysis policy may serialize that candidate as INCONCLUSIVE or
// retain it as an exploratory warning.
enum class Verdict {
  feasible,
  infeasible,
  unknown
};

struct Limits {
  // The exact CFG-only product is bounded by blocks * (events + 1).
  unsigned int max_cfg_states = 8192;

  // The SMT search is deliberately bounded.  It is a refinement of the
  // finite CFG check, not an unbounded path enumerator.
  unsigned int max_symbolic_states = 4096;
  unsigned int max_solver_checks = 2048;
  unsigned int max_constraints = 256;
  unsigned int max_block_visits = 2;
  unsigned int solver_timeout_ms = 25;
  unsigned int total_timeout_ms = 100;

  // Translation-unit-wide hard limits.  Per-query limits alone are not
  // sufficient when one large function produces hundreds of diagnostic
  // candidates: many individually bounded solver calls can still make the
  // compiler appear to hang.  The TU budget starts at reset_statistics(),
  // is shared by all feasibility query kinds, and is never replenished until
  // the next analyzer run.  Exhaustion always yields UNKNOWN.
  unsigned int max_tu_queries = 256;
  unsigned int max_tu_cfg_states = 262144;
  unsigned int max_tu_symbolic_states = 65536;
  unsigned int max_tu_solver_checks = 32768;
  unsigned int tu_timeout_ms = 2000;
  bool use_solver = true;
};

struct Result {
  Verdict verdict = Verdict::unknown;
  bool used_solver = false;
  // True only if this query actually initialized the Z3 context/solver.  A
  // structurally linear proof can use the solver-enabled policy without
  // paying solver-construction cost.
  bool solver_initialized = false;
  bool truncated = false;
  // True only when a translation-unit-wide limit, rather than a per-query
  // limit, stopped this query.  This is stable dump evidence for fail-open
  // budget abstention and must never accompany an INFEASIBLE verdict.
  bool tu_budget_exhausted = false;
  unsigned int cfg_states = 0;
  unsigned int symbolic_states = 0;
  unsigned int solver_checks = 0;
  unsigned int solver_prunes = 0;
  // Exact allocation/NULL constraints decided in the same path traversal,
  // without constructing an SMT context. These are not CFG may-reach checks.
  unsigned int exact_null_checks = 0;
  unsigned int exact_null_prunes = 0;
  unsigned int exact_scalar_checks = 0;
  unsigned int exact_scalar_prunes = 0;
  // Number of strict, event-neutral diamonds collapsed at their common
  // successor.  This is an exact reachability abstraction, not a dropped
  // predicate: both arms must be ordinary acyclic CFG edges and must rejoin
  // before any event or required provenance edge.
  unsigned int abstracted_diamonds = 0;
  // Number of canonical, event-neutral unsigned counted loops whose unique
  // normal exit was proven reachable before modular wrap.  The recognizer is
  // deliberately narrower than GCC's general loop optimizer: an unsupported
  // loop remains bounded UNKNOWN rather than being guessed finite.
  unsigned int abstracted_loops = 0;
  // Number of canonical loops whose ownership-event-bearing body was
  // traversed exactly once before a proven latch-to-exit transition.  These
  // loops are unfolded, not abstracted: every body statement and provenance
  // edge is still visited in execution order.
  unsigned int exact_one_trip_unfolds = 0;
  unsigned int abstracted_blocks = 0;
  unsigned int encoded_conditions = 0;
  unsigned int unsupported_conditions = 0;
  std::string reason;

  // Conservative low-level helper.  Only a proof of infeasibility returns
  // false; callers still apply the active proof-only/exploratory policy to
  // UNKNOWN.
  bool may_be_feasible() const { return verdict != Verdict::infeasible; }
  bool proven_infeasible() const { return verdict == Verdict::infeasible; }
};

struct Statistics {
  unsigned long queries = 0;
  unsigned long tu_admitted_queries = 0;
  unsigned long tu_consumed_cfg_states = 0;
  unsigned long tu_consumed_symbolic_states = 0;
  unsigned long tu_consumed_solver_checks = 0;
  unsigned long tu_budget_exhaustions = 0;
  unsigned long tu_timeout_exhaustions = 0;
  unsigned long tu_query_cap_exhaustions = 0;
  unsigned long tu_cfg_state_cap_exhaustions = 0;
  unsigned long tu_symbolic_state_cap_exhaustions = 0;
  unsigned long tu_solver_check_cap_exhaustions = 0;
  unsigned long solver_queries = 0;
  unsigned long solver_initializations = 0;
  unsigned long cfg_only_queries = 0;
  unsigned long feasible = 0;
  unsigned long infeasible = 0;
  unsigned long unknown = 0;
  unsigned long truncated = 0;
  unsigned long cfg_states = 0;
  unsigned long symbolic_states = 0;
  unsigned long solver_checks = 0;
  unsigned long solver_prunes = 0;
  unsigned long exact_null_checks = 0;
  unsigned long exact_null_prunes = 0;
  unsigned long exact_scalar_checks = 0;
  unsigned long exact_scalar_prunes = 0;
  unsigned long abstracted_diamonds = 0;
  unsigned long abstracted_loops = 0;
  unsigned long exact_one_trip_unfolds = 0;
  unsigned long abstracted_blocks = 0;
};

// A caller/callee contract can add a small scalar guard to the same symbolic
// state that proves event order.  Keeping these predicates inside the one
// traversal is essential: checking the caller path and the callee guard in
// separate SAT queries could combine mutually exclusive executions.
enum class ScalarRelation {
  equal,
  not_equal
};

struct RequiredPredicate {
  ScalarRelation relation = ScalarRelation::equal;
  tree lhs = nullptr;
  tree rhs = nullptr;
};

// Checks whether one execution can encounter EVENTS in the supplied order.
// The current GCC function (cfun) must contain every statement in its live
// GIMPLE iterators; a stale BB pointer alone is not accepted.  Empty sequences
// are feasible.  This API never mutates GIMPLE or the CFG.
Result sequence_feasible(const std::vector<gimple *> &events,
                         const Limits &limits);
Result sequence_feasible(const std::vector<gimple *> &events);

// As above, while also requiring the exact PHI provenance edges to occur
// after the first event and no later than the last event.  Event stage, edge
// stage, and path predicates are advanced by one correlated symbolic
// traversal; this prevents independent control-flow arms from being combined
// into a synthetic use-after-free witness.  Invalid or fake required edges
// conservatively return UNKNOWN.
Result sequence_feasible_through_edges(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges,
    const Limits &limits);
Result sequence_feasible_through_edges(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges);

// As above, with every scalar predicate required at the final event.  This is
// the bounded relational-summary primitive used for guards such as
// `callee_arg != NULL`.  Unsupported or inexact expressions yield UNKNOWN;
// they are never treated as satisfiable proof in proof-only mode.
Result sequence_feasible_with_predicates(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges,
    const std::vector<RequiredPredicate> &required_predicates,
    const Limits &limits);
Result sequence_feasible_with_predicates(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges,
    const std::vector<RequiredPredicate> &required_predicates);

// Checks for a path that executes REQUIRED_START and subsequently reaches the
// current function's EXIT without executing any FORBIDDEN statement.  This is
// the conservative leak-refinement query: INFEASIBLE proves that every exit
// path after the allocation is released (or that the allocation cannot reach
// EXIT); FEASIBLE proves a modeled avoid-release path; UNKNOWN must retain the
// leak finding.
Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const Limits &limits);
Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden);

// Additionally excludes exact CFG edges after REQUIRED_START becomes active.
// This models ownership transferred on one PHI incoming edge without treating
// the shared return statement as a transfer on every arm.  Before the start,
// these edges remain traversable.  Stale, foreign, fake, or abnormal edges
// return UNKNOWN; path abstractions never skip an excluded edge.
Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const std::vector<edge> &forbidden_edges, const Limits &limits);
Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const std::vector<edge> &forbidden_edges);

// Adds exact zero/nonzero constraints on independent caller entry parameters
// or literal integers. Unsupported predicates remain UNKNOWN.
Result path_to_exit_avoiding_with_predicates(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const std::vector<edge> &forbidden_edges,
    const std::vector<RequiredPredicate> &required_predicates,
    const Limits &limits);

void set_default_limits(const Limits &limits);
Limits default_limits();

bool solver_compiled_in();
const char *verdict_name(Verdict verdict);

Statistics statistics();
void reset_statistics();

// Writes stable, one-line records through GCC's dump machinery.  They appear
// with -fdump-ipa-all-details and stay out of normal compiler diagnostics.
void dump_result(const Result &result, const char *purpose = nullptr);
void dump_statistics();

}  // namespace feasibility
}  // namespace static_analyzer

#endif
