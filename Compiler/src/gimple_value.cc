#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "dumpfile.h"

#include "static_analyzer/gimple_value.h"
#include "static_analyzer/memory_value.h"
#include "static_analyzer/ownership_call.h"
#include "static_analyzer/ownership_summary.h"

#include <algorithm>
#include <functional>
#include <set>
#include <unordered_map>
#include <utility>

namespace static_analyzer {
namespace {

// These are hard implementation bounds rather than tuning hints.  Pointer
// provenance is a diagnostic refinement, so exhausting one must lose
// precision (complete=false) instead of risking exponential compile time or
// unbounded per-function memory.
constexpr unsigned int max_expansion_nodes = 4096;
constexpr unsigned int max_expansion_work = 32768;
constexpr unsigned int max_expansion_origins = 128;
constexpr unsigned int max_local_memo_entries = 2048;
constexpr unsigned int max_top_level_cache_entries = 2048;

enum class BudgetLimit {
  none,
  nodes,
  work,
  origins,
  local_memo,
  top_level_cache
};

struct ExpansionKey {
  tree value = NULL_TREE;
  unsigned int remaining_depth = 0;
  bool derived_pointer = false;
};

struct ExpansionKeyHash {
  std::size_t operator()(const ExpansionKey &key) const {
    std::size_t result = std::hash<tree>()(key.value);
    const auto combine = [&result](std::size_t value) {
      result ^= value + static_cast<std::size_t>(0x9e3779b9U) +
                (result << 6) + (result >> 2);
    };
    combine(std::hash<unsigned int>()(key.remaining_depth));
    combine(std::hash<bool>()(key.derived_pointer));
    return result;
  }
};

struct ExpansionKeyEqual {
  bool operator()(const ExpansionKey &left,
                  const ExpansionKey &right) const {
    return left.value == right.value &&
           left.remaining_depth == right.remaining_depth &&
           left.derived_pointer == right.derived_pointer;
  }
};

using ExpansionCache =
    std::unordered_map<ExpansionKey, ValueExpansion, ExpansionKeyHash,
                       ExpansionKeyEqual>;

ExpansionCache expansion_cache;
ValueExpansionStatistics function_statistics;

bool same_origin(const ValueOrigin &lhs, const ValueOrigin &rhs) {
  return lhs.kind == rhs.kind && lhs.value == rhs.value &&
         lhs.defining_statement == rhs.defining_statement &&
         lhs.derived_pointer == rhs.derived_pointer;
}

const gimple *definition_for(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
                 TREE_CODE(value) == SSA_NAME
             ? SSA_NAME_DEF_STMT(value)
             : nullptr;
}

ValueExpansion unknown_origin(tree value, const gimple *statement,
                              bool derived_pointer) {
  ValueExpansion result;
  result.complete = false;
  result.origins.push_back(
      {OriginKind::unknown, value, statement, derived_pointer});
  return result;
}

void mark_budget_exhausted(ValueExpansion *expansion, tree value,
                           const gimple *statement,
                           bool derived_pointer) {
  expansion->complete = false;
  if (expansion->budget_exhausted)
    return;
  expansion->budget_exhausted = true;

  // Normal insertion reserves one slot for this conservative sentinel.  The
  // replacement is a defensive fallback for a result produced by an older
  // cache entry or a future caller that did not preserve that invariant.
  const ValueOrigin unknown{OriginKind::unknown, value, statement,
                            derived_pointer};
  if (!expansion->origins.empty() &&
      same_origin(expansion->origins.back(), unknown))
    return;
  if (expansion->origins.size() < max_expansion_origins)
    expansion->origins.push_back(unknown);
  else if (!expansion->origins.empty())
    expansion->origins.back() = unknown;
}

class ExpansionContext {
 public:
  bool consume_work() {
    if (limit_ != BudgetLimit::none)
      return false;
    if (work_ >= max_expansion_work) {
      exhaust(BudgetLimit::work);
      return false;
    }
    ++work_;
    return true;
  }

