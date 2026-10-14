#include "gcc-plugin.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "gimple-fold.h"
#include "basic-block.h"
#include "cfg.h"
#include "cgraph.h"
#include "tree-cfg.h"
#include "function.h"
#include "dumpfile.h"

#include "static_analyzer/feasibility.h"
#include "static_analyzer/memory_value.h"
#include "static_analyzer/ownership_call.h"
#include "static_analyzer/ownership_summary.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <deque>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#ifdef STATIC_ANALYZER_HAVE_Z3
// GCC exposes its -O level as an `optimize` macro.  That macro collides with
// z3::optimize while parsing z3++.h; all GCC headers needed here have already
// been included, so it is safe to remove the preprocessor alias locally.
#ifdef optimize
#undef optimize
#endif
#include <z3++.h>
#endif

namespace static_analyzer {
namespace feasibility {
namespace {

Limits configured_limits;
Statistics accumulated;

enum class TuBudgetLimit {
  none,
  timeout,
  queries,
  cfg_states,
  symbolic_states,
  solver_checks
};

struct TuBudgetState {
  bool initialized = false;
  Limits limits;
  std::chrono::steady_clock::time_point deadline;
  unsigned long queries = 0;
  unsigned long cfg_states = 0;
  unsigned long symbolic_states = 0;
  unsigned long solver_checks = 0;
};

TuBudgetState tu_budget;

#ifdef STATIC_ANALYZER_HAVE_Z3
// Z3 contexts are expensive compared with the tiny translation units this
// plugin commonly analyzes.  Queries are sequential inside cc1, so one lazy
// context per TU is sufficient; each query still receives a fresh solver and
// a fresh symbol map, preventing constraint leakage across functions.
std::unique_ptr<z3::context> tu_z3_context;

z3::context &shared_z3_context() {
  if (!tu_z3_context)
    tu_z3_context.reset(new z3::context());
  return *tu_z3_context;
}
#endif

const char *tu_budget_reason(TuBudgetLimit limit) {
  switch (limit) {
    case TuBudgetLimit::timeout:
      return "TU feasibility wall-clock budget exhausted";
    case TuBudgetLimit::queries:
      return "TU feasibility query cap exhausted";
    case TuBudgetLimit::cfg_states:
      return "TU feasibility CFG-state cap exhausted";
    case TuBudgetLimit::symbolic_states:
      return "TU feasibility symbolic-state cap exhausted";
    case TuBudgetLimit::solver_checks:
      return "TU feasibility solver-check cap exhausted";
    case TuBudgetLimit::none:
      break;
  }
  return "TU feasibility budget exhausted";
}

void start_tu_budget(const Limits &limits) {
  tu_budget = TuBudgetState{};
  tu_budget.initialized = true;
  tu_budget.limits = limits;
  tu_budget.deadline =
      std::chrono::steady_clock::now() +
      std::chrono::milliseconds(limits.tu_timeout_ms);
}

class QueryBudget {
 public:
  QueryBudget(const Limits &limits, Result *result)
      : limits_(limits), result_(result),
        deadline_(std::chrono::steady_clock::now() +
                  std::chrono::milliseconds(limits.total_timeout_ms)) {}

  bool begin() {
    if (!tu_budget.initialized)
      start_tu_budget(limits_);
    TuBudgetLimit exhausted = current_tu_limit();
    if (exhausted != TuBudgetLimit::none)
      return fail_tu(exhausted);
    ++tu_budget.queries;
    return time_available();
  }

  bool time_available() {
    if (hard_stopped_)
      return false;
    if (std::chrono::steady_clock::now() >= tu_budget.deadline)
      return fail_tu(TuBudgetLimit::timeout);
    if (std::chrono::steady_clock::now() >= deadline_) {
      result_->verdict = Verdict::unknown;
      result_->truncated = true;
      result_->reason = "per-query feasibility timeout";
      hard_stopped_ = true;
      return false;
    }
    return true;
  }

  bool consume_cfg_state() {
    if (!time_available())
      return false;
    if (result_->cfg_states >= limits_.max_cfg_states) {
      result_->verdict = Verdict::unknown;
      result_->truncated = true;
      result_->reason = "CFG product-state cap";
      hard_stopped_ = true;
      return false;
    }
    if (tu_budget.cfg_states >= tu_budget.limits.max_tu_cfg_states)
      return fail_tu(TuBudgetLimit::cfg_states);
    ++result_->cfg_states;
    ++tu_budget.cfg_states;
    return true;
  }

  bool consume_symbolic_state(bool record_in_result = true) {
    if (!time_available())
      return false;
    if (symbolic_work_ >= limits_.max_symbolic_states) {
      result_->verdict = Verdict::unknown;
      result_->truncated = true;
      result_->reason = "symbolic state cap";
      hard_stopped_ = true;
      return false;
    }
    if (tu_budget.symbolic_states >=
        tu_budget.limits.max_tu_symbolic_states)
      return fail_tu(TuBudgetLimit::symbolic_states);
    ++symbolic_work_;
    if (record_in_result)
      ++result_->symbolic_states;
    ++tu_budget.symbolic_states;
    return true;
  }

  bool consume_solver_check() {
    if (!time_available())
      return false;
    if (result_->solver_checks >= limits_.max_solver_checks) {
      result_->verdict = Verdict::unknown;
      result_->truncated = true;
      result_->reason = "solver check cap";
      hard_stopped_ = true;
      return false;
    }
    if (tu_budget.solver_checks >=
        tu_budget.limits.max_tu_solver_checks)
      return fail_tu(TuBudgetLimit::solver_checks);
    ++result_->solver_checks;
    ++tu_budget.solver_checks;
    return true;
  }

  unsigned int solver_timeout_ms() const {
    const auto now = std::chrono::steady_clock::now();
    const auto query_remaining =
        std::chrono::duration_cast<std::chrono::milliseconds>(deadline_ - now)
            .count();
    const auto tu_remaining =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            tu_budget.deadline - now)
            .count();
    const unsigned long remaining = static_cast<unsigned long>(
        std::max<std::int64_t>(1, std::min(query_remaining, tu_remaining)));
    // A zero Z3 timeout means "unbounded", which is never suitable here.
    const unsigned long configured =
        std::max<unsigned long>(1, limits_.solver_timeout_ms);
    return static_cast<unsigned int>(std::min(configured, remaining));
  }

 private:
  TuBudgetLimit current_tu_limit() const {
    // Prefer deterministic resource caps in dump evidence when more than one
    // limit becomes observable between two queries.
    if (tu_budget.queries >= tu_budget.limits.max_tu_queries)
      return TuBudgetLimit::queries;
    if (tu_budget.cfg_states >= tu_budget.limits.max_tu_cfg_states)
      return TuBudgetLimit::cfg_states;
    if (tu_budget.symbolic_states >=
        tu_budget.limits.max_tu_symbolic_states)
      return TuBudgetLimit::symbolic_states;
    if (tu_budget.solver_checks >=
        tu_budget.limits.max_tu_solver_checks)
      return TuBudgetLimit::solver_checks;
    if (std::chrono::steady_clock::now() >= tu_budget.deadline)
      return TuBudgetLimit::timeout;
    return TuBudgetLimit::none;
  }

  bool fail_tu(TuBudgetLimit limit) {
    result_->verdict = Verdict::unknown;
    result_->truncated = true;
    result_->tu_budget_exhausted = true;
    result_->reason = tu_budget_reason(limit);
    hard_stopped_ = true;
    return false;
  }

  const Limits &limits_;
  Result *result_;
  std::chrono::steady_clock::time_point deadline_;
  unsigned int symbolic_work_ = 0;
  // Local path abstraction (for example one loop-visit cap) sets Result's
  // truncated bit but must not starve exact sibling paths.  Only actual
  // per-query/TU resource exhaustion stops the whole search.
  bool hard_stopped_ = false;
};

struct ProductState {
  basic_block block;
  unsigned int stage;

  bool operator<(const ProductState &other) const {
    if (block != other.block)
      return std::less<basic_block>()(block, other.block);
    return stage < other.stage;
  }
};

struct EdgeProductState {
  basic_block block;
  unsigned int event_stage;
  unsigned int edge_stage;

  bool operator<(const EdgeProductState &other) const {
    if (block != other.block)
      return std::less<basic_block>()(block, other.block);
    if (event_stage != other.event_stage)
      return event_stage < other.event_stage;
    return edge_stage < other.edge_stage;
  }
};

gimple *block_terminator(basic_block block) {
  if (block == nullptr)
    return nullptr;
  gimple_stmt_iterator iterator = gsi_last_bb(block);
  return gsi_end_p(iterator) ? nullptr : gsi_stmt(iterator);
}

struct StructuralResult {
  bool reachable = false;
  bool truncated = false;
  // A reachable CFG construct was intentionally excluded from the model.
  // Callers must not turn a negative search result into an INFEASIBLE proof
  // when this bit is set.
  bool incomplete = false;
};

unsigned int advance_events(basic_block block, unsigned int stage,
                            const std::vector<gimple *> &events,
                            QueryBudget *budget) {
  for (gphi_iterator iterator = gsi_start_phis(block);
       !gsi_end_p(iterator) && stage < events.size(); gsi_next(&iterator)) {
    if (!budget->time_available())
      return stage;
    if (gsi_stmt(iterator) == events[stage])
      ++stage;
  }
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator) && stage < events.size(); gsi_next(&iterator)) {
    if (!budget->time_available())
      return stage;
    if (gsi_stmt(iterator) == events[stage])
      ++stage;
  }
  return stage;
}

struct FunctionInventory {
  std::set<basic_block> blocks;
  std::map<gimple *, basic_block> statements;
  std::map<edge, basic_block> edges;
  std::map<edge, basic_block> predecessor_edges;
};

bool inventory_current_function(FunctionInventory *inventory,
                                QueryBudget *budget) {
  if (inventory == nullptr || cfun == nullptr || cfun->cfg == nullptr)
    return false;

  basic_block block;
  FOR_ALL_BB_FN(block, cfun) {
    if (!budget->time_available())
      return false;
    inventory->blocks.insert(block);

    for (gphi_iterator iterator = gsi_start_phis(block);
         !gsi_end_p(iterator); gsi_next(&iterator)) {
      if (!budget->time_available())
        return false;
      inventory->statements.insert(
          std::make_pair(gsi_stmt(iterator), block));
    }
    for (gimple_stmt_iterator iterator = gsi_start_bb(block);
         !gsi_end_p(iterator); gsi_next(&iterator)) {
      if (!budget->time_available())
        return false;
      inventory->statements.insert(
          std::make_pair(gsi_stmt(iterator), block));
    }

    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if (!budget->time_available())
        return false;
      inventory->edges.insert(std::make_pair(successor, block));
    }
    edge predecessor;
    FOR_EACH_EDGE(predecessor, iterator, block->preds) {
      if (!budget->time_available())
        return false;
      inventory->predecessor_edges.insert(
          std::make_pair(predecessor, block));
    }
  }
  return true;
}

// Checking gimple_bb alone is not enough: clients can retain a statement
// whose old BB pointer still happens to name a live block after a CFG rewrite.
// Build the inventory from the current iterators and require pointer identity
// with a statement that is actually present in that block.
bool inventory_owns_events(const FunctionInventory &inventory,
                           const std::vector<gimple *> &events,
                           QueryBudget *budget) {
  for (gimple *statement : events) {
    if (!budget->time_available())
      return false;
    if (statement == nullptr)
      return false;
    const auto found = inventory.statements.find(statement);
    if (found == inventory.statements.end() ||
        gimple_bb(statement) != found->second)
      return false;
  }
  return true;
}

// Do not dereference a client-supplied edge until it has been found by pointer
// identity in the current CFG.  This makes stale, cross-function, null, and
// fake PHI witnesses conservative UNKNOWN inputs rather than false proofs.
bool inventory_owns_required_edges(
    const FunctionInventory &inventory,
    const std::vector<edge> &required_edges, QueryBudget *budget) {
  for (edge required : required_edges) {
    if (!budget->time_available())
      return false;
    if (required == nullptr)
      return false;
    const auto found = inventory.edges.find(required);
    const auto predecessor = inventory.predecessor_edges.find(required);
    if (found == inventory.edges.end() ||
        predecessor == inventory.predecessor_edges.end())
      return false;
    if (required->src != found->second ||
        required->dest != predecessor->second ||
        required->dest == nullptr ||
        inventory.blocks.find(required->src) == inventory.blocks.end() ||
        inventory.blocks.find(required->dest) == inventory.blocks.end() ||
        (required->flags & EDGE_FAKE) != 0)
      return false;
  }
  return true;
}

bool inventory_owns_forbidden_edges(
    const FunctionInventory &inventory,
    const std::vector<edge> &forbidden_edges, QueryBudget *budget) {
  // Establish pointer ownership before reading any caller-supplied edge.
  if (!inventory_owns_required_edges(inventory, forbidden_edges, budget))
    return false;
  const unsigned int normal_flags =
      EDGE_FALLTHRU | EDGE_DFS_BACK | EDGE_IRREDUCIBLE_LOOP |
      EDGE_TRUE_VALUE | EDGE_FALSE_VALUE | EDGE_EXECUTABLE | EDGE_CROSSING |
      EDGE_CAN_FALLTHRU | EDGE_LOOP_EXIT;
  for (edge forbidden : forbidden_edges) {
    if (!budget->time_available() ||
        (static_cast<unsigned int>(forbidden->flags) & ~normal_flags) != 0)
      return false;
  }
  return true;
}

bool current_function_owns(const std::vector<gimple *> &events,
                           QueryBudget *budget) {
  FunctionInventory inventory;
  return inventory_current_function(&inventory, budget) &&
         inventory_owns_events(inventory, events, budget);
}

StructuralResult structural_sequence(
    const std::vector<gimple *> &events, QueryBudget *budget) {
  StructuralResult result;
  std::deque<ProductState> worklist;
  std::set<ProductState> visited;

  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, ENTRY_BLOCK_PTR_FOR_FN(cfun)->succs) {
    if (!budget->time_available()) {
      result.truncated = true;
      return result;
    }
    if ((successor->flags & EDGE_FAKE) == 0)
      worklist.push_back({successor->dest, 0});
  }

  while (!worklist.empty()) {
    ProductState state = worklist.front();
    worklist.pop_front();
    if (!visited.insert(state).second)
      continue;
    if (!budget->consume_cfg_state()) {
      result.truncated = true;
      return result;
    }

    if (state.block == EXIT_BLOCK_PTR_FOR_FN(cfun))
      continue;
    state.stage = advance_events(state.block, state.stage, events, budget);
    if (!budget->time_available()) {
      result.truncated = true;
      return result;
    }
    if (state.stage == events.size()) {
      result.reachable = true;
      return result;
    }

    FOR_EACH_EDGE(successor, iterator, state.block->succs) {
      if (!budget->time_available()) {
        result.truncated = true;
        return result;
      }
      if ((successor->flags & EDGE_FAKE) == 0)
        worklist.push_back({successor->dest, state.stage});
    }
  }
  if (!budget->time_available())
    result.truncated = true;
  return result;
}

StructuralResult structural_sequence_through_edges(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges,
    QueryBudget *budget) {
  StructuralResult result;
  std::deque<EdgeProductState> worklist;
  std::set<EdgeProductState> visited;

  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, ENTRY_BLOCK_PTR_FOR_FN(cfun)->succs) {
    if (!budget->time_available()) {
      result.truncated = true;
      return result;
    }
    if ((successor->flags & EDGE_FAKE) == 0)
      worklist.push_back({successor->dest, 0, 0});
  }

  while (!worklist.empty()) {
    EdgeProductState state = worklist.front();
    worklist.pop_front();
    if (!visited.insert(state).second)
      continue;
    if (!budget->consume_cfg_state()) {
      result.truncated = true;
      return result;
    }

    if (state.block == EXIT_BLOCK_PTR_FOR_FN(cfun))
      continue;
    state.event_stage =
        advance_events(state.block, state.event_stage, events, budget);
    if (!budget->time_available()) {
      result.truncated = true;
      return result;
    }
    if (state.event_stage == events.size()) {
      if (state.edge_stage == required_edges.size()) {
        result.reachable = true;
        return result;
      }
      continue;
    }

    FOR_EACH_EDGE(successor, iterator, state.block->succs) {
      if (!budget->time_available()) {
        result.truncated = true;
        return result;
      }
      if ((successor->flags & EDGE_FAKE) != 0)
        continue;
      unsigned int next_edge_stage = state.edge_stage;
      // The allocation is EVENTS[0].  A PHI edge witnessing propagation from
      // that SSA definition cannot be consumed before the definition itself.
      if (state.event_stage != 0 &&
          next_edge_stage < required_edges.size() &&
          successor == required_edges[next_edge_stage])
        ++next_edge_stage;
      worklist.push_back(
          {successor->dest, state.event_stage, next_edge_stage});
    }
  }
  if (!budget->time_available())
    result.truncated = true;
  return result;
}

bool scan_avoiding(basic_block block, bool active, gimple *required_start,
                   const std::set<gimple *> &forbidden,
                   QueryBudget *budget) {
  for (gphi_iterator iterator = gsi_start_phis(block); !gsi_end_p(iterator);
       gsi_next(&iterator)) {
    if (!budget->time_available())
      return active;
    gimple *statement = gsi_stmt(iterator);
    if (statement == required_start)
      active = true;
    else if (active && forbidden.find(statement) != forbidden.end())
      active = false;
  }
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    if (!budget->time_available())
      return active;
    gimple *statement = gsi_stmt(iterator);
    if (statement == required_start)
      active = true;
    else if (active && forbidden.find(statement) != forbidden.end())
      active = false;
  }
  return active;
}

StructuralResult structural_path_to_exit_avoiding(
    gimple *required_start, const std::set<gimple *> &forbidden,
    const std::set<edge> &forbidden_edges, QueryBudget *budget) {
  StructuralResult result;
  std::deque<ProductState> worklist;
  std::set<ProductState> visited;

  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, ENTRY_BLOCK_PTR_FOR_FN(cfun)->succs) {
    if (!budget->time_available()) {
      result.truncated = true;
      return result;
    }
    if ((successor->flags & EDGE_FAKE) == 0)
      worklist.push_back({successor->dest, 0});
  }

  while (!worklist.empty()) {
    ProductState state = worklist.front();
    worklist.pop_front();
    if (!visited.insert(state).second)
      continue;
    if (!budget->consume_cfg_state()) {
      result.truncated = true;
      return result;
    }

    const bool active = state.stage != 0;
    if (state.block == EXIT_BLOCK_PTR_FOR_FN(cfun)) {
      if (active)
        result.reachable = true;
      continue;
    }

    const bool output = scan_avoiding(state.block, active, required_start,
                                      forbidden, budget);
    if (!budget->time_available()) {
      result.truncated = true;
      return result;
    }
    FOR_EACH_EDGE(successor, iterator, state.block->succs) {
      if (!budget->time_available()) {
        result.truncated = true;
        return result;
      }
      if (output && forbidden_edges.find(successor) != forbidden_edges.end())
        continue;
      if ((successor->flags & EDGE_FAKE) != 0) {
        // GCC uses fake EXIT edges for constructs such as non-terminating
        // loops.  Once the allocation is active, dropping such an edge and
        // then proving "no avoiding exit" would be an unsound leak
        // suppression.  Record model incompleteness instead.
        if (output)
          result.incomplete = true;
        continue;
      }
      worklist.push_back({successor->dest, output ? 1u : 0u});
    }
  }
  if (!budget->time_available())
    result.truncated = true;
  return result;
}

#ifdef STATIC_ANALYZER_HAVE_Z3

std::vector<std::set<basic_block>> reverse_reachability(
    const std::vector<gimple *> &events, QueryBudget *budget) {
  std::vector<std::set<basic_block>> result(events.size());
  for (unsigned int index = 0; index < events.size(); ++index) {
    std::deque<basic_block> worklist;
    worklist.push_back(gimple_bb(events[index]));
    while (!worklist.empty()) {
      basic_block block = worklist.front();
      worklist.pop_front();
      if (!result[index].insert(block).second)
        continue;
      if (!budget->consume_symbolic_state(false))
        return result;
      edge predecessor;
      edge_iterator iterator;
      FOR_EACH_EDGE(predecessor, iterator, block->preds) {
        if (!budget->time_available())
          return result;
        if ((predecessor->flags & EDGE_FAKE) == 0)
          worklist.push_back(predecessor->src);
      }
    }
  }
  return result;
}

std::set<basic_block> reverse_reachability(basic_block target,
                                           QueryBudget *budget) {
  std::set<basic_block> result;
  std::deque<basic_block> worklist;
  worklist.push_back(target);
  while (!worklist.empty()) {
    basic_block block = worklist.front();
    worklist.pop_front();
    if (!result.insert(block).second)
      continue;
    if (!budget->consume_symbolic_state(false))
      return result;
    edge predecessor;
    edge_iterator iterator;
    FOR_EACH_EDGE(predecessor, iterator, block->preds) {
      if (!budget->time_available())
        return result;
      if ((predecessor->flags & EDGE_FAKE) == 0)
        worklist.push_back(predecessor->src);
    }
  }
  return result;
}

bool successor_can_reach_event(
    basic_block successor, unsigned int stage,
    const std::vector<std::set<basic_block>> &can_reach) {
  return stage >= can_reach.size() ||
         can_reach[stage].find(successor) != can_reach[stage].end();
}

// GCC's CFG API exposes exact edge identity and edge flags.  Use those facts
// to recognize only the smallest structured region for which path merging is
// a theorem rather than a heuristic:
//
/*
 *                 condition
 *                 /       \
 *          [one block] [one block]     (either arm may be empty)
 *                 \       /
 *                    join
 */
//
// Every non-empty arm must have exactly one ordinary edge to the same join.
// There can be no event or required PHI-provenance edge inside the region.
// Consequently at least one arm reaches JOIN for every valuation of the
// GIMPLE_COND, and forgetting this one predicate preserves existence of an
// event path exactly.  Calls and assignments inside an arm do not invalidate
// the proof: an exact scalar used after the join is either represented by its
// dominating SSA definition, or needs a PHI (which ConditionEncoder already
// treats as inexact and therefore cannot use for an UNSAT proof).  EH,
// abnormal, fake, cyclic-arm, larger, and otherwise unsupported regions
// simply fail recognition and retain the conservative bounded search.  A
// surrounding loop is still handled by the caller's repeated-block cap and
// can only make the final verdict UNKNOWN.
struct NeutralDiamond {
  basic_block join = nullptr;
  unsigned int interior_blocks = 0;
};

struct NeutralCountedLoop {
  basic_block exit = nullptr;
  unsigned int interior_blocks = 0;
};

// A canonical loop whose guard is known to select the body for the initial
// PHI value and the exit for the value carried by its unique latch.  Unlike
// NeutralCountedLoop this descriptor never skips the body: SymbolicSearch
// follows the body once, observes every ownership event, follows the real
// latch edge, and only then forces the proven exit edge.
struct ExactOneTripLoop {
  edge preheader = nullptr;
  edge latch = nullptr;
  edge body = nullptr;
  edge exit = nullptr;
};

bool ordinary_edge(edge candidate) {
  return candidate != nullptr &&
         (candidate->flags &
          (EDGE_FAKE | EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL)) == 0;
}

bool block_contains_relevant_statement(
    basic_block block, const std::set<gimple *> &relevant_statements,
    QueryBudget *budget) {
  for (gphi_iterator iterator = gsi_start_phis(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    if (!budget->time_available())
      return true;
    if (relevant_statements.find(gsi_stmt(iterator)) !=
        relevant_statements.end())
      return true;
  }
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    if (!budget->time_available())
      return true;
    if (relevant_statements.find(gsi_stmt(iterator)) !=
        relevant_statements.end())
      return true;
  }
  return false;
}

bool counted_loop_body_is_bounded(basic_block block, QueryBudget *budget) {
  if (block == nullptr)
    return false;
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    if (!budget->time_available())
      return false;
    gimple *statement = gsi_stmt(iterator);
    if (is_gimple_debug(statement))
      continue;
    // Calls, asm, volatile accesses, and control statements require effect or
    // termination modeling that this small loop theorem intentionally does
    // not provide.  Ordinary assignments include the array-initialization
    // stores that motivated this abstraction; virtual SSA keeps their memory
    // effects visible to later GIMPLE statements.
    if (!is_gimple_assign(statement) ||
        gimple_has_volatile_ops(statement))
      return false;
  }
  return true;
}

bool counted_loop_header_is_bounded(basic_block block, gimple *terminator,
                                    QueryBudget *budget) {
  if (block == nullptr || terminator == nullptr)
    return false;
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    if (!budget->time_available())
      return false;
    gimple *statement = gsi_stmt(iterator);
    // A source label is a position marker, not an executable effect.  It is
    // common in a canonical hand-written false-edge loop and does not alter
    // the PHI recurrence or branch theorem.
    if (is_gimple_debug(statement) ||
        gimple_code(statement) == GIMPLE_LABEL || statement == terminator)
      continue;
    return false;
  }
  return true;
}

bool required_edge_p(edge candidate,
                     const std::set<edge> &required_edges) {
  return required_edges.find(candidate) != required_edges.end();
}

bool single_ordinary_successor(basic_block block, edge *successor) {
  if (block == nullptr || EDGE_COUNT(block->succs) != 1)
    return false;
  edge only = single_succ_edge(block);
  if (!ordinary_edge(only))
    return false;
  *successor = only;
  return true;
}

bool neutral_arm(basic_block arm, basic_block join, edge exit,
                 const std::set<gimple *> &relevant_statements,
                 const std::set<edge> &required_edges,
                 QueryBudget *budget) {
  return arm != nullptr && join != nullptr && arm != join &&
         exit != nullptr && exit->src == arm && exit->dest == join &&
         !required_edge_p(exit, required_edges) &&
         !block_contains_relevant_statement(arm, relevant_statements,
                                            budget);
}

bool strict_neutral_diamond(
    basic_block block, const std::set<gimple *> &relevant_statements,
    const std::set<edge> &required_edges, NeutralDiamond *result,
    QueryBudget *budget) {
  if (!budget->time_available())
    return false;
  if (block == nullptr || result == nullptr ||
      EDGE_COUNT(block->succs) != 2)
    return false;
  gimple *terminator = block_terminator(block);
  if (terminator == nullptr || gimple_code(terminator) != GIMPLE_COND)
    return false;

  edge branches[2] = {nullptr, nullptr};
  unsigned int count = 0;
  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, block->succs) {
    if (count >= 2 || !ordinary_edge(successor) ||
        required_edge_p(successor, required_edges))
      return false;
    branches[count++] = successor;
  }
  if (count != 2 || branches[0]->dest == nullptr ||
      branches[1]->dest == nullptr ||
      branches[0]->dest == branches[1]->dest)
    return false;
  const bool first_true =
      (branches[0]->flags & EDGE_TRUE_VALUE) != 0;
  const bool first_false =
      (branches[0]->flags & EDGE_FALSE_VALUE) != 0;
  const bool second_true =
      (branches[1]->flags & EDGE_TRUE_VALUE) != 0;
  const bool second_false =
      (branches[1]->flags & EDGE_FALSE_VALUE) != 0;
  // A GIMPLE_COND is a total Boolean partition only when GCC identifies the
  // two alternatives explicitly.  Do not abstract switch-like, guessed, or
  // malformed multi-successor CFGs.
  if (!((first_true && !first_false && second_false && !second_true) ||
        (first_false && !first_true && second_true && !second_false)))
    return false;

  basic_block left = branches[0]->dest;
  basic_block right = branches[1]->dest;
  edge left_exit = nullptr;
  edge right_exit = nullptr;
  const bool left_single = single_ordinary_successor(left, &left_exit);
  const bool right_single = single_ordinary_successor(right, &right_exit);

  // One empty arm: the condition has a direct edge to the join, while the
  // other arm consists of exactly one block.
  if (right_single && right_exit->dest == left && left != block &&
      neutral_arm(right, left, right_exit, relevant_statements,
                  required_edges, budget)) {
    result->join = left;
    result->interior_blocks = 1;
    return true;
  }
  if (left_single && left_exit->dest == right && right != block &&
      neutral_arm(left, right, left_exit, relevant_statements,
                  required_edges, budget)) {
    result->join = right;
    result->interior_blocks = 1;
    return true;
  }

  // Two one-block arms.
  if (!left_single || !right_single || left_exit->dest == nullptr ||
      left_exit->dest != right_exit->dest || left_exit->dest == block ||
      left == right)
    return false;
  basic_block join = left_exit->dest;
  if (!neutral_arm(left, join, left_exit, relevant_statements,
                   required_edges, budget) ||
      !neutral_arm(right, join, right_exit, relevant_statements,
                   required_edges, budget))
    return false;
  result->join = join;
  result->interior_blocks = 2;
  return true;
}

enum tree_code swap_comparison(enum tree_code code) {
  switch (code) {
    case LT_EXPR:
      return GT_EXPR;
    case LE_EXPR:
      return GE_EXPR;
    case GT_EXPR:
      return LT_EXPR;
    case GE_EXPR:
      return LE_EXPR;
    case EQ_EXPR:
    case NE_EXPR:
      return code;
    default:
      return ERROR_MARK;
  }
}

enum tree_code negate_comparison(enum tree_code code) {
  switch (code) {
    case LT_EXPR:
      return GE_EXPR;
    case LE_EXPR:
      return GT_EXPR;
    case GT_EXPR:
      return LE_EXPR;
    case GE_EXPR:
      return LT_EXPR;
    case EQ_EXPR:
      return NE_EXPR;
    case NE_EXPR:
      return EQ_EXPR;
    default:
      return ERROR_MARK;
  }
}

bool unsigned_integer_constant(tree value, unsigned int precision,
                               std::uint64_t *result) {
  if (value == NULL_TREE || result == nullptr ||
      TREE_CODE(value) != INTEGER_CST || TREE_TYPE(value) == NULL_TREE ||
      !INTEGRAL_TYPE_P(TREE_TYPE(value)) ||
      !TYPE_UNSIGNED(TREE_TYPE(value)) ||
      TYPE_PRECISION(TREE_TYPE(value)) != precision ||
      !tree_fits_uhwi_p(value))
    return false;
  *result = static_cast<std::uint64_t>(tree_to_uhwi(value));
  return true;
}

bool unsigned_condition(enum tree_code code, std::uint64_t lhs,
                        std::uint64_t rhs, bool *result) {
  if (result == nullptr)
    return false;
  switch (code) {
    case LT_EXPR:
      *result = lhs < rhs;
      return true;
    case LE_EXPR:
      *result = lhs <= rhs;
      return true;
    case GT_EXPR:
      *result = lhs > rhs;
      return true;
    case GE_EXPR:
      *result = lhs >= rhs;
      return true;
    case EQ_EXPR:
      *result = lhs == rhs;
      return true;
    case NE_EXPR:
      *result = lhs != rhs;
      return true;
    default:
      return false;
  }
}