  bool consume_node() {
    if (limit_ != BudgetLimit::none)
      return false;
    if (nodes_ >= max_expansion_nodes) {
      exhaust(BudgetLimit::nodes);
      return false;
    }
    ++nodes_;
    return true;
  }

  bool lookup(const ExpansionKey &key, ValueExpansion *result) {
    const auto cached = memo_.find(key);
    if (cached == memo_.end())
      return false;
    ++memo_hits_;
    *result = cached->second;
    return true;
  }

  bool remember(const ExpansionKey &key, ValueExpansion *result) {
    // A cycle answer depends on the current recursion stack.  Reusing it when
    // the same SSA node is later reached outside that stack would discard a
    // valid acyclic arm, so cycle-tainted results never enter the normal memo.
    if (result->cycle_detected || result->budget_exhausted)
      return true;
    if (memo_.size() >= max_local_memo_entries) {
      exhaust(BudgetLimit::local_memo);
      mark_budget_exhausted(result, key.value,
                            definition_for(key.value),
                            key.derived_pointer);
      return false;
    }
    const auto inserted = memo_.insert(std::make_pair(key, *result));
    if (inserted.second)
      ++memo_entries_;
    return true;
  }

  void exhaust(BudgetLimit limit) {
    if (limit_ == BudgetLimit::none)
      limit_ = limit;
  }

  bool exhausted() const { return limit_ != BudgetLimit::none; }
  BudgetLimit limit() const { return limit_; }
  unsigned long nodes() const { return nodes_; }
  unsigned long work() const { return work_; }
  unsigned long memo_hits() const { return memo_hits_; }
  unsigned long memo_entries() const { return memo_entries_; }