// Recognize the deliberately small theorem used by the symbolic searches:
//
//       preheader                 exit
//            \\                   /
//             header(condition)
//                   \\           /
//                    one-block body
//                         |
//                         +----> header
//
// HEADER's condition is driven by an unsigned PHI with a constant preheader
// value and `next = phi + positive_constant` on the sole backedge.  We only
// accept a normal two-way GIMPLE_COND, a unique one-block body/backedge, no
// relevant event/provenance edge in the region, and a mathematical crossing
// of an LT/LE bound before modular wrap.  Thus the normal EXIT edge is known
// to occur after finitely many iterations.  No general loop behavior is
// inferred; every shape outside this proof remains subject to the visit cap.
bool strict_neutral_counted_loop(
    basic_block header, const std::set<gimple *> &relevant_statements,
    const std::set<edge> &required_edges, NeutralCountedLoop *result,
    QueryBudget *budget) {
  if (!budget->time_available() || header == nullptr || result == nullptr ||
      EDGE_COUNT(header->succs) != 2 || EDGE_COUNT(header->preds) != 2 ||
      block_contains_relevant_statement(header, relevant_statements, budget))
    return false;

  gimple *terminator = block_terminator(header);
  if (terminator == nullptr || gimple_code(terminator) != GIMPLE_COND)
    return false;
  if (!counted_loop_header_is_bounded(header, terminator, budget))
    return false;

  edge branches[2] = {nullptr, nullptr};
  unsigned int branch_count = 0;
  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, header->succs) {
    if (branch_count >= 2 || !ordinary_edge(successor) ||
        required_edge_p(successor, required_edges))
      return false;
    branches[branch_count++] = successor;
  }
  if (branch_count != 2)
    return false;
  const bool first_true = (branches[0]->flags & EDGE_TRUE_VALUE) != 0;
  const bool first_false = (branches[0]->flags & EDGE_FALSE_VALUE) != 0;
  const bool second_true = (branches[1]->flags & EDGE_TRUE_VALUE) != 0;
  const bool second_false = (branches[1]->flags & EDGE_FALSE_VALUE) != 0;
  if (!((first_true && !first_false && second_false && !second_true) ||
        (first_false && !first_true && second_true && !second_false)))
    return false;

  edge body_edge = nullptr;
  edge exit_edge = nullptr;
  edge backedge = nullptr;
  for (unsigned int index = 0; index < 2; ++index) {
    basic_block candidate = branches[index]->dest;
    edge candidate_backedge = nullptr;
    if (candidate == nullptr || candidate == header ||
        candidate == EXIT_BLOCK_PTR_FOR_FN(cfun) ||
        EDGE_COUNT(candidate->preds) != 1 ||
        single_pred_edge(candidate) != branches[index] ||
        !single_ordinary_successor(candidate, &candidate_backedge) ||
        candidate_backedge->dest != header)
      continue;
    if (body_edge != nullptr)
      return false;
    body_edge = branches[index];
    exit_edge = branches[1 - index];
    backedge = candidate_backedge;
  }
  if (body_edge == nullptr || exit_edge == nullptr || backedge == nullptr ||
      exit_edge->dest == nullptr || exit_edge->dest == header ||
      exit_edge->dest == body_edge->dest ||
      required_edge_p(backedge, required_edges) ||
      block_contains_relevant_statement(body_edge->dest,
                                        relevant_statements, budget) ||
      !counted_loop_body_is_bounded(body_edge->dest, budget))
    return false;

  edge preheader = nullptr;
  edge predecessor;
  FOR_EACH_EDGE(predecessor, iterator, header->preds) {
    if (predecessor == backedge)
      continue;
    if (preheader != nullptr)
      return false;
    preheader = predecessor;
  }
  if (!ordinary_edge(preheader) || preheader->src == header ||
      preheader->src == body_edge->dest ||
      required_edge_p(preheader, required_edges))
    return false;

  tree induction = gimple_cond_lhs(terminator);
  tree bound_tree = gimple_cond_rhs(terminator);
  enum tree_code condition_code = gimple_cond_code(terminator);
  gimple *definition =
      induction != NULL_TREE && TREE_CODE(induction) == SSA_NAME
          ? SSA_NAME_DEF_STMT(induction)
          : nullptr;
  if (definition == nullptr || gimple_code(definition) != GIMPLE_PHI ||
      gimple_bb(definition) != header) {
    std::swap(induction, bound_tree);
    condition_code = swap_comparison(condition_code);
    definition =
        induction != NULL_TREE && TREE_CODE(induction) == SSA_NAME
            ? SSA_NAME_DEF_STMT(induction)
            : nullptr;
  }
  if (condition_code == ERROR_MARK || definition == nullptr ||
      gimple_code(definition) != GIMPLE_PHI ||
      gimple_bb(definition) != header || TREE_TYPE(induction) == NULL_TREE ||
      !INTEGRAL_TYPE_P(TREE_TYPE(induction)) ||
      !TYPE_UNSIGNED(TREE_TYPE(induction)))
    return false;

  const unsigned int precision = TYPE_PRECISION(TREE_TYPE(induction));
  if (precision == 0 || precision > 64 || gimple_phi_num_args(definition) != 2)
    return false;

  const gphi *phi = as_a<gphi *>(definition);
  tree initial_tree = NULL_TREE;
  tree next_tree = NULL_TREE;
  for (unsigned int index = 0; index < 2; ++index) {
    edge incoming = gimple_phi_arg_edge(phi, index);
    if (incoming == preheader)
      initial_tree = gimple_phi_arg_def(phi, index);
    else if (incoming == backedge)
      next_tree = gimple_phi_arg_def(phi, index);
    else
      return false;
  }
  if (initial_tree == NULL_TREE || next_tree == NULL_TREE ||
      TREE_CODE(next_tree) != SSA_NAME)
    return false;

  gimple *increment = SSA_NAME_DEF_STMT(next_tree);
  if (increment == nullptr || !is_gimple_assign(increment) ||
      gimple_bb(increment) != body_edge->dest ||
      gimple_assign_rhs_code(increment) != PLUS_EXPR)
    return false;
  tree add_lhs = gimple_assign_rhs1(increment);
  tree add_rhs = gimple_assign_rhs2(increment);
  tree step_tree = NULL_TREE;
  if (add_lhs == induction)
    step_tree = add_rhs;
  else if (add_rhs == induction)
    step_tree = add_lhs;
  else
    return false;

  std::uint64_t initial = 0;
  std::uint64_t bound = 0;
  std::uint64_t step = 0;
  if (!unsigned_integer_constant(initial_tree, precision, &initial) ||
      !unsigned_integer_constant(step_tree, precision, &step) || step == 0)
    return false;

  // Normalize to the predicate under which the body/backedge is taken.
  if ((body_edge->flags & EDGE_FALSE_VALUE) != 0)
    condition_code = negate_comparison(condition_code);
  if (condition_code == ERROR_MARK)
    return false;

  const bool constant_bound =
      unsigned_integer_constant(bound_tree, precision, &bound);
  if (!constant_bound) {
    // For every value B of an unsigned type, `i = C; i < B; ++i`
    // either exits immediately (C >= B) or reaches B without wrapping.  This
    // is a universally quantified termination theorem, so B need not be
    // constant.  Keep it deliberately exact: a step other than one can skip
    // over B and wrap, <= can wrap when B is MAX, and a loop-carried bound can
    // change during the recurrence.
    if (condition_code != LT_EXPR || step != 1 ||
        bound_tree == NULL_TREE || TREE_CODE(bound_tree) != SSA_NAME ||
        TREE_TYPE(bound_tree) == NULL_TREE ||
        !INTEGRAL_TYPE_P(TREE_TYPE(bound_tree)) ||
        !TYPE_UNSIGNED(TREE_TYPE(bound_tree)) ||
        TYPE_PRECISION(TREE_TYPE(bound_tree)) != precision)
      return false;
    gimple *bound_definition = SSA_NAME_DEF_STMT(bound_tree);
    if (bound_definition != nullptr &&
        (gimple_bb(bound_definition) == header ||
         gimple_bb(bound_definition) == body_edge->dest))
      return false;
    result->exit = exit_edge->dest;
    result->interior_blocks = 1;
    return true;
  }

  bool enters_body = false;
  if (!unsigned_condition(condition_code, initial, bound, &enters_body))
    return false;
  if (enters_body) {
    const std::uint64_t maximum =
        precision == 64
            ? UINT64_MAX
            : (static_cast<std::uint64_t>(1) << precision) - 1;
    std::uint64_t last_before_exit = initial;
    if (condition_code == LT_EXPR) {
      const std::uint64_t distance = bound - initial;
      const std::uint64_t increments =
          distance / step + (distance % step != 0 ? 1 : 0);
      if (increments == 0)
        return false;
      last_before_exit = initial + (increments - 1) * step;
    } else if (condition_code == LE_EXPR) {
      if (bound == maximum)
        return false;
      const std::uint64_t distance = bound - initial;
      last_before_exit = initial + (distance / step) * step;
    } else {
      return false;
    }
    // The next recurrence value crosses the bound without modular wrap.
    if (step > maximum - last_before_exit)
      return false;
  }

  result->exit = exit_edge->dest;
  result->interior_blocks = 1;
  return true;
}

bool exact_integer_constant(tree value, tree type) {
  return value != NULL_TREE && type != NULL_TREE &&
         TREE_CODE(value) == INTEGER_CST && TREE_TYPE(value) != NULL_TREE &&
         INTEGRAL_TYPE_P(type) && INTEGRAL_TYPE_P(TREE_TYPE(value)) &&
         TYPE_PRECISION(TREE_TYPE(value)) == TYPE_PRECISION(type) &&
         TYPE_UNSIGNED(TREE_TYPE(value)) == TYPE_UNSIGNED(type) &&
         !TREE_OVERFLOW(value);
}

bool wide_integer_condition(enum tree_code code, const wide_int &lhs,
                            const wide_int &rhs, signop signedness,
                            bool *result) {
  if (result == nullptr)
    return false;
  switch (code) {
    case LT_EXPR:
      *result = wi::lt_p(lhs, rhs, signedness);
      return true;
    case LE_EXPR:
      *result = wi::le_p(lhs, rhs, signedness);
      return true;
    case GT_EXPR:
      *result = wi::gt_p(lhs, rhs, signedness);
      return true;
    case GE_EXPR:
      *result = wi::ge_p(lhs, rhs, signedness);
      return true;
    case EQ_EXPR:
      *result = wi::eq_p(lhs, rhs);
      return true;
    case NE_EXPR:
      *result = wi::ne_p(lhs, rhs);
      return true;
    default:
      return false;
  }
}

// Prove that the body has one canonical way to re-enter HEADER.  Paths that
// return, abort, or otherwise leave before the latch are retained by the
// ordinary traversal; they do not invalidate the local header transition.
// Reject every unmodeled edge because a negative reachability claim must not
// be based on silently dropping exceptional or abnormal control flow.
bool exact_one_trip_body_region(edge body, basic_block header, edge latch,
                                QueryBudget *budget) {
  if (body == nullptr || body->dest == nullptr || header == nullptr ||
      latch == nullptr || latch->src == nullptr || latch->dest != header)
    return false;

  std::deque<basic_block> worklist;
  std::set<basic_block> visited;
  worklist.push_back(body->dest);
  bool saw_latch = false;

  while (!worklist.empty()) {
    basic_block block = worklist.front();
    worklist.pop_front();
    if (!visited.insert(block).second)
      continue;
    if (!budget->consume_symbolic_state(false) || block == header)
      return false;

    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if (!budget->time_available() || !ordinary_edge(successor))
        return false;
      if (successor->dest == header) {
        if (successor != latch)
          return false;
        saw_latch = true;
        continue;
      }
      worklist.push_back(successor->dest);
    }
  }
  return saw_latch;
}

// Exhaustively prove, in the current intraprocedural CFG, that taking EXIT
// cannot recycle the one-trip schedule through either the latch or header.
// This is deliberately stronger than merely observing that EXIT != BODY.
bool exact_one_trip_exit_region(edge exit, basic_block header, edge latch,
                                QueryBudget *budget) {
  if (exit == nullptr || exit->dest == nullptr || header == nullptr ||
      latch == nullptr || latch->src == nullptr)
    return false;

  std::deque<basic_block> worklist;
  std::set<basic_block> visited;
  worklist.push_back(exit->dest);
  while (!worklist.empty()) {
    basic_block block = worklist.front();
    worklist.pop_front();
    if (!visited.insert(block).second)
      continue;
    if (!budget->consume_symbolic_state(false) || block == header ||
        block == latch->src)
      return false;

    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if (!budget->time_available() || !ordinary_edge(successor))
        return false;
      worklist.push_back(successor->dest);
    }
  }
  return true;
}

// Recognize a two-input affine PHI whose condition changes branch after one
// defined addition.  This is a local two-transition theorem rather than a
// general trip-count guess.  In particular, signed overflow, modular wrap,
// runtime bounds, multiple latches, and exit paths that can re-enter the loop
// all fail closed and remain subject to the normal visit cap.
bool strict_exact_one_trip_loop(
    basic_block header, const std::set<gimple *> &relevant_statements,
    ExactOneTripLoop *result, QueryBudget *budget) {
  if (!budget->time_available() || header == nullptr || result == nullptr ||
      EDGE_COUNT(header->succs) != 2 || EDGE_COUNT(header->preds) != 2 ||
      block_contains_relevant_statement(header, relevant_statements, budget))
    return false;

  gimple *terminator = block_terminator(header);
  if (terminator == nullptr || gimple_code(terminator) != GIMPLE_COND ||
      !counted_loop_header_is_bounded(header, terminator, budget))
    return false;

  edge true_edge = nullptr;
  edge false_edge = nullptr;
  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, header->succs) {
    if (!ordinary_edge(successor) || successor->dest == nullptr ||
        successor->dest == header)
      return false;
    const bool is_true = (successor->flags & EDGE_TRUE_VALUE) != 0;
    const bool is_false = (successor->flags & EDGE_FALSE_VALUE) != 0;
    if (is_true == is_false)
      return false;
    if (is_true) {
      if (true_edge != nullptr)
        return false;
      true_edge = successor;
    } else {
      if (false_edge != nullptr)
        return false;
      false_edge = successor;
    }
  }
  if (true_edge == nullptr || false_edge == nullptr ||
      true_edge->dest == false_edge->dest)
    return false;

  tree induction = gimple_cond_lhs(terminator);
  tree bound_tree = gimple_cond_rhs(terminator);
  enum tree_code condition_code = gimple_cond_code(terminator);
  gimple *definition =
      induction != NULL_TREE && TREE_CODE(induction) == SSA_NAME
          ? SSA_NAME_DEF_STMT(induction)
          : nullptr;
  if (definition == nullptr || gimple_code(definition) != GIMPLE_PHI ||
      gimple_bb(definition) != header) {
    std::swap(induction, bound_tree);
    condition_code = swap_comparison(condition_code);
    definition =
        induction != NULL_TREE && TREE_CODE(induction) == SSA_NAME
            ? SSA_NAME_DEF_STMT(induction)
            : nullptr;
  }
  if (condition_code == ERROR_MARK || definition == nullptr ||
      gimple_code(definition) != GIMPLE_PHI ||
      gimple_bb(definition) != header || TREE_TYPE(induction) == NULL_TREE ||
      !INTEGRAL_TYPE_P(TREE_TYPE(induction)) ||
      TYPE_PRECISION(TREE_TYPE(induction)) == 0 ||
      gimple_phi_num_args(definition) != 2)
    return false;

  const gphi *phi = as_a<gphi *>(definition);
  tree initial_tree = NULL_TREE;
  tree next_tree = NULL_TREE;
  edge preheader = nullptr;
  edge latch = nullptr;
  for (unsigned int index = 0; index < 2; ++index) {
    tree argument = gimple_phi_arg_def(phi, index);
    edge incoming = gimple_phi_arg_edge(phi, index);
    if (argument != NULL_TREE && TREE_CODE(argument) == INTEGER_CST) {
      if (initial_tree != NULL_TREE)
        return false;
      initial_tree = argument;
      preheader = incoming;
    } else if (argument != NULL_TREE && TREE_CODE(argument) == SSA_NAME) {
      if (next_tree != NULL_TREE)
        return false;
      next_tree = argument;
      latch = incoming;
    } else {
      return false;
    }
  }
  if (!ordinary_edge(preheader) || !ordinary_edge(latch) ||
      preheader == latch || preheader->dest != header ||
      latch->dest != header || preheader->src == nullptr ||
      latch->src == nullptr || preheader->src == header ||
      latch->src == header || preheader->src == latch->src)
    return false;

  gimple *increment = SSA_NAME_DEF_STMT(next_tree);
  if (increment == nullptr || !is_gimple_assign(increment) ||
      gimple_bb(increment) != latch->src ||
      gimple_assign_lhs(increment) != next_tree ||
      gimple_assign_rhs_code(increment) != PLUS_EXPR ||
      gimple_has_volatile_ops(increment))
    return false;
  tree add_lhs = gimple_assign_rhs1(increment);
  tree add_rhs = gimple_assign_rhs2(increment);
  tree step_tree = NULL_TREE;
  if (add_lhs == induction)
    step_tree = add_rhs;
  else if (add_rhs == induction)
    step_tree = add_lhs;
  else
    return false;

  tree type = TREE_TYPE(induction);
  if (!exact_integer_constant(initial_tree, type) ||
      !exact_integer_constant(bound_tree, type) ||
      !exact_integer_constant(step_tree, type))
    return false;

  const signop signedness = TYPE_UNSIGNED(type) ? UNSIGNED : SIGNED;
  const wide_int initial = wi::to_wide(initial_tree);
  const wide_int bound = wi::to_wide(bound_tree);
  const wide_int step = wi::to_wide(step_tree);
  wi::overflow_type overflow = wi::OVF_NONE;
  const wide_int next = wi::add(initial, step, signedness, &overflow);
  // wi::add reports wrap even for unsigned types.  Reject it intentionally:
  // this keeps the proof independent of language overflow modes.
  if (overflow != wi::OVF_NONE)
    return false;

  bool initial_condition = false;
  bool next_condition = false;
  if (!wide_integer_condition(condition_code, initial, bound, signedness,
                              &initial_condition) ||
      !wide_integer_condition(condition_code, next, bound, signedness,
                              &next_condition) ||
      initial_condition == next_condition)
    return false;

  edge body = initial_condition ? true_edge : false_edge;
  edge exit = next_condition ? true_edge : false_edge;
  if (body == exit ||
      !exact_one_trip_body_region(body, header, latch, budget) ||
      !exact_one_trip_exit_region(exit, header, latch, budget))
    return false;

  result->preheader = preheader;
  result->latch = latch;
  result->body = body;
  result->exit = exit;
  return true;
}

// One certificate implementation shared by event-sequence and avoid-release
// searches. Each instance belongs to one query with fixed relevance barriers.
class ExactOneTripCache {
 public:
  bool lookup(basic_block header,
              const std::set<gimple *> &relevant_statements,
              ExactOneTripLoop *result, QueryBudget *budget) {
    const auto found = proven_.find(header);
    if (found != proven_.end()) {
      *result = found->second;
      return true;
    }
    if (rejected_.count(header) != 0)
      return false;
    ExactOneTripLoop candidate;
    if (!strict_exact_one_trip_loop(header, relevant_statements, &candidate,
                                    budget)) {
      rejected_.insert(header);
      return false;
    }
    proven_.emplace(header, candidate);
    *result = candidate;
    return true;
  }

 private:
  std::map<basic_block, ExactOneTripLoop> proven_;
  std::set<basic_block> rejected_;
};

#endif

void account(const Result &result) {
  ++accumulated.queries;
  if (result.tu_budget_exhausted) {
    ++accumulated.tu_budget_exhaustions;
    if (result.reason == tu_budget_reason(TuBudgetLimit::timeout))
      ++accumulated.tu_timeout_exhaustions;
    else if (result.reason == tu_budget_reason(TuBudgetLimit::queries))
      ++accumulated.tu_query_cap_exhaustions;
    else if (result.reason == tu_budget_reason(TuBudgetLimit::cfg_states))
      ++accumulated.tu_cfg_state_cap_exhaustions;
    else if (result.reason ==
             tu_budget_reason(TuBudgetLimit::symbolic_states))
      ++accumulated.tu_symbolic_state_cap_exhaustions;
    else if (result.reason ==
             tu_budget_reason(TuBudgetLimit::solver_checks))
      ++accumulated.tu_solver_check_cap_exhaustions;
  }
  if (result.used_solver)
    ++accumulated.solver_queries;
  else
    ++accumulated.cfg_only_queries;
  if (result.solver_initialized)
    ++accumulated.solver_initializations;
  if (result.verdict == Verdict::feasible)
    ++accumulated.feasible;
  else if (result.verdict == Verdict::infeasible)
    ++accumulated.infeasible;
  else
    ++accumulated.unknown;
  if (result.truncated)
    ++accumulated.truncated;
  accumulated.cfg_states += result.cfg_states;
  accumulated.symbolic_states += result.symbolic_states;
  accumulated.solver_checks += result.solver_checks;
  accumulated.solver_prunes += result.solver_prunes;
  accumulated.exact_null_checks += result.exact_null_checks;
  accumulated.exact_null_prunes += result.exact_null_prunes;
  accumulated.exact_scalar_checks += result.exact_scalar_checks;
  accumulated.exact_scalar_prunes += result.exact_scalar_prunes;
  accumulated.abstracted_diamonds += result.abstracted_diamonds;
  accumulated.abstracted_loops += result.abstracted_loops;
  accumulated.exact_one_trip_unfolds += result.exact_one_trip_unfolds;
  accumulated.abstracted_blocks += result.abstracted_blocks;
}

Result finish(Result result) {
  account(result);
  return result;
}

#ifdef STATIC_ANALYZER_HAVE_Z3

struct EncodedScalar {
  EncodedScalar(z3::context &context)
      : available(false), exact(false), width(0), value(context) {}
  EncodedScalar(const z3::expr &expression, unsigned int bit_width,
                bool is_exact)
      : available(true), exact(is_exact), width(bit_width), value(expression) {}

  bool available;
  bool exact;
  unsigned int width;
  z3::expr value;
};

struct EncodedCondition {
  explicit EncodedCondition(z3::context &context)
      : available(false), exact(false), value(context) {}
  EncodedCondition(const z3::expr &expression, bool is_exact)
      : available(true), exact(is_exact), value(expression) {}

  bool available;
  bool exact;
  z3::expr value;
};

struct ConstantReturnSummary {
  bool resolved = false;
  // A structural rejection is stable for the translation unit and may be
  // cached.  A QueryBudget interruption is not: another feasibility query
  // receives its own per-query allowance, so caching that temporary failure
  // would make recall depend on which call site happened to be visited first.
  bool cacheable = true;
  tree value = NULL_TREE;
};

// This is deliberately much smaller than the ownership-summary budget.  It
// only serves path-condition expansion, so reaching any bound must leave the
// call result unconstrained instead of adding latency to every feasibility
// query in a large TU.
constexpr unsigned int max_constant_return_cache_entries = 512;
constexpr unsigned int max_constant_return_blocks = 1024;
constexpr unsigned int max_constant_return_statements = 8192;
constexpr unsigned int max_constant_return_edges = 4096;

std::map<cgraph_node *, ConstantReturnSummary> constant_return_cache;

void dump_constant_return_decision(const gimple *statement,
                                   const char *decision) {
  if (dump_file == nullptr || !(dump_flags & TDF_DETAILS))
    return;
  tree declaration = statement != nullptr && is_gimple_call(statement)
                         ? gimple_call_fndecl(statement)
                         : NULL_TREE;
  const char *name = declaration != NULL_TREE &&
                             DECL_NAME(declaration) != NULL_TREE
                         ? IDENTIFIER_POINTER(DECL_NAME(declaration))
                         : "<indirect>";
  dump_printf(MSG_NOTE,
              "constant-return-summary: callee=%s decision=%s\n", name,
              decision);
}

cgraph_node *body_bound_direct_target(const gimple *statement) {
  if (statement == nullptr || !is_gimple_call(statement) || cfun == nullptr ||
      cfun->decl == NULL_TREE ||
      (gimple_call_flags(statement) & ECF_RETURNS_TWICE) != 0 ||
      gimple_call_noreturn_p(statement)) {
    dump_constant_return_decision(statement, "reject-call-semantics");
    return nullptr;
  }

  cgraph_node *caller = cgraph_node::get(cfun->decl);
  if (caller == nullptr) {
    dump_constant_return_decision(statement, "reject-no-caller-node");
    return nullptr;
  }
  cgraph_edge *call_edge =
      caller->get_edge(const_cast<gimple *>(statement));
  if (call_edge == nullptr || call_edge->callee == nullptr ||
      call_edge->indirect_unknown_callee || call_edge->speculative ||
      !call_edge->binds_to_current_def_p()) {
    dump_constant_return_decision(statement, "reject-unbound-edge");
    return nullptr;
  }

  enum availability availability = AVAIL_NOT_AVAILABLE;
  cgraph_node *target =
      call_edge->callee->ultimate_alias_target(&availability, caller);
  // Thunks may adjust the actual arguments or returned value.  Following one
  // into its implementation is not an exact call-result/body theorem.
  if (target == nullptr || !target->has_gimple_body_p() ||
      availability < AVAIL_AVAILABLE ||
      !target->definition || target->decl == NULL_TREE ||
      TREE_CODE(target->decl) != FUNCTION_DECL || DECL_WEAK(target->decl) ||
      DECL_IS_RETURNS_TWICE(target->decl) || target->ifunc_resolver ||
      lookup_attribute("ifunc", DECL_ATTRIBUTES(target->decl)) != NULL_TREE ||
      (call_edge->callee->decl != NULL_TREE &&
       lookup_attribute("ifunc", DECL_ATTRIBUTES(call_edge->callee->decl)) !=
           NULL_TREE) ||
      !gimple_has_body_p(target->decl) || target->get_fun() == nullptr) {
    dump_constant_return_decision(statement, "reject-replaceable-target");
    return nullptr;
  }
  return target;
}

// A constant value theorem is only useful to path feasibility if the call is
// also known to return normally.  Restrict the first implementation to a
// local, call-free GIMPLE DAG.  The mask admits only annotations on ordinary
// GIMPLE edges; fake, EH, abnormal, transactional, ignored, sibcall, and any
// future unrecognized edge kind fail closed.
bool constant_return_edge_p(edge candidate) {
  if (candidate == nullptr || candidate->dest == nullptr)
    return false;
  const unsigned int normal_flags =
      EDGE_FALLTHRU | EDGE_DFS_BACK | EDGE_IRREDUCIBLE_LOOP |
      EDGE_TRUE_VALUE | EDGE_FALSE_VALUE | EDGE_EXECUTABLE | EDGE_CROSSING |
      EDGE_CAN_FALLTHRU | EDGE_LOOP_EXIT;
  return (static_cast<unsigned int>(candidate->flags) & ~normal_flags) == 0;
}

bool constant_return_statement_p(const gimple *statement) {
  if (statement == nullptr || gimple_has_volatile_ops(statement))
    return false;
  switch (gimple_code(statement)) {
    case GIMPLE_DEBUG:
    case GIMPLE_LABEL:
    case GIMPLE_NOP:
    case GIMPLE_PREDICT:
    case GIMPLE_ASSIGN:
    case GIMPLE_COND:
    case GIMPLE_GOTO:
    case GIMPLE_SWITCH:
    case GIMPLE_RETURN:
      return true;
    default:
      return false;
  }
}

ConstantReturnSummary compute_constant_return_summary(cgraph_node *target,
                                                       QueryBudget *budget) {
  ConstantReturnSummary result;
  if (target == nullptr || target->decl == NULL_TREE)
    return result;
  if (budget == nullptr) {
    result.cacheable = false;
    return result;
  }

  const auto consume_scan_state = [&]() {
    if (budget->consume_symbolic_state(false))
      return true;
    result.cacheable = false;
    return false;
  };

  tree function_type = TREE_TYPE(target->decl);
  tree return_type = function_type == NULL_TREE ? NULL_TREE
                                                : TREE_TYPE(function_type);
  if (return_type == NULL_TREE || !INTEGRAL_TYPE_P(return_type) ||
      TYPE_PRECISION(return_type) == 0)
    return result;

  function *body = target->get_fun();
  basic_block entry = ENTRY_BLOCK_PTR_FOR_FN(body);
  basic_block exit = EXIT_BLOCK_PTR_FOR_FN(body);
  if (entry == nullptr || exit == nullptr)
    return result;

  std::deque<basic_block> worklist;
  std::set<basic_block> visited;
  std::map<basic_block, std::vector<basic_block>> successors;
  std::map<basic_block, unsigned int> indegree;
  worklist.push_back(entry);
  unsigned int statement_count = 0;
  unsigned int edge_count = 0;
  tree constant = NULL_TREE;
  bool saw_normal_return = false;

  while (!worklist.empty()) {
    basic_block block = worklist.front();
    worklist.pop_front();
    if (!visited.insert(block).second)
      continue;
    if (!consume_scan_state() ||
        visited.size() > max_constant_return_blocks)
      return result;
    indegree.insert(std::make_pair(block, 0));

    bool block_returns = false;
    if (block != entry && block != exit) {
      for (gphi_iterator iterator = gsi_start_phis(block);
           !gsi_end_p(iterator); gsi_next(&iterator)) {
        if (!consume_scan_state() ||
            ++statement_count > max_constant_return_statements)
          return result;
      }
      for (gimple_stmt_iterator iterator = gsi_start_bb(block);
           !gsi_end_p(iterator); gsi_next(&iterator)) {
        if (!consume_scan_state() ||
            ++statement_count > max_constant_return_statements)
          return result;
        gimple *statement = gsi_stmt(iterator);
        if (!constant_return_statement_p(statement))
          return result;
        if (gimple_code(statement) != GIMPLE_RETURN)
          continue;

        if (block_returns || statement != block_terminator(block))
          return result;

        tree returned =
            gimple_return_retval(as_a<greturn *>(statement));
        if (returned == NULL_TREE || TREE_CODE(returned) != INTEGER_CST ||
            TREE_TYPE(returned) == NULL_TREE ||
            !INTEGRAL_TYPE_P(TREE_TYPE(returned)) ||
            TYPE_PRECISION(TREE_TYPE(returned)) !=
                TYPE_PRECISION(return_type) ||
            TYPE_UNSIGNED(TREE_TYPE(returned)) !=
                TYPE_UNSIGNED(return_type) ||
            TREE_OVERFLOW(returned))
          return result;
        if (constant == NULL_TREE) {
          constant = returned;
        } else if (TYPE_PRECISION(TREE_TYPE(constant)) !=
                       TYPE_PRECISION(TREE_TYPE(returned)) ||
                   TYPE_UNSIGNED(TREE_TYPE(constant)) !=
                       TYPE_UNSIGNED(TREE_TYPE(returned)) ||
                   !tree_int_cst_equal(constant, returned)) {
          return result;
        }
        saw_normal_return = true;
        block_returns = true;
      }
    }

    if (block == exit)
      continue;

    unsigned int traversed_successors = 0;
    edge successor;
    edge_iterator edge_iterator;
    FOR_EACH_EDGE(successor, edge_iterator, block->succs) {
      if (!consume_scan_state() ||
          ++edge_count > max_constant_return_edges ||
          !constant_return_edge_p(successor))
        return result;
      if (block_returns != (successor->dest == exit))
        return result;
      ++traversed_successors;
      successors[block].push_back(successor->dest);
      ++indegree[successor->dest];
      worklist.push_back(successor->dest);
    }
    // Reject fake-exit-only noreturn blocks, unreachable builtins, and any
    // other modeled sink.  In a finite DAG every maximal path must end in the
    // unique synthetic EXIT through an identical GIMPLE_RETURN.
    if (traversed_successors == 0)
      return result;
  }

  if (!saw_normal_return || visited.find(exit) == visited.end())
    return result;

  // Kahn's algorithm proves that the entire reachable CFG is acyclic.  This
  // is stronger than ignoring backedge annotations: an infinite self-loop or
  // an unmarked irreducible cycle cannot turn a conditional return theorem
  // into a claim that this particular call returns.
  std::deque<basic_block> roots;
  for (basic_block block : visited) {
    const auto found = indegree.find(block);
    if (found == indegree.end() || found->second == 0)
      roots.push_back(block);
  }
  unsigned int sorted_blocks = 0;
  while (!roots.empty()) {
    basic_block block = roots.front();
    roots.pop_front();
    ++sorted_blocks;
    const auto found = successors.find(block);
    if (found == successors.end())
      continue;
    for (basic_block successor : found->second) {
      auto degree = indegree.find(successor);
      if (degree == indegree.end() || degree->second == 0)
        return result;
      --degree->second;
      if (degree->second == 0)
        roots.push_back(successor);
    }
  }
  if (sorted_blocks != visited.size())
    return result;
  result.resolved = true;
  result.value = constant;
  return result;
}

tree body_bound_integer_constant_return(const gimple *statement,
                                        QueryBudget *budget) {
  cgraph_node *target = body_bound_direct_target(statement);
  if (target == nullptr)
    return NULL_TREE;

  const auto cached = constant_return_cache.find(target);
  if (cached != constant_return_cache.end())
    return cached->second.resolved ? cached->second.value : NULL_TREE;

  if (constant_return_cache.size() >= max_constant_return_cache_entries)
    return NULL_TREE;
  const ConstantReturnSummary summary =
      compute_constant_return_summary(target, budget);
  if (summary.cacheable)
    constant_return_cache[target] = summary;
  dump_constant_return_decision(
      statement,
      summary.resolved
          ? "accept-identical-integer"
          : summary.cacheable ? "reject-body-proof" : "defer-budget");
  return summary.resolved ? summary.value : NULL_TREE;
}

// Exact pointer projection is independent of Z3. Both the finite NULL domain
// and SMT encoding must use this same canonical SSA identity and VSSA proof.
class ExactPointerValues {
 public:
  explicit ExactPointerValues(QueryBudget *budget) : budget_(budget) {}

  static bool null_pointer_constant(tree value) {
    return value != NULL_TREE && TREE_CODE(value) == INTEGER_CST &&
           integer_zerop(value);
  }