 private:
  ExpansionCache memo_;
  BudgetLimit limit_ = BudgetLimit::none;
  unsigned long nodes_ = 0;
  unsigned long work_ = 0;
  unsigned long memo_hits_ = 0;
  unsigned long memo_entries_ = 0;
};

bool append_origin(ExpansionContext *context, ValueExpansion *expansion,
                   const ValueOrigin &origin, tree owner_value,
                   const gimple *owner_statement, bool owner_derived) {
  for (const ValueOrigin &candidate : expansion->origins) {
    if (!context->consume_work()) {
      mark_budget_exhausted(expansion, owner_value, owner_statement,
                            owner_derived);
      return false;
    }
    if (same_origin(candidate, origin))
      return true;
  }

  // Keep one emergency slot so a cap can never drop the unknown sentinel.
  if (expansion->origins.size() >= max_expansion_origins - 1) {
    context->exhaust(BudgetLimit::origins);
    mark_budget_exhausted(expansion, owner_value, owner_statement,
                          owner_derived);
    return false;
  }
  expansion->origins.push_back(origin);
  return true;
}

void append_expansion(ExpansionContext *context,
                      ValueExpansion *destination,
                      const ValueExpansion &source, tree owner_value,
                      const gimple *owner_statement, bool owner_derived) {
  destination->complete = destination->complete && source.complete;
  destination->cycle_detected =
      destination->cycle_detected || source.cycle_detected;
  destination->depth_exhausted =
      destination->depth_exhausted || source.depth_exhausted;
  if (context->exhausted() || source.budget_exhausted) {
    mark_budget_exhausted(destination, owner_value, owner_statement,
                          owner_derived);
    return;
  }

  for (const ValueOrigin &origin : source.origins)
    if (!append_origin(context, destination, origin, owner_value,
                       owner_statement, owner_derived))
      return;
}

void account_budget_limit(BudgetLimit limit) {
  if (limit == BudgetLimit::none)
    return;
  ++function_statistics.budget_exhaustions;
  switch (limit) {
    case BudgetLimit::nodes:
      ++function_statistics.node_cap_exhaustions;
      break;
    case BudgetLimit::work:
      ++function_statistics.work_cap_exhaustions;
      break;
    case BudgetLimit::origins:
      ++function_statistics.origin_cap_exhaustions;
      break;
    case BudgetLimit::local_memo:
      ++function_statistics.memo_cap_exhaustions;
      break;
    case BudgetLimit::top_level_cache:
      ++function_statistics.top_cache_cap_exhaustions;
      break;
    case BudgetLimit::none:
      break;
  }
}

bool pointer_preserving_cast_p(const gimple *statement) {
  if (!is_gimple_assign(statement) ||
      !gimple_assign_cast_p(statement))
    return false;
  tree lhs = gimple_assign_lhs(statement);
  tree rhs = gimple_assign_rhs1(statement);
  if (lhs == NULL_TREE || rhs == NULL_TREE || TREE_TYPE(lhs) == NULL_TREE ||
      TREE_TYPE(rhs) == NULL_TREE || !POINTER_TYPE_P(TREE_TYPE(lhs)) ||
      !POINTER_TYPE_P(TREE_TYPE(rhs)))
    return false;

  tree lhs_target = TREE_TYPE(TREE_TYPE(lhs));
  tree rhs_target = TREE_TYPE(TREE_TYPE(rhs));
  if (lhs_target == NULL_TREE || rhs_target == NULL_TREE ||
      TYPE_ADDR_SPACE(lhs_target) != TYPE_ADDR_SPACE(rhs_target))
    return false;

  // GCC's predicate rejects conversions that need an instruction (including
  // width-changing representations).  Requiring pointers on both sides is
  // intentional: even a pointer-sized integer round trip is not ownership
  // proof, and a narrowing integer round trip is necessarily incomplete.
  return tree_nop_conversion_p(TREE_TYPE(lhs), TREE_TYPE(rhs));
}

ValueExpansion budget_unknown(tree value, bool derived_pointer) {
  ValueExpansion result;
  mark_budget_exhausted(&result, value, definition_for(value),
                        derived_pointer);
  return result;
}

ValueExpansion expand_impl(tree value, unsigned int depth,
                           bool derived_pointer, ExpansionContext *context,
                           std::set<tree> *active_values) {
  if (!context->consume_work())
    return budget_unknown(value, derived_pointer);

  const ExpansionKey key{value, depth, derived_pointer};

  // Preserve the historical precedence: reaching the caller-selected depth
  // bound is a depth exhaustion even if the same value also closes a cycle.
  if (depth == 0) {
    ValueExpansion cached;
    if (context->lookup(key, &cached))
      return cached;
    if (!context->consume_node())
      return budget_unknown(value, derived_pointer);
    ValueExpansion result = unknown_origin(value, definition_for(value),
                                           derived_pointer);
    result.depth_exhausted = true;
    context->remember(key, &result);
    return result;
  }

  if (value != NULL_TREE && value != error_mark_node &&
      TREE_CODE(value) == SSA_NAME &&
      active_values->find(value) != active_values->end()) {
    // This answer is specific to the current recursion stack and is
    // deliberately returned before consulting/storing the normal memo.
    ValueExpansion result = unknown_origin(value, definition_for(value),
                                           derived_pointer);
    result.cycle_detected = true;
    return result;
  }

  ValueExpansion cached;
  if (context->lookup(key, &cached))
    return cached;
  if (!context->consume_node())
    return budget_unknown(value, derived_pointer);

  if (value == NULL_TREE) {
    ValueExpansion result = unknown_origin(value, nullptr, derived_pointer);
    context->remember(key, &result);
    return result;
  }

  if (TREE_CODE(value) == ADDR_EXPR) {
    ValueExpansion result;
    append_origin(context, &result,
                  {OriginKind::addressed_object, TREE_OPERAND(value, 0),
                   nullptr, derived_pointer},
                  value, nullptr, derived_pointer);
    context->remember(key, &result);
    return result;
  }

  if (TREE_CODE(value) == INTEGER_CST && integer_zerop(value)) {
    ValueExpansion result;
    append_origin(context, &result,
                  {OriginKind::null_pointer, value, nullptr,
                   derived_pointer},
                  value, nullptr, derived_pointer);
    context->remember(key, &result);
    return result;
  }

  if (TREE_CODE(value) != SSA_NAME) {
    ValueExpansion result = unknown_origin(value, nullptr, derived_pointer);
    context->remember(key, &result);
    return result;
  }

  active_values->insert(value);
  const gimple *definition = SSA_NAME_DEF_STMT(value);
  ValueExpansion result;

  if (definition == nullptr || gimple_code(definition) == GIMPLE_NOP) {
    result.complete = false;
    append_origin(context, &result,
                  {OriginKind::parameter, value, definition,
                   derived_pointer},
                  value, definition, derived_pointer);
  } else if (gimple_code(definition) == GIMPLE_PHI) {
    const gphi *phi = as_a<const gphi *>(definition);
    const unsigned int argument_count = gimple_phi_num_args(phi);
    if (argument_count == 0)
      result = unknown_origin(value, definition, derived_pointer);
    for (unsigned int index = 0;
         index < argument_count && !context->exhausted(); ++index) {
      ValueExpansion argument =
          expand_impl(gimple_phi_arg_def(phi, index), depth - 1,
                      derived_pointer, context, active_values);
      append_expansion(context, &result, argument, value, definition,
                       derived_pointer);
    }
  } else if (is_gimple_call(definition)) {
    tree returned = ownership_summary::returned_argument(definition);
    if (returned != NULL_TREE) {
      result = expand_impl(returned, depth - 1, derived_pointer, context,
                           active_values);
    } else {
      OriginKind kind = OriginKind::call_result;
      if (ownership_allocation_call_p(definition) ||
          ownership_summary::call_returns_fresh_owned(definition))
        kind = OriginKind::allocation_call;
      else if (ownership_reallocation_call_p(definition))
        kind = OriginKind::reallocation_call;
      append_origin(context, &result,
                    {kind, value, definition, derived_pointer}, value,
                    definition, derived_pointer);
    }
  } else if (is_gimple_assign(definition)) {
    const enum tree_code rhs_code = gimple_assign_rhs_code(definition);
    tree rhs1 = gimple_assign_rhs1(definition);

    if (gimple_assign_load_p(definition)) {
      // A memory load is not equal to its address operand.  Recover its value
      // from virtual SSA only when every reaching path has one must-quality
      // pointer store; ambiguous or incomplete walks remain unknown.
      const MemoryValueResult memory_value = resolve_pointer_load(definition);
      dump_memory_value_result(memory_value);
      tree stored_value = memory_value.unique_stored_value();
      if (stored_value != NULL_TREE)
        result = expand_impl(stored_value, depth - 1, derived_pointer,
                             context, active_values);
      else
        result = unknown_origin(value, definition, derived_pointer);
    } else if (rhs_code == SSA_NAME ||
               pointer_preserving_cast_p(definition)) {
      result = expand_impl(rhs1, depth - 1, derived_pointer, context,
                           active_values);
    } else if (rhs_code == ADDR_EXPR) {
      result = expand_impl(rhs1, depth - 1, derived_pointer, context,
                           active_values);
    } else if (rhs_code == POINTER_PLUS_EXPR) {
      result = expand_impl(rhs1, depth - 1, true, context, active_values);
    } else if (rhs_code == COND_EXPR) {
      ValueExpansion true_value =
          expand_impl(gimple_assign_rhs2(definition), depth - 1,
                      derived_pointer, context, active_values);
      append_expansion(context, &result, true_value, value, definition,
                       derived_pointer);
      if (!context->exhausted()) {
        ValueExpansion false_value =
            expand_impl(gimple_assign_rhs3(definition), depth - 1,
                        derived_pointer, context, active_values);
        append_expansion(context, &result, false_value, value, definition,
                         derived_pointer);
      }
    } else if (rhs1 != NULL_TREE && TREE_CODE(rhs1) == INTEGER_CST &&
               integer_zerop(rhs1)) {
      result = expand_impl(rhs1, depth - 1, derived_pointer, context,
                           active_values);
    } else {
      // In particular, do not follow the address operand of MEM_REF as if a
      // loaded pointer value were identical to the address used for the load.
      result = unknown_origin(value, definition, derived_pointer);
    }
  } else {
    result = unknown_origin(value, definition, derived_pointer);
  }

  active_values->erase(value);
  if (context->exhausted())
    mark_budget_exhausted(&result, value, definition, derived_pointer);
  context->remember(key, &result);
  return result;
}

}  // namespace

ValueExpansion expand_pointer_value(tree value, unsigned int max_depth) {
  ++function_statistics.requests;
  const ExpansionKey key{value, max_depth, false};
  const auto cached = expansion_cache.find(key);
  if (cached != expansion_cache.end()) {
    ++function_statistics.hits;
    return cached->second;
  }

  ++function_statistics.evaluations;
  if (expansion_cache.size() >= max_top_level_cache_entries) {
    ValueExpansion result = budget_unknown(value, false);
    account_budget_limit(BudgetLimit::top_level_cache);
    function_statistics.origins += result.origins.size();
    return result;
  }

  ExpansionContext context;
  std::set<tree> active_values;
  ValueExpansion expansion =
      expand_impl(value, max_depth, false, &context, &active_values);
  if (context.exhausted())
    mark_budget_exhausted(&expansion, value, definition_for(value), false);

  function_statistics.nodes += context.nodes();
  function_statistics.work += context.work();
  function_statistics.memo_hits += context.memo_hits();
  function_statistics.memo_entries += context.memo_entries();
  function_statistics.origins += expansion.origins.size();
  account_budget_limit(context.limit());
  expansion_cache.insert(std::make_pair(key, expansion));
  return expansion;
}

void reset_value_expansion_cache() {
  expansion_cache.clear();
  function_statistics = ValueExpansionStatistics{};
}

ValueExpansionStatistics value_expansion_statistics() {
  ValueExpansionStatistics result = function_statistics;
  result.cache_entries =
      static_cast<unsigned int>(expansion_cache.size());
  return result;
}

void dump_value_expansion_statistics() {
  if (dump_file == nullptr)
    return;
  const ValueExpansionStatistics current = value_expansion_statistics();
  dump_printf(MSG_NOTE,
              "gimple-value-cache summary: requests=%lu evaluations=%lu "
              "hits=%lu entries=%u nodes=%lu work=%lu memo-hits=%lu "
              "memo-entries=%lu origins=%lu budget-exhaustions=%lu "
              "node-cap-exhaustions=%lu work-cap-exhaustions=%lu "
              "origin-cap-exhaustions=%lu memo-cap-exhaustions=%lu "
              "top-cache-cap-exhaustions=%lu node-limit=%u work-limit=%u "
              "origin-limit=%u memo-limit=%u top-cache-limit=%u\n",
              current.requests, current.evaluations, current.hits,
              current.cache_entries, current.nodes, current.work,
              current.memo_hits, current.memo_entries, current.origins,
              current.budget_exhaustions,
              current.node_cap_exhaustions,
              current.work_cap_exhaustions,
              current.origin_cap_exhaustions,
              current.memo_cap_exhaustions,
              current.top_cache_cap_exhaustions,
              max_expansion_nodes, max_expansion_work,
              max_expansion_origins, max_local_memo_entries,
              max_top_level_cache_entries);
}

const char *origin_kind_name(OriginKind kind) {
  switch (kind) {
    case OriginKind::allocation_call:
      return "allocation-call";
    case OriginKind::reallocation_call:
      return "reallocation-call";
    case OriginKind::call_result:
      return "call-result";
    case OriginKind::addressed_object:
      return "addressed-object";
    case OriginKind::null_pointer:
      return "null";
    case OriginKind::parameter:
      return "parameter";
    case OriginKind::unknown:
      return "unknown";
  }
  return "unknown";
}

}  // namespace static_analyzer