  static bool identical_pointer_representation(tree left, tree right) {
    if (left == NULL_TREE || right == NULL_TREE ||
        TREE_TYPE(left) == NULL_TREE || TREE_TYPE(right) == NULL_TREE)
      return false;
    tree left_type = TREE_TYPE(left);
    tree right_type = TREE_TYPE(right);
    return POINTER_TYPE_P(left_type) && POINTER_TYPE_P(right_type) &&
           TYPE_PRECISION(left_type) == TYPE_PRECISION(right_type) &&
           TYPE_MODE(left_type) == TYPE_MODE(right_type) &&
           TYPE_ADDR_SPACE(TREE_TYPE(left_type)) ==
               TYPE_ADDR_SPACE(TREE_TYPE(right_type));
  }

  // One exact SSA/value-memory step, shared by expression encoding and the
  // allocation-origin check.  Merely recognizing a reloaded allocation in
  // the latter is not sufficient: its condition must use the original SSA
  // symbol too, or p != NULL and a reload of p == NULL could both be SAT.
  tree pointer_value_source(tree value) {
    if (value == NULL_TREE || TREE_CODE(value) != SSA_NAME ||
        TREE_TYPE(value) == NULL_TREE || !POINTER_TYPE_P(TREE_TYPE(value)))
      return NULL_TREE;
    const auto cached = pointer_sources_.find(value);
    if (cached != pointer_sources_.end())
      return cached->second;
    if (!budget_->consume_symbolic_state(false))
      return NULL_TREE;

    tree source = NULL_TREE;
    gimple *definition = SSA_NAME_DEF_STMT(value);
    if (definition != nullptr && is_gimple_assign(definition) &&
        !gimple_has_volatile_ops(definition)) {
      tree rhs = gimple_assign_rhs1(definition);
      const enum tree_code code = gimple_assign_rhs_code(definition);
      if ((gimple_assign_single_p(definition) || code == NOP_EXPR ||
           code == CONVERT_EXPR) &&
          identical_pointer_representation(value, rhs) &&
          (TREE_CODE(rhs) == SSA_NAME || null_pointer_constant(rhs))) {
        source = rhs;
      } else if (gimple_assign_load_p(definition)) {
        // This oracle accepts only one complete MUST_KILL reaching store;
        // entry values, mixed stores, opaque clobbers and volatile loads
        // remain unknown.  Its own VSSA depth/state caps also bound the work
        // before it returns to this query's shared time/state budget.
        const MemoryValueResult memory = resolve_pointer_load(definition);
        const unsigned int work = memory.visited_states + memory.walked_vdefs;
        for (unsigned int index = 0; index < work; ++index)
          if (!budget_->consume_symbolic_state(false))
            return NULL_TREE;
        if (!budget_->time_available())
          return NULL_TREE;
        tree stored = memory.unique_stored_value();
        if (stored != NULL_TREE && memory.definitions.size() == 1 &&
            (TREE_CODE(stored) == SSA_NAME || null_pointer_constant(stored)) &&
            memory.definitions.front().statement != nullptr &&
            !gimple_has_volatile_ops(memory.definitions.front().statement) &&
            identical_pointer_representation(value, stored) &&
            identical_pointer_representation(value, memory.load_reference) &&
            identical_pointer_representation(
                value, memory.definitions.front().memory_reference))
          source = stored;
      }
    }
    pointer_sources_[value] = source;
    return source;
  }

  tree canonical_pointer_value(tree value) {
    const auto cached = pointer_values_.find(value);
    if (cached != pointer_values_.end())
      return cached->second;
    tree current = value;
    std::set<tree> visited;
    for (unsigned int depth = 0; depth < 32; ++depth) {
      if (!budget_->consume_symbolic_state(false) ||
          !visited.insert(current).second)
        break;
      tree source = pointer_value_source(current);
      if (source == NULL_TREE) {
        if (!budget_->time_available())
          return NULL_TREE;
        pointer_values_[value] = current;
        return current;
      }
      current = source;
    }
    // Both consumers must reject the entire projection on a cycle or cap.
    // A partial canonical value cannot authorize a different, deeper symbol
    // merely because the two consumers have different expression stacks.
    pointer_values_[value] = NULL_TREE;
    return NULL_TREE;
  }

  bool authoritative_allocation_result(tree value) {
    tree origin = canonical_pointer_value(value);
    if (origin == NULL_TREE || TREE_CODE(origin) != SSA_NAME)
      return false;
    gimple *definition = SSA_NAME_DEF_STMT(origin);
    return definition != nullptr && is_gimple_call(definition) &&
           (ownership_allocation_call_p(definition) ||
            ownership_reallocation_call_p(definition));
  }

  bool nonnull_released_return(tree value) {
    tree origin = canonical_pointer_value(value);
    if (origin == NULL_TREE || TREE_CODE(origin) != SSA_NAME)
      return false;
    gimple *definition = SSA_NAME_DEF_STMT(origin);
    if (definition == nullptr || !is_gimple_call(definition) ||
        ownership_allocation_call_p(definition) ||
        ownership_reallocation_call_p(definition))
      return false;
    const auto effect = ownership_summary::returned_object_effect(definition);
    return effect.resolved && !effect.may_be_null &&
           effect.lifetime == ownership_summary::ReturnedObjectLifetime::released &&
           budget_->time_available();
  }

 private:
  QueryBudget *budget_;
  std::map<tree, tree> pointer_sources_;
  std::map<tree, tree> pointer_values_;
};

class ConditionEncoder {
 public:
  ConditionEncoder(z3::context &context, QueryBudget *budget,
                   ExactPointerValues &pointer_values)
      : context_(context), budget_(budget), pointer_values_(pointer_values) {}

  EncodedCondition encode(const gimple *statement) {
    if (statement == nullptr || gimple_code(statement) != GIMPLE_COND)
      return EncodedCondition(context_);
    return comparison(gimple_cond_code(statement),
                      gimple_cond_lhs(statement),
                      gimple_cond_rhs(statement));
  }

  EncodedCondition encode(const RequiredPredicate &predicate) {
    const enum tree_code code =
        predicate.relation == ScalarRelation::equal ? EQ_EXPR : NE_EXPR;
    return comparison(code, predicate.lhs, predicate.rhs);
  }

 private:
  static bool scalar_type(tree type) {
    return type != NULL_TREE &&
           (INTEGRAL_TYPE_P(type) || POINTER_TYPE_P(type));
  }

  static bool null_pointer_constant(tree value) {
    return ExactPointerValues::null_pointer_constant(value);
  }

  static bool identical_pointer_representation(tree left, tree right) {
    return ExactPointerValues::identical_pointer_representation(left, right);
  }

  tree canonical_pointer_value(tree value) {
    return pointer_values_.canonical_pointer_value(value);
  }

  bool authoritative_allocation_result(tree value) {
    return pointer_values_.authoritative_allocation_result(value);
  }

  EncodedScalar symbol(tree value, bool exact) {
    tree type = TREE_TYPE(value);
    if (!scalar_type(type))
      return EncodedScalar(context_);
    const unsigned int width = TYPE_PRECISION(type);
    if (width == 0)
      return EncodedScalar(context_);

    auto found = symbols_.find(value);
    if (found != symbols_.end())
      return EncodedScalar(found->second, width, exact);

    std::ostringstream name;
    if (TREE_CODE(value) == SSA_NAME)
      name << "sa_ssa_" << SSA_NAME_VERSION(value);
    else if (DECL_P(value))
      name << "sa_decl_" << DECL_UID(value);
    else
      name << "sa_tree_" << reinterpret_cast<std::uintptr_t>(value);
    z3::expr expression = context_.bv_const(name.str().c_str(), width);
    symbols_.insert(std::make_pair(value, expression));
    return EncodedScalar(expression, width, exact);
  }

  EncodedScalar scalar(tree value, std::set<tree> *active) {
    // Expression expansion is part of the same bounded symbolic query.  A
    // long SSA definition chain must not evade state/time limits merely
    // because it is traversed while constructing a condition.
    if (!budget_->consume_symbolic_state(false))
      return EncodedScalar(context_);
    if (value == NULL_TREE || TREE_TYPE(value) == NULL_TREE ||
        !scalar_type(TREE_TYPE(value)))
      return EncodedScalar(context_);

    const unsigned int width = TYPE_PRECISION(TREE_TYPE(value));
    if (width == 0)
      return EncodedScalar(context_);

    if (TREE_CODE(value) == INTEGER_CST) {
      if (tree_fits_shwi_p(value))
        return EncodedScalar(
            context_.bv_val(static_cast<int64_t>(tree_to_shwi(value)), width),
            width, true);
      if (tree_fits_uhwi_p(value))
        return EncodedScalar(
            context_.bv_val(static_cast<uint64_t>(tree_to_uhwi(value)), width),
            width, true);
      return EncodedScalar(context_);
    }

    if (TREE_CODE(value) == ADDR_EXPR)
      return symbol(value, true);

    if (TREE_CODE(value) == PARM_DECL || TREE_CODE(value) == VAR_DECL ||
        TREE_CODE(value) == RESULT_DECL) {
      // A bare declaration in SSA GIMPLE normally denotes a memory-resident
      // value.  The same tree can therefore be loaded at several different
      // virtual-SSA versions around stores or calls.  Keying those loads by
      // DECL_UID and calling the symbol exact would incorrectly assert that
      // the values cannot change.  Keep a direct declaration conservative;
      // pointer identity is represented by ADDR_EXPR above, not by this case.
      return symbol(value, false);
    }

    if (TREE_CODE(value) != SSA_NAME)
      return symbol(value, false);

    if (!active->insert(value).second)
      return symbol(value, false);

    gimple *definition = SSA_NAME_DEF_STMT(value);
    if (definition == nullptr || gimple_code(definition) == GIMPLE_NOP) {
      active->erase(value);
      // Default definitions represent unconstrained function inputs or
      // externally visible state and are exact bit-vector symbols.
      return symbol(value, true);
    }

    if (is_gimple_call(definition)) {
      tree constant = body_bound_integer_constant_return(definition, budget_);
      if (constant != NULL_TREE && TREE_TYPE(constant) != NULL_TREE &&
          INTEGRAL_TYPE_P(TREE_TYPE(constant)) &&
          TYPE_PRECISION(TREE_TYPE(constant)) == width) {
        EncodedScalar result = scalar(constant, active);
        active->erase(value);
        return result;
      }
    }

    if (is_gimple_assign(definition)) {
      const enum tree_code code = gimple_assign_rhs_code(definition);
      if (POINTER_TYPE_P(TREE_TYPE(value))) {
        tree source = canonical_pointer_value(value);
        if (source != NULL_TREE && source != value) {
          EncodedScalar result = scalar(source, active);
          active->erase(value);
          return result;
        }
      }
      if (gimple_assign_single_p(definition)) {
        tree rhs = gimple_assign_rhs1(definition);
        // Reuse GCC's own linkability/interposition-aware constant-folding
        // oracle for loads such as a TU-local static scalar that is never
        // changed.  Hand-reading DECL_INITIAL here would be unsound for
        // preemptible, externally initialized, or otherwise mutable objects;
        // get_symbol_constant_value deliberately returns NULL_TREE for those.
        if (!gimple_has_volatile_ops(definition) && rhs != NULL_TREE &&
            DECL_P(rhs) && TREE_TYPE(rhs) != NULL_TREE &&
            scalar_type(TREE_TYPE(rhs))) {
          tree constant = get_symbol_constant_value(rhs);
          if (constant != NULL_TREE && TREE_TYPE(constant) != NULL_TREE &&
              scalar_type(TREE_TYPE(constant)) &&
              (identical_pointer_representation(value, constant) ||
               (INTEGRAL_TYPE_P(TREE_TYPE(value)) &&
                INTEGRAL_TYPE_P(TREE_TYPE(constant)))) &&
              TYPE_PRECISION(TREE_TYPE(constant)) == width) {
            EncodedScalar result = scalar(constant, active);
            active->erase(value);
            return result;
          }
        }
        if (rhs != NULL_TREE && TREE_TYPE(rhs) != NULL_TREE &&
            INTEGRAL_TYPE_P(TREE_TYPE(value)) &&
            INTEGRAL_TYPE_P(TREE_TYPE(rhs)) &&
            TYPE_PRECISION(TREE_TYPE(rhs)) == width &&
            (code == SSA_NAME || code == INTEGER_CST)) {
          EncodedScalar result = scalar(rhs, active);
          active->erase(value);
          return result;
        }
      }

      if ((code == BIT_AND_EXPR || code == BIT_IOR_EXPR ||
           code == BIT_XOR_EXPR) &&
          INTEGRAL_TYPE_P(TREE_TYPE(value)) &&
          !gimple_has_volatile_ops(definition)) {
        tree lhs = gimple_assign_rhs1(definition);
        tree rhs = gimple_assign_rhs2(definition);
        // GCC commonly lowers eager Boolean conjunctions to BIT_AND_EXPR.
        // Bitwise operations have exact fixed-width semantics; this does
        // not authorize arithmetic, width-changing casts or pointer ops.
        if (lhs != NULL_TREE && rhs != NULL_TREE &&
            TREE_TYPE(lhs) != NULL_TREE && TREE_TYPE(rhs) != NULL_TREE &&
            INTEGRAL_TYPE_P(TREE_TYPE(lhs)) &&
            INTEGRAL_TYPE_P(TREE_TYPE(rhs)) &&
            TYPE_PRECISION(TREE_TYPE(lhs)) == width &&
            TYPE_PRECISION(TREE_TYPE(rhs)) == width) {
          EncodedScalar left = scalar(lhs, active);
          EncodedScalar right = scalar(rhs, active);
          if (left.available && right.available &&
              left.width == width && right.width == width) {
            z3::expr expression = code == BIT_AND_EXPR
                                      ? left.value & right.value
                                      : code == BIT_IOR_EXPR
                                            ? left.value | right.value
                                            : left.value ^ right.value;
            EncodedScalar result(expression, width, left.exact && right.exact);
            active->erase(value);
            return result;
          }
        }
      }

      if (TREE_CODE_CLASS(code) == tcc_comparison) {
        EncodedCondition condition =
            comparison(code, gimple_assign_rhs1(definition),
                       gimple_assign_rhs2(definition), active);
        if (condition.available) {
          z3::expr one = context_.bv_val(1, width);
          z3::expr zero = context_.bv_val(0, width);
          EncodedScalar result(z3::ite(condition.value, one, zero), width,
                               condition.exact);
          active->erase(value);
          return result;
        }
      }
    }

    active->erase(value);
    // Each unsupported SSA definition gets its own unconstrained symbol.  It
    // is deliberately inexact: the searches below never use an inexact
    // condition to produce an UNSAT proof.
    return symbol(value, false);
  }

  EncodedCondition comparison(enum tree_code code, tree lhs, tree rhs) {
    std::set<tree> active;
    return comparison(code, lhs, rhs, &active);
  }

  EncodedCondition comparison(enum tree_code code, tree lhs, tree rhs,
                              std::set<tree> *active) {
    // A completed body certificate can prove a returned identity nonnull
    // even though its lifetime has ended. Fold ONLY this NULL test: the
    // result is neither a live allocator nor an exact arbitrary pointer
    // symbol, and must not gain a new global freshness/no-alias promise.
    if ((code == EQ_EXPR || code == NE_EXPR) &&
        identical_pointer_representation(lhs, rhs) &&
        ((null_pointer_constant(rhs) &&
          pointer_values_.nonnull_released_return(lhs)) ||
         (null_pointer_constant(lhs) &&
          pointer_values_.nonnull_released_return(rhs))))
      return EncodedCondition(context_.bool_val(code == NE_EXPR), true);
    EncodedScalar left = scalar(lhs, active);
    EncodedScalar right = scalar(rhs, active);
    if (!left.available || !right.available || left.width != right.width)
      return EncodedCondition(context_);

    // A GCC builtin/metadata-backed allocation result is a stable SSA value,
    // so its comparison with NULL is exact even though the call result itself
    // is otherwise unconstrained.  Keep this promotion local to pointer/null
    // EQ/NE.  Treating the result as exact in arbitrary comparisons would let
    // two distinct successful allocations compare equal in the SMT model and
    // could manufacture a feasible witness.
    if (code == EQ_EXPR || code == NE_EXPR) {
      if (null_pointer_constant(rhs) && lhs != NULL_TREE &&
          TREE_TYPE(lhs) != NULL_TREE && POINTER_TYPE_P(TREE_TYPE(lhs)) &&
          authoritative_allocation_result(lhs))
        left = EncodedScalar(left.value, left.width, true);
      if (null_pointer_constant(lhs) && rhs != NULL_TREE &&
          TREE_TYPE(rhs) != NULL_TREE && POINTER_TYPE_P(TREE_TYPE(rhs)) &&
          authoritative_allocation_result(rhs))
        right = EncodedScalar(right.value, right.width, true);
    }

    z3::expr expression(context_);
    switch (code) {
      case EQ_EXPR:
        expression = left.value == right.value;
        break;
      case NE_EXPR:
        expression = left.value != right.value;
        break;
      case LT_EXPR:
      case LE_EXPR:
      case GT_EXPR:
      case GE_EXPR: {
        tree type = TREE_TYPE(lhs);
        if (type == NULL_TREE || !INTEGRAL_TYPE_P(type))
          return EncodedCondition(context_);
        const bool is_unsigned = TYPE_UNSIGNED(type);
        if (code == LT_EXPR)
          expression = is_unsigned ? z3::ult(left.value, right.value)
                                   : left.value < right.value;
        else if (code == LE_EXPR)
          expression = is_unsigned ? z3::ule(left.value, right.value)
                                   : left.value <= right.value;
        else if (code == GT_EXPR)
          expression = is_unsigned ? z3::ugt(left.value, right.value)
                                   : left.value > right.value;
        else
          expression = is_unsigned ? z3::uge(left.value, right.value)
                                   : left.value >= right.value;
        break;
      }
      default:
        return EncodedCondition(context_);
    }
    return EncodedCondition(expression, left.exact && right.exact);
  }

  z3::context &context_;
  QueryBudget *budget_;
  std::map<tree, z3::expr> symbols_;
  ExactPointerValues &pointer_values_;
};

class SymbolicSearch {
 public:
  SymbolicSearch(const std::vector<gimple *> &events,
                 const std::vector<edge> &required_edges,
                 const std::vector<RequiredPredicate> &required_predicates,
                 const Limits &limits, Result *result,
                 QueryBudget *budget)
      : events_(events), required_edges_(required_edges),
        required_predicates_(required_predicates), limits_(limits),
        result_(result), budget_(budget), pointer_values_(budget),
        can_reach_(reverse_reachability(events, budget)),
        visits_(last_basic_block_for_fn(cfun), 0) {
    relevant_statements_.insert(events_.begin(), events_.end());
    relevant_edges_.insert(required_edges_.begin(), required_edges_.end());
  }

  void run() {
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, ENTRY_BLOCK_PTR_FOR_FN(cfun)->succs) {
      if (!budget_->time_available())
        break;
      if ((successor->flags & EDGE_FAKE) != 0 || found_feasible_)
        continue;
      if (successor_can_reach_event(successor->dest, 0, can_reach_))
        visit(successor->dest, 0, 0, 0, false, successor);
    }

    budget_->time_available();

    if (found_feasible_) {
      result_->verdict = Verdict::feasible;
      if (!required_predicates_.empty())
        result_->reason = required_edges_.empty()
                              ? "satisfiable correlated event/contract path"
                              : "satisfiable correlated event/PHI-edge/contract "
                                "path";
      else
        result_->reason = required_edges_.empty()
                              ? "satisfiable path constraints"
                              : "satisfiable correlated event/PHI-edge path";
    } else if (result_->truncated || saw_unknown_) {
      result_->verdict = Verdict::unknown;
      if (result_->reason.empty())
        result_->reason = "path constraints were conservatively approximated";
    } else {
      result_->verdict = Verdict::infeasible;
      if (!required_predicates_.empty())
        result_->reason = required_edges_.empty()
                              ? "all correlated event/contract paths are "
                                "unsatisfiable"
                              : "all correlated event/PHI-edge/contract paths "
                                "are unsatisfiable";
      else
        result_->reason =
            required_edges_.empty()
                ? "all structurally valid event paths are unsatisfiable"
                : "all correlated event/PHI-edge paths are unsatisfiable";
    }
  }

 private:
  void ensure_encoder() {
    if (encoder_)
      return;
    encoder_.reset(new ConditionEncoder(shared_z3_context(), budget_,
                                        pointer_values_));
  }

  void ensure_solver() {
    if (solver_)
      return;
    z3::context &context = shared_z3_context();
    solver_.reset(new z3::solver(context));
    z3::params parameters(context);
    parameters.set("timeout", budget_->solver_timeout_ms());
    solver_->set(parameters);
    result_->solver_initialized = true;
  }

  bool budget_available() {
    return budget_->time_available();
  }

  bool allocation_null_constraint(enum tree_code code, tree left, tree right,
                                  tree *allocation, bool *is_null) {
    if (code != EQ_EXPR && code != NE_EXPR)
      return false;
    if (!ExactPointerValues::null_pointer_constant(right))
      std::swap(left, right);
    if (!ExactPointerValues::null_pointer_constant(right) ||
        !ExactPointerValues::identical_pointer_representation(left, right) ||
        TYPE_PRECISION(TREE_TYPE(left)) == 0 ||
        !budget_->consume_symbolic_state(false))
      return false;
    tree origin = pointer_values_.canonical_pointer_value(left);
    if (origin == NULL_TREE ||
        !pointer_values_.authoritative_allocation_result(origin))
      return false;
    *allocation = origin;
    *is_null = code == EQ_EXPR;
    return budget_available();
  }

  // Only an all-NULL conjunction outside any active SMT frame can be
  // decided here. Leaf predicates are transactional and never leak to a
  // sibling path. Unsupported predicates still use the full existing solver.
  bool accept_null_predicates() {
    if (smt_depth_ != 0)
      return false;
    std::vector<std::pair<tree, bool>> decoded;
    for (const RequiredPredicate &predicate : required_predicates_) {
      tree allocation = NULL_TREE;
      bool is_null = false;
      const auto code = predicate.relation == ScalarRelation::equal
                            ? EQ_EXPR : NE_EXPR;
      if (!allocation_null_constraint(code, predicate.lhs, predicate.rhs,
                                      &allocation, &is_null))
        return false;
      decoded.emplace_back(allocation, is_null);
    }
    std::map<tree, bool> constraints = null_constraints_;
    for (const auto &predicate : decoded) {
      ++result_->exact_null_checks;
      const auto inserted = constraints.emplace(predicate);
      if (!inserted.second && inserted.first->second != predicate.second) {
        ++result_->exact_null_prunes;
        return true;
      }
    }
    if (budget_available())
      found_feasible_ = true;
    return true;
  }

  // A NULL-only prefix is not dropped when the path reaches a harder
  // condition. Replay every exact constraint inside the new solver frame,
  // using the very same canonicalizer and symbols as that condition.
  bool replay_null_constraints() {
    // An active parent SMT frame already owns the entire NULL prefix.
    // No finite-domain mutation is allowed below that frame.
    if (smt_depth_ != 0)
      return true;
    for (const auto &constraint : null_constraints_) {
      RequiredPredicate predicate;
      predicate.lhs = constraint.first;
      predicate.rhs = build_int_cst(TREE_TYPE(constraint.first), 0);
      predicate.relation = constraint.second ? ScalarRelation::equal
                                             : ScalarRelation::not_equal;
      EncodedCondition encoded = encoder_->encode(predicate);
      if (!encoded.available || !encoded.exact || !budget_available()) {
        saw_unknown_ = true;
        return false;
      }
      solver_->add(encoded.value);
    }
    return true;
  }

  void accept_completed_candidate(unsigned int constraint_count,
                                  bool incomplete) {
    if (incomplete) {
      saw_unknown_ = true;
      return;
    }
    if (required_predicates_.empty()) {
      found_feasible_ = true;
      return;
    }
    if (required_predicates_.size() >
        limits_.max_constraints -
            std::min(constraint_count, limits_.max_constraints)) {
      saw_unknown_ = true;
      result_->truncated = true;
      if (result_->reason.empty())
        result_->reason = "constraint cap while applying call contract";
      return;
    }

    if (accept_null_predicates() || !budget_available())
      return;
    ensure_encoder();
    std::vector<EncodedCondition> predicates;
    predicates.reserve(required_predicates_.size());
    for (const RequiredPredicate &predicate : required_predicates_) {
      EncodedCondition encoded = encoder_->encode(predicate);
      if (!encoded.available || !encoded.exact) {
        ++result_->unsupported_conditions;
        saw_unknown_ = true;
        if (result_->reason.empty())
          result_->reason =
              "call-contract predicate was unsupported or inexact";
        return;
      }
      ++result_->encoded_conditions;
      predicates.push_back(encoded);
    }
    if (!budget_->consume_solver_check())
      return;
    ensure_solver();
    solver_->push();
    if (!replay_null_constraints()) {
      solver_->pop();
      return;
    }
    for (const EncodedCondition &predicate : predicates)
      solver_->add(predicate.value);
    const z3::check_result check = solver_->check();
    if (!budget_->time_available()) {
      solver_->pop();
      return;
    }
    if (check == z3::sat) {
      found_feasible_ = true;
    } else if (check == z3::unsat) {
      ++result_->solver_prunes;
    } else {
      saw_unknown_ = true;
      result_->truncated = true;
      if (result_->reason.empty())
        result_->reason =
            "solver timeout or unknown call-contract predicate";
    }
    solver_->pop();
  }

  bool exact_one_trip_loop(basic_block header, ExactOneTripLoop *result) {
    return exact_one_trip_cache_.lookup(header, relevant_statements_, result,
                                        budget_);
  }

  unsigned int advance_required_edge(edge successor,
                                     unsigned int event_stage,
                                     unsigned int edge_stage) const {
    // Consume a witness only on the predecessor's outgoing transition.  The
    // incoming latch edge is therefore not consumed again when the proven
    // second header evaluation selects the exit.
    if (event_stage != 0 && edge_stage < required_edges_.size() &&
        successor == required_edges_[edge_stage])
      return edge_stage + 1;
    return edge_stage;
  }

  void follow_proven_edge(edge successor, unsigned int event_stage,
                          unsigned int edge_stage,
                          unsigned int constraint_count, bool incomplete) {
    if (successor == nullptr || !ordinary_edge(successor) ||
        !successor_can_reach_event(successor->dest, event_stage, can_reach_))
      return;
    visit(successor->dest, event_stage,
          advance_required_edge(successor, event_stage, edge_stage),
          constraint_count, incomplete, successor);
  }

  void visit(basic_block block, unsigned int event_stage,
             unsigned int edge_stage, unsigned int constraint_count,
             bool incomplete, edge incoming) {
    if (found_feasible_ || !budget_available())
      return;
    if (!budget_->consume_symbolic_state())
      return;

    if (block == EXIT_BLOCK_PTR_FOR_FN(cfun))
      return;
    if (block->index < 0 ||
        static_cast<unsigned int>(block->index) >= visits_.size()) {
      saw_unknown_ = true;
      return;
    }

    unsigned int &visit_count = visits_[block->index];
    if (visit_count >= limits_.max_block_visits) {
      saw_unknown_ = true;
      result_->truncated = true;
      if (result_->reason.empty())
        result_->reason = "loop visit cap";
      return;
    }
    const unsigned int previous_visit_count = visit_count;
    const bool repeated_block = previous_visit_count != 0;
    ++visit_count;

    event_stage = advance_events(block, event_stage, events_, budget_);
    if (!budget_->time_available()) {
      --visit_count;
      return;
    }
    if (event_stage == events_.size()) {
      if (edge_stage == required_edges_.size()) {
        accept_completed_candidate(constraint_count,
                                   incomplete || repeated_block);
      }
      --visit_count;
      return;
    }

    // Do not skip an ownership-event-bearing loop body.  For the deliberately
    // narrow one-trip theorem, select its proven initial body edge, traverse
    // every real body block and the latch normally, then select the proven
    // exit on the second header evaluation.  Incoming edge identity plus the
    // recursion-stack visit count prevents a descriptor from being reused by
    // an external or exit-side cycle.
    if (!incomplete) {
      ExactOneTripLoop loop;
      edge proven = nullptr;
      bool begins_unfold = false;
      if (exact_one_trip_loop(block, &loop)) {
        if (previous_visit_count == 0 && incoming == loop.preheader) {
          proven = loop.body;
          begins_unfold = true;
        } else if (previous_visit_count == 1 && incoming == loop.latch) {
          proven = loop.exit;
        }
      }
      if (proven != nullptr) {
        if (begins_unfold)
          ++result_->exact_one_trip_unfolds;
        follow_proven_edge(proven, event_stage, edge_stage, constraint_count,
                           false);
        --visit_count;
        return;
      }
    }

    // Collapse strictly proven finite, event-neutral counted loops before
    // splitting the solver stack.  Unlike the generic visit cap, this is an
    // exact normal-exit theorem for a canonical unsigned SSA recurrence.
    if (!repeated_block && !incomplete) {
      NeutralCountedLoop loop;
      if (strict_neutral_counted_loop(block, relevant_statements_,
                                      relevant_edges_, &loop, budget_) &&
          successor_can_reach_event(loop.exit, event_stage, can_reach_)) {
        ++result_->abstracted_loops;
        result_->abstracted_blocks += loop.interior_blocks;
        visit(loop.exit, event_stage, edge_stage, constraint_count, false,
              nullptr);
        --visit_count;
        return;
      }
    }

    // Collapse event-neutral structured diamonds before splitting the solver
    // stack.  Thirty independent diamonds now cost thirty recognitions and a
    // linear number of CFG states instead of 2^30 symbolic continuations.
    // The recognizer rejects cyclic arms, exceptional/fake edges, provenance
    // edges, and any arm containing an event.  A surrounding loop is still
    // bounded below and can only make the query UNKNOWN.
    if (!repeated_block && !incomplete) {
      NeutralDiamond diamond;
      if (strict_neutral_diamond(block, relevant_statements_,
                                 relevant_edges_, &diamond, budget_) &&
          successor_can_reach_event(diamond.join, event_stage,
                                    can_reach_)) {
        ++result_->abstracted_diamonds;
        result_->abstracted_blocks += diamond.interior_blocks;
        visit(diamond.join, event_stage, edge_stage, constraint_count, false,
              nullptr);
        --visit_count;
        return;
      }
    }

    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if (!budget_->time_available())
        break;
      if (found_feasible_ || (successor->flags & EDGE_FAKE) != 0 ||
          !successor_can_reach_event(successor->dest, event_stage,
                                     can_reach_))
        continue;

      unsigned int next_edge_stage = edge_stage;
      // Keep provenance and event order in this same symbolic state.  In
      // particular, the solver constraints accumulated while reaching a PHI
      // argument edge must still be present when the release/use sequence is
      // checked.  Consuming witnesses before EVENTS[0] is forbidden because
      // they describe propagation of the allocation result.
      next_edge_stage =
          advance_required_edge(successor, event_stage, next_edge_stage);

      const bool true_edge = (successor->flags & EDGE_TRUE_VALUE) != 0;
      const bool false_edge = (successor->flags & EDGE_FALSE_VALUE) != 0;
      gimple *terminator = block_terminator(block);
      if ((!true_edge && !false_edge) || terminator == nullptr ||
          gimple_code(terminator) != GIMPLE_COND || repeated_block ||
          incomplete) {
        const bool unsupported =
            EDGE_COUNT(block->succs) > 1 &&
            ((!true_edge && !false_edge) || terminator == nullptr ||
             gimple_code(terminator) != GIMPLE_COND);
        if (unsupported)
          ++result_->unsupported_conditions;
        visit(successor->dest, event_stage, next_edge_stage, constraint_count,
              incomplete || unsupported || repeated_block, successor);
        continue;
      }

      if (constraint_count >= limits_.max_constraints) {
        saw_unknown_ = true;
        result_->truncated = true;
        if (result_->reason.empty())
          result_->reason = "constraint cap";
        continue;
      }
      if (smt_depth_ == 0 && ordinary_edge(successor)) {
        tree allocation = NULL_TREE;
        bool is_null = false;
        if (allocation_null_constraint(gimple_cond_code(terminator),
                                        gimple_cond_lhs(terminator),
                                        gimple_cond_rhs(terminator),
                                        &allocation, &is_null)) {
          if (!true_edge)
            is_null = !is_null;
          ++result_->exact_null_checks;
          const auto inserted = null_constraints_.emplace(allocation, is_null);
          if (inserted.second || inserted.first->second == is_null) {
            visit(successor->dest, event_stage, next_edge_stage,
                  constraint_count + 1, false, successor);
          } else {
            ++result_->exact_null_prunes;
          }
          // Even an early successful witness, cap or timeout returns through
          // this restoration point before another CFG successor is examined.
          if (inserted.second)
            null_constraints_.erase(inserted.first);
          continue;
        }
      }
      if (!budget_available())
        break;
      ensure_encoder();
      EncodedCondition condition = encoder_->encode(terminator);
      if (!condition.available) {
        ++result_->unsupported_conditions;
        visit(successor->dest, event_stage, next_edge_stage,
              constraint_count, true, successor);
        continue;
      }

      ++result_->encoded_conditions;
      if (!condition.exact) {
        // Inexact expressions are over-approximations only.  In particular,
        // mutable memory loads must not be equated across virtual-SSA
        // versions and then used to prune a real path as UNSAT.
        ++result_->unsupported_conditions;
        visit(successor->dest, event_stage, next_edge_stage,
              constraint_count, true, successor);
        continue;
      }
      if (!budget_->consume_solver_check())
        continue;
      ensure_solver();
      solver_->push();
      if (!replay_null_constraints()) {
        solver_->pop();
        continue;
      }
      solver_->add(true_edge ? condition.value : !condition.value);
      const z3::check_result check = solver_->check();
      if (!budget_->time_available()) {
        solver_->pop();
        break;
      }
      if (check == z3::sat) {
        ++smt_depth_;
        visit(successor->dest, event_stage, next_edge_stage,
              constraint_count + 1,
              incomplete || !condition.exact, successor);
        --smt_depth_;
      } else if (check == z3::unsat) {
        ++result_->solver_prunes;
      } else {
        saw_unknown_ = true;
        result_->truncated = true;
        if (result_->reason.empty())
          result_->reason = "solver timeout or unknown result";
      }
      solver_->pop();
    }
    --visit_count;
  }

  const std::vector<gimple *> &events_;
  const std::vector<edge> &required_edges_;
  const std::vector<RequiredPredicate> &required_predicates_;
  const Limits &limits_;
  Result *result_;
  QueryBudget *budget_;
  ExactPointerValues pointer_values_;
  std::map<tree, bool> null_constraints_;
  unsigned int smt_depth_ = 0;
  std::unique_ptr<z3::solver> solver_;
  std::unique_ptr<ConditionEncoder> encoder_;
  std::vector<std::set<basic_block>> can_reach_;
  std::set<gimple *> relevant_statements_;
  std::set<edge> relevant_edges_;
  ExactOneTripCache exact_one_trip_cache_;
  std::vector<unsigned int> visits_;
  bool found_feasible_ = false;
  bool saw_unknown_ = false;
};

class AvoidingSearch {
 public:
  AvoidingSearch(gimple *required_start,
                 const std::set<gimple *> &forbidden,
                 const std::set<edge> &forbidden_edges,
                 const std::vector<RequiredPredicate> &required_predicates,
                 const Limits &limits, Result *result,
                 QueryBudget *budget)
      : required_start_(required_start),
        required_predicates_(required_predicates), forbidden_(forbidden),
        forbidden_edges_(forbidden_edges),
        limits_(limits), result_(result), budget_(budget), pointer_values_(budget),
        can_reach_start_(reverse_reachability(gimple_bb(required_start),
                                              budget)),
        can_reach_exit_(
            reverse_reachability(EXIT_BLOCK_PTR_FOR_FN(cfun), budget)),
        visits_(last_basic_block_for_fn(cfun), 0) {
    relevant_statements_.insert(required_start_);
    relevant_statements_.insert(forbidden_.begin(), forbidden_.end());
  }

  void run() {
    if (!initialize_required_predicates())
      return;
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, ENTRY_BLOCK_PTR_FOR_FN(cfun)->succs) {
      if (!budget_->time_available())
        break;
      if ((successor->flags & EDGE_FAKE) != 0 || found_feasible_)
        continue;
      if (can_reach_target(successor->dest, false))
        visit(successor->dest, false, required_predicates_.size(), false, successor);
    }

    budget_->time_available();

    if (found_feasible_) {
      result_->verdict = Verdict::feasible;
      result_->reason = "satisfiable path reaches EXIT without a forbidden event";
    } else if (result_->truncated || saw_unknown_) {
      result_->verdict = Verdict::unknown;
      if (result_->reason.empty())
        result_->reason =
            "avoid-forbidden exit paths were conservatively approximated";
    } else {
      result_->verdict = Verdict::infeasible;
      result_->reason =
          "all avoid-forbidden exit paths are unsatisfiable";
    }
  }

 private:
  bool initialize_required_predicates() {
    if (required_predicates_.size() > 32 || required_predicates_.size() > limits_.max_constraints) {
      result_->verdict = Verdict::unknown;
      result_->reason = "conditional-callee predicate cap";
      return false;
    }
    for (const RequiredPredicate &predicate : required_predicates_) {
      if (!budget_->time_available())
        return false;
      tree lhs = predicate.lhs;
      tree rhs = predicate.rhs;
      const bool equal = predicate.relation == ScalarRelation::equal;
      if ((predicate.relation != ScalarRelation::equal &&
           predicate.relation != ScalarRelation::not_equal) || lhs == NULL_TREE ||
          rhs == NULL_TREE || !INTEGRAL_TYPE_P(TREE_TYPE(lhs)) ||
          !INTEGRAL_TYPE_P(TREE_TYPE(rhs))) {
        result_->verdict = Verdict::unknown;
        result_->reason = "unsupported conditional-callee borrow predicate";
        return false;
      }
      if (TREE_CODE(lhs) == INTEGER_CST && TREE_CODE(rhs) == INTEGER_CST &&
          !TREE_OVERFLOW(lhs) && !TREE_OVERFLOW(rhs) && integer_zerop(rhs)) {
        ++result_->exact_scalar_checks;
        if (integer_zerop(lhs) != equal) {
          ++result_->exact_scalar_prunes;
          result_->verdict = Verdict::infeasible;
          result_->reason = "literal caller argument contradicts the callee borrow guard";
          return false;
        }
        continue;
      }
      tree identity = NULL_TREE;
      bool is_zero = false, pointer = false;
      if (!zero_constraint(equal ? EQ_EXPR : NE_EXPR, lhs, rhs,
                           &identity, &is_zero, &pointer) || pointer ||
          SSA_NAME_VAR(identity) == NULL_TREE || DECL_CONTEXT(SSA_NAME_VAR(identity)) != cfun->decl) {
        result_->verdict = Verdict::unknown;
        if (result_->reason.empty())
          result_->reason = "unsupported conditional-callee borrow predicate";
        return false;
      }
      ++result_->exact_scalar_checks;
      const auto existing = zero_constraints_.find(identity);
      if (existing != zero_constraints_.end() && existing->second != is_zero) {
        ++result_->exact_scalar_prunes;
        result_->verdict = Verdict::infeasible;
        result_->reason = "callee borrow guards require contradictory caller arguments";
        return false;
      }
      zero_constraints_[identity] = is_zero;
    }
    return true;
  }

  void ensure_encoder() {
    if (encoder_)
      return;
    encoder_.reset(new ConditionEncoder(shared_z3_context(), budget_,
                                        pointer_values_));
  }

  void ensure_solver() {
    if (solver_)
      return;
    z3::context &context = shared_z3_context();
    solver_.reset(new z3::solver(context));
    z3::params parameters(context);
    parameters.set("timeout", budget_->solver_timeout_ms());
    solver_->set(parameters);
    result_->solver_initialized = true;
  }

  bool can_reach_target(basic_block block, bool active) const {
    const std::set<basic_block> &reachable =
        active ? can_reach_exit_ : can_reach_start_;
    return reachable.find(block) != reachable.end();
  }

  // An exact zero/nonzero domain for independent entry parameters, plus the
  // same allocation/NULL projection used by SMT. Memory values, derived
  // scalar expressions and opaque returns require the ordinary encoder.
  // Restrict this fast domain to the prefix outside every active SMT frame.
  bool zero_constraint(gimple *condition, tree *identity, bool *is_zero,
                       bool *pointer) {
    const enum tree_code code = gimple_cond_code(condition);
    return zero_constraint(code, gimple_cond_lhs(condition), gimple_cond_rhs(condition),
                           identity, is_zero, pointer);
  }

  bool zero_constraint(enum tree_code code, tree left, tree right,
                       tree *identity, bool *is_zero, bool *pointer) {
    if (code != EQ_EXPR && code != NE_EXPR)
      return false;
    if (!ExactPointerValues::null_pointer_constant(right))
      std::swap(left, right);
    if (!ExactPointerValues::null_pointer_constant(right) ||
        TREE_OVERFLOW(right) || left == NULL_TREE || TREE_TYPE(left) == NULL_TREE ||
        TYPE_PRECISION(TREE_TYPE(left)) == 0 ||
        !budget_->consume_symbolic_state(false))
      return false;
    *pointer = POINTER_TYPE_P(TREE_TYPE(left));
    if (*pointer) {
      if (!ExactPointerValues::identical_pointer_representation(left, right))
        return false;
      left = pointer_values_.canonical_pointer_value(left);
      if (left == NULL_TREE ||
          !pointer_values_.authoritative_allocation_result(left))
        return false;
    } else if (!INTEGRAL_TYPE_P(TREE_TYPE(left)) ||
               !INTEGRAL_TYPE_P(TREE_TYPE(right)) ||
               TREE_CODE(left) != SSA_NAME || !SSA_NAME_IS_DEFAULT_DEF(left) ||
               SSA_NAME_VAR(left) == NULL_TREE ||
               TREE_CODE(SSA_NAME_VAR(left)) != PARM_DECL ||
               TREE_THIS_VOLATILE(SSA_NAME_VAR(left))) {
      return false;
    }
    *identity = left;
    *is_zero = code == EQ_EXPR;
    return budget_available();
  }

  bool replay_zero_constraints() {
    if (smt_depth_ != 0)
      return true;
    for (const auto &constraint : zero_constraints_) {
      RequiredPredicate predicate;
      predicate.lhs = constraint.first;
      predicate.rhs = build_int_cst(TREE_TYPE(constraint.first), 0);
      predicate.relation = constraint.second ? ScalarRelation::equal
                                             : ScalarRelation::not_equal;
      EncodedCondition encoded = encoder_->encode(predicate);
      if (!encoded.available || !encoded.exact || !budget_available()) {
        saw_unknown_ = true;
        return false;
      }
      solver_->add(encoded.value);
    }
    return true;
  }

  bool budget_available() {
    return budget_->time_available();
  }

  void visit(basic_block block, bool active, unsigned int constraint_count,
             bool incomplete, edge incoming) {
    if (found_feasible_ || !budget_available())
      return;
    if (!budget_->consume_symbolic_state())
      return;

    if (block == EXIT_BLOCK_PTR_FOR_FN(cfun)) {
      if (active) {
        if (incomplete)
          saw_unknown_ = true;
        else
          found_feasible_ = true;
      }
      return;
    }
    if (block->index < 0 ||
        static_cast<unsigned int>(block->index) >= visits_.size()) {
      saw_unknown_ = true;
      return;
    }

    unsigned int &visit_count = visits_[block->index];
    if (visit_count >= limits_.max_block_visits) {
      saw_unknown_ = true;
      result_->truncated = true;
      if (result_->reason.empty())
        result_->reason = "loop visit cap";
      return;
    }
    const unsigned int previous_visit_count = visit_count;
    const bool repeated_block = previous_visit_count != 0;
    ++visit_count;

    active = scan_avoiding(block, active, required_start_, forbidden_,
                           budget_);
    if (!budget_->time_available()) {
      --visit_count;
      return;
    }

    // Eventful one-trip loops cannot be collapsed as neutral loops. Traverse
    // the real body once, including all allocations, releases, and forbidden
    // edges, then take the certificate's exit on the matching latch return.
    // Incoming edge plus stack visit count binds the theorem to this exact
    // loop invocation; an unrelated cycle cannot reuse its second step.
    if (!incomplete) {
      ExactOneTripLoop loop;
      edge proven = nullptr;
      bool begins_unfold = false;
      if (exact_one_trip_cache_.lookup(block, relevant_statements_, &loop,
                                       budget_)) {
        if (previous_visit_count == 0 && incoming == loop.preheader) {
          proven = loop.body;
          begins_unfold = true;
        } else if (previous_visit_count == 1 && incoming == loop.latch) {
          proven = loop.exit;
        }
      }
      if (proven != nullptr) {
        if (begins_unfold)
          ++result_->exact_one_trip_unfolds;
        if ((!active || forbidden_edges_.count(proven) == 0) &&
            ordinary_edge(proven) &&
            can_reach_target(proven->dest, active))
          visit(proven->dest, active, constraint_count, false, proven);
        --visit_count;
        return;
      }
    }

    // The same strict stutter theorem is useful for leak refinement: when
    // neither the allocation start nor any release occurs in the loop, its
    // proven unique normal exit preserves the active/forbidden automaton
    // state exactly.  The recognizer's assignment-only whitelist excludes
    // opaque calls, asm, volatile operations, and abnormal control.  Passing
    // the forbidden edges as relevance barriers prevents either abstraction
    // from crossing a conditional ownership-transfer edge, including an
    // edge into the join or the loop's own exit.
    if (!repeated_block && !incomplete) {
      NeutralCountedLoop loop;
      if (strict_neutral_counted_loop(block, relevant_statements_,
                                      forbidden_edges_, &loop, budget_) &&
          can_reach_target(loop.exit, active)) {
        ++result_->abstracted_loops;
        result_->abstracted_blocks += loop.interior_blocks;
        visit(loop.exit, active, constraint_count, false, nullptr);
        --visit_count;
        return;
      }
    }

    if (!repeated_block && !incomplete) {
      NeutralDiamond diamond;
      if (strict_neutral_diamond(block, relevant_statements_,
                                 forbidden_edges_, &diamond, budget_) &&
          can_reach_target(diamond.join, active)) {
        ++result_->abstracted_diamonds;
        result_->abstracted_blocks += diamond.interior_blocks;
        visit(diamond.join, active, constraint_count, false, nullptr);
        --visit_count;
        return;
      }
    }

    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if (!budget_->time_available())
        break;
      if (found_feasible_)
        continue;
      if (active && forbidden_edges_.find(successor) != forbidden_edges_.end())
        continue;
      if ((successor->flags & EDGE_FAKE) != 0) {
        if (active) {
          saw_unknown_ = true;
          if (result_->reason.empty())
            result_->reason =
                "active path reaches an unmodeled fake CFG edge";
        }
        continue;
      }
      if (!can_reach_target(successor->dest, active))
        continue;

      const bool true_edge = (successor->flags & EDGE_TRUE_VALUE) != 0;
      const bool false_edge = (successor->flags & EDGE_FALSE_VALUE) != 0;
      gimple *terminator = block_terminator(block);
      if ((!true_edge && !false_edge) || terminator == nullptr ||
          gimple_code(terminator) != GIMPLE_COND || repeated_block ||
          incomplete) {
        const bool unsupported =
            EDGE_COUNT(block->succs) > 1 &&
            ((!true_edge && !false_edge) || terminator == nullptr ||
             gimple_code(terminator) != GIMPLE_COND);
        if (unsupported)
          ++result_->unsupported_conditions;
        visit(successor->dest, active, constraint_count,
              incomplete || unsupported || repeated_block, successor);
        continue;
      }

      if (constraint_count >= limits_.max_constraints) {
        saw_unknown_ = true;
        result_->truncated = true;
        if (result_->reason.empty())
          result_->reason = "constraint cap";
        continue;
      }
      if (smt_depth_ == 0 && ordinary_edge(successor)) {
        tree identity = NULL_TREE;
        bool is_zero = false;
        bool pointer = false;
        if (zero_constraint(terminator, &identity, &is_zero, &pointer)) {
          if (!true_edge)
            is_zero = !is_zero;
          if (pointer)
            ++result_->exact_null_checks;
          else
            ++result_->exact_scalar_checks;
          const auto inserted = zero_constraints_.emplace(identity, is_zero);
          if (inserted.second || inserted.first->second == is_zero) {
            visit(successor->dest, active, constraint_count + 1,
                  false, successor);
          } else if (pointer) {
            ++result_->exact_null_prunes;
          } else {
            ++result_->exact_scalar_prunes;
          }
          if (inserted.second)
            zero_constraints_.erase(inserted.first);
          continue;
        }
      }
      if (!budget_available())
        break;
      ensure_encoder();
      EncodedCondition condition = encoder_->encode(terminator);
      if (!condition.available) {
        ++result_->unsupported_conditions;
        visit(successor->dest, active, constraint_count, true, successor);
        continue;
      }

      ++result_->encoded_conditions;
      if (!condition.exact) {
        // Only exact encodings may prove that an avoid-release path is
        // impossible.  Retain inexact paths as UNKNOWN.
        ++result_->unsupported_conditions;
        visit(successor->dest, active, constraint_count, true, successor);
        continue;
      }
      if (!budget_->consume_solver_check())
        continue;
      ensure_solver();
      solver_->push();
      if (!replay_zero_constraints()) {
        solver_->pop();
        continue;
      }
      solver_->add(true_edge ? condition.value : !condition.value);
      const z3::check_result check = solver_->check();
      if (!budget_->time_available()) {
        solver_->pop();
        break;
      }
      if (check == z3::sat) {
        ++smt_depth_;
        visit(successor->dest, active, constraint_count + 1,
              incomplete || !condition.exact, successor);
        --smt_depth_;
      } else if (check == z3::unsat) {
        ++result_->solver_prunes;
      } else {
        saw_unknown_ = true;
        result_->truncated = true;
        if (result_->reason.empty())
          result_->reason = "solver timeout or unknown result";
      }
      solver_->pop();
    }
    --visit_count;
  }

  gimple *required_start_;
  const std::vector<RequiredPredicate> &required_predicates_;
  const std::set<gimple *> &forbidden_;
  const std::set<edge> &forbidden_edges_;
  const Limits &limits_;
  Result *result_;
  QueryBudget *budget_;
  ExactPointerValues pointer_values_;
  std::map<tree, bool> zero_constraints_;
  unsigned int smt_depth_ = 0;
  std::unique_ptr<z3::solver> solver_;
  std::unique_ptr<ConditionEncoder> encoder_;
  std::set<basic_block> can_reach_start_;
  std::set<basic_block> can_reach_exit_;
  std::set<gimple *> relevant_statements_;
  ExactOneTripCache exact_one_trip_cache_;
  std::vector<unsigned int> visits_;
  bool found_feasible_ = false;
  bool saw_unknown_ = false;
};

#endif  // STATIC_ANALYZER_HAVE_Z3

}  // namespace

Result sequence_feasible(const std::vector<gimple *> &events,
                         const Limits &limits) {
  Result result;
  QueryBudget budget(limits, &result);
  if (!budget.begin())
    return finish(result);
  if (events.empty()) {
    result.verdict = Verdict::feasible;
    result.reason = "empty event sequence";
    return finish(result);
  }
  if (!current_function_owns(events, &budget)) {
    if (result.truncated)
      return finish(result);
    result.verdict = Verdict::unknown;
    result.reason =
        "events are not present in the current GCC function CFG";
    return finish(result);
  }

  const StructuralResult structural = structural_sequence(events, &budget);
  if (structural.truncated) {
    result.verdict = Verdict::unknown;
    result.truncated = true;
    if (result.reason.empty())
      result.reason = "CFG product-state cap";
    return finish(result);
  }
  if (!structural.reachable) {
    result.verdict = Verdict::infeasible;
    result.reason = "event order is unreachable in the CFG";
    return finish(result);
  }

#ifdef STATIC_ANALYZER_HAVE_Z3
  if (limits.use_solver) {
    result.used_solver = true;
    try {
      const std::vector<edge> no_required_edges;
      const std::vector<RequiredPredicate> no_required_predicates;
      SymbolicSearch search(events, no_required_edges,
                            no_required_predicates, limits, &result, &budget);
      search.run();
    } catch (const z3::exception &error) {
      result.verdict = Verdict::unknown;
      result.truncated = true;
      result.reason = std::string("Z3 error: ") + error.msg();
    }
    return finish(result);
  }
#endif

  result.verdict = Verdict::unknown;
  result.reason = limits.use_solver
                      ? "Z3 support was not compiled in; CFG says may execute"
                      : "solver disabled; CFG says may execute";
  return finish(result);
}

Result sequence_feasible(const std::vector<gimple *> &events) {
  return sequence_feasible(events, configured_limits);
}

Result sequence_feasible_through_edges(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges,
    const Limits &limits) {
  const std::vector<RequiredPredicate> no_required_predicates;
  return sequence_feasible_with_predicates(
      events, required_edges, no_required_predicates, limits);
}

Result sequence_feasible_with_predicates(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges,
    const std::vector<RequiredPredicate> &required_predicates,
    const Limits &limits) {
  if (required_edges.empty() && required_predicates.empty())
    return sequence_feasible(events, limits);

  Result result;
  QueryBudget budget(limits, &result);
  if (!budget.begin())
    return finish(result);
  FunctionInventory inventory;
  if (!inventory_current_function(&inventory, &budget) ||
      !inventory_owns_events(inventory, events, &budget)) {
    if (result.truncated)
      return finish(result);
    result.verdict = Verdict::unknown;
    result.reason =
        "events are not present in the current GCC function CFG";
    return finish(result);
  }
  if (!required_edges.empty() &&
      !inventory_owns_required_edges(inventory, required_edges, &budget)) {
    if (result.truncated)
      return finish(result);
    result.verdict = Verdict::unknown;
    result.reason =
        "required PHI edge is null, fake, stale, or outside the current CFG";
    return finish(result);
  }
  if (events.empty()) {
    result.verdict = Verdict::unknown;
    result.reason =
        "required provenance or call-contract predicates need an event interval";
    return finish(result);
  }

  const StructuralResult structural = structural_sequence_through_edges(
      events, required_edges, &budget);
  if (structural.truncated) {
    result.verdict = Verdict::unknown;
    result.truncated = true;
    if (result.reason.empty())
      result.reason = "CFG event/PHI-edge product-state cap";
    return finish(result);
  }
  if (!structural.reachable) {
    result.verdict = Verdict::infeasible;
    result.reason = required_edges.empty()
                        ? "event order is unreachable in the CFG"
                        : "event order cannot traverse the required PHI "
                          "provenance edges";
    return finish(result);
  }

#ifdef STATIC_ANALYZER_HAVE_Z3
  if (limits.use_solver) {
    result.used_solver = true;
    try {
      // Event and PHI-edge stages advance in one traversal under one solver
      // stack.  This prevents a satisfiable event-only arm from standing in
      // for an unsatisfiable provenance-constrained arm.
      SymbolicSearch search(events, required_edges,
                            required_predicates, limits, &result, &budget);
      search.run();
    } catch (const z3::exception &error) {
      result.verdict = Verdict::unknown;
      result.truncated = true;
      result.reason = std::string("Z3 error: ") + error.msg();
    }
    return finish(result);
  }
#endif

  result.verdict = Verdict::unknown;
  result.reason =
      limits.use_solver
          ? "Z3 support was not compiled in; CFG contract product says may "
            "execute"
          : "solver disabled; CFG contract product says may execute";
  return finish(result);
}

Result sequence_feasible_through_edges(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges) {
  return sequence_feasible_through_edges(events, required_edges,
                                         configured_limits);
}

Result sequence_feasible_with_predicates(
    const std::vector<gimple *> &events,
    const std::vector<edge> &required_edges,
    const std::vector<RequiredPredicate> &required_predicates) {
  return sequence_feasible_with_predicates(
      events, required_edges, required_predicates, configured_limits);
}

Result path_to_exit_avoiding_with_predicates(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const std::vector<edge> &forbidden_edges,
    const std::vector<RequiredPredicate> &required_predicates, const Limits &limits) {
  Result result;
  QueryBudget budget(limits, &result);
  if (!budget.begin())
    return finish(result);
  std::vector<gimple *> owned;
  owned.reserve(forbidden.size() + 1);
  owned.push_back(required_start);
  owned.insert(owned.end(), forbidden.begin(), forbidden.end());
  FunctionInventory inventory;
  if (!inventory_current_function(&inventory, &budget) ||
      !inventory_owns_events(inventory, owned, &budget)) {
    if (result.truncated)
      return finish(result);
    result.verdict = Verdict::unknown;
    result.reason =
        "start or forbidden event is not present in the current function CFG";
    return finish(result);
  }
  if (!inventory_owns_forbidden_edges(inventory, forbidden_edges, &budget)) {
    if (result.truncated)
      return finish(result);
    result.verdict = Verdict::unknown;
    result.reason =
        "forbidden edge is null, stale, outside the current CFG, or abnormal";
    return finish(result);
  }

  std::set<gimple *> forbidden_set(forbidden.begin(), forbidden.end());
  const std::set<edge> forbidden_edge_set(forbidden_edges.begin(),
                                         forbidden_edges.end());
  if (forbidden_set.find(required_start) != forbidden_set.end()) {
    result.verdict = Verdict::unknown;
    result.reason = "the required start event is also forbidden";
    return finish(result);
  }

  const StructuralResult structural = structural_path_to_exit_avoiding(
      required_start, forbidden_set, forbidden_edge_set, &budget);
  if (structural.truncated) {
    result.verdict = Verdict::unknown;
    result.truncated = true;
    if (result.reason.empty())
      result.reason = "CFG product-state cap";
    return finish(result);
  }
  if (structural.incomplete) {
    result.verdict = Verdict::unknown;
    result.reason =
        "an active avoid-release path reaches an unmodeled fake CFG edge";
    return finish(result);
  }
  if (!structural.reachable) {
    result.verdict = Verdict::infeasible;
    result.reason =
        "no CFG path reaches EXIT after start while avoiding forbidden events";
    return finish(result);
  }

#ifdef STATIC_ANALYZER_HAVE_Z3
  if (limits.use_solver) {
    result.used_solver = true;
    try {
      AvoidingSearch search(required_start, forbidden_set, forbidden_edge_set,
                            required_predicates, limits, &result, &budget);
      search.run();
    } catch (const z3::exception &error) {
      result.verdict = Verdict::unknown;
      result.truncated = true;
      result.reason = std::string("Z3 error: ") + error.msg();
    }
    return finish(result);
  }
#endif

  result.verdict = Verdict::unknown;
  result.reason = limits.use_solver
                      ? "Z3 support was not compiled in; CFG has an avoid path"
                      : "solver disabled; CFG has an avoid path";
  return finish(result);
}

Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const std::vector<edge> &forbidden_edges, const Limits &limits) {
  const std::vector<RequiredPredicate> no_predicates;
  return path_to_exit_avoiding_with_predicates(required_start, forbidden, forbidden_edges,
                                              no_predicates, limits);
}

Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const Limits &limits) {
  const std::vector<edge> no_forbidden_edges;
  return path_to_exit_avoiding(required_start, forbidden, no_forbidden_edges,
                               limits);
}

Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden) {
  return path_to_exit_avoiding(required_start, forbidden, configured_limits);
}

Result path_to_exit_avoiding(
    gimple *required_start, const std::vector<gimple *> &forbidden,
    const std::vector<edge> &forbidden_edges) {
  return path_to_exit_avoiding(required_start, forbidden, forbidden_edges,
                               configured_limits);
}

void set_default_limits(const Limits &limits) {
  configured_limits = limits;
  // analyzer::run resets statistics before applying the current plugin
  // options.  Refreshing an unused budget here ensures the same run observes
  // the newly selected limits without allowing a mid-run caller to replenish
  // already consumed resources.
  if (!tu_budget.initialized ||
      (accumulated.queries == 0 && tu_budget.queries == 0))
    start_tu_budget(configured_limits);
}

Limits default_limits() { return configured_limits; }

bool solver_compiled_in() {
#ifdef STATIC_ANALYZER_HAVE_Z3
  return true;
#else
  return false;
#endif
}

const char *verdict_name(Verdict verdict) {
  switch (verdict) {
    case Verdict::feasible:
      return "feasible";
    case Verdict::infeasible:
      return "infeasible";
    case Verdict::unknown:
      return "unknown";
  }
  return "unknown";
}

Statistics statistics() {
  Statistics result = accumulated;
  result.tu_admitted_queries = tu_budget.queries;
  result.tu_consumed_cfg_states = tu_budget.cfg_states;
  result.tu_consumed_symbolic_states = tu_budget.symbolic_states;
  result.tu_consumed_solver_checks = tu_budget.solver_checks;
  return result;
}

void reset_statistics() {
  accumulated = Statistics();
#ifdef STATIC_ANALYZER_HAVE_Z3
  tu_z3_context.reset();
  constant_return_cache.clear();
#endif
  start_tu_budget(configured_limits);
}

void dump_result(const Result &result, const char *purpose) {
  if (dump_file == nullptr)
    return;
  dump_printf(MSG_NOTE,
              "path-feasibility: purpose=%s verdict=%s backend=%s "
              "reason=%s solver-initialized=%s cfg-states=%u "
              "symbolic-states=%u checks=%u "
              "prunes=%u ",
              purpose == nullptr ? "unspecified" : purpose,
              verdict_name(result.verdict),
              result.used_solver ? "z3" : "cfg",
              result.reason.c_str(),
              result.solver_initialized ? "yes" : "no", result.cfg_states,
              result.symbolic_states, result.solver_checks,
              result.solver_prunes);
  dump_printf(MSG_NOTE, "exact-null-checks=%u exact-null-prunes=%u ",
              result.exact_null_checks, result.exact_null_prunes);
  dump_printf(MSG_NOTE, "exact-scalar-checks=%u exact-scalar-prunes=%u ",
              result.exact_scalar_checks, result.exact_scalar_prunes);
  dump_printf(MSG_NOTE,
              "abstracted-diamonds=%u abstracted-loops=%u "
              "exact-one-trip-unfolds=%u abstracted-blocks=%u "
              "encoded=%u unsupported=%u tu-budget-exhausted=%s "
              "truncated=%s\n",
              result.abstracted_diamonds, result.abstracted_loops,
              result.exact_one_trip_unfolds, result.abstracted_blocks,
              result.encoded_conditions,
              result.unsupported_conditions,
              result.tu_budget_exhausted ? "yes" : "no",
              result.truncated ? "yes" : "no");
}

void dump_statistics() {
  if (dump_file == nullptr)
    return;
  const Statistics current = statistics();
  dump_printf(MSG_NOTE,
              "path-feasibility-summary: queries=%lu solver=%lu "
              "solver-initializations=%lu cfg-only=%lu "
              "feasible=%lu infeasible=%lu unknown=%lu truncated=%lu "
              "cfg-states=%lu symbolic-states=%lu checks=%lu prunes=%lu ",
              current.queries, current.solver_queries,
              current.solver_initializations, current.cfg_only_queries,
              current.feasible, current.infeasible, current.unknown,
              current.truncated, current.cfg_states,
              current.symbolic_states, current.solver_checks,
              current.solver_prunes);
  dump_printf(MSG_NOTE, "exact-null-checks=%lu exact-null-prunes=%lu ",
              current.exact_null_checks, current.exact_null_prunes);
  dump_printf(MSG_NOTE, "exact-scalar-checks=%lu exact-scalar-prunes=%lu ",
              current.exact_scalar_checks, current.exact_scalar_prunes);
  dump_printf(MSG_NOTE,
              "abstracted-diamonds=%lu abstracted-loops=%lu "
              "exact-one-trip-unfolds=%lu abstracted-blocks=%lu ",
              current.abstracted_diamonds, current.abstracted_loops,
              current.exact_one_trip_unfolds, current.abstracted_blocks);
  // GCC's pretty-printer accepts fewer variadic arguments than printf.  Keep
  // this continuation separate so adding a counter cannot make dump output
  // trigger a compiler ICE.
  dump_printf(MSG_NOTE,
              "tu-admitted=%lu tu-cfg-used=%lu tu-symbolic-used=%lu "
              "tu-checks-used=%lu tu-budget-exhaustions=%lu tu-timeouts=%lu "
              "tu-query-cap=%lu tu-cfg-state-cap=%lu "
              "tu-symbolic-state-cap=%lu tu-solver-check-cap=%lu "
              "tu-query-limit=%u tu-cfg-state-limit=%u "
              "tu-symbolic-state-limit=%u tu-solver-check-limit=%u "
              "tu-time-limit-ms=%u\n",
              current.tu_admitted_queries,
              current.tu_consumed_cfg_states,
              current.tu_consumed_symbolic_states,
              current.tu_consumed_solver_checks,
              current.tu_budget_exhaustions,
              current.tu_timeout_exhaustions,
              current.tu_query_cap_exhaustions,
              current.tu_cfg_state_cap_exhaustions,
              current.tu_symbolic_state_cap_exhaustions,
              current.tu_solver_check_cap_exhaustions,
              tu_budget.limits.max_tu_queries,
              tu_budget.limits.max_tu_cfg_states,
              tu_budget.limits.max_tu_symbolic_states,
              tu_budget.limits.max_tu_solver_checks,
              tu_budget.limits.tu_timeout_ms);
}

}  // namespace feasibility
}  // namespace static_analyzer
