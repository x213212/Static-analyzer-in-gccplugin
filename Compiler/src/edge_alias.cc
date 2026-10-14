#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "basic-block.h"
#include "dumpfile.h"

#include "static_analyzer/edge_alias.h"
#include "static_analyzer/ownership_call.h"

#include <deque>
#include <map>
#include <set>

namespace static_analyzer {
namespace edge_alias {
namespace {

enum class Reachability {
  reachable,
  unreachable,
  unknown
};

struct Context {
  tree allocation = NULL_TREE;
  const gimple *allocation_statement = nullptr;
  unsigned int max_depth = 0;
  unsigned int max_values = 0;
  Evidence evidence;
  std::set<basic_block> reachable_blocks;
  std::set<basic_block> blocks_reaching_allocation;
  bool cfg_complete = true;
  bool reverse_cfg_complete = true;
  std::set<tree> active_values;
  std::map<std::pair<tree, unsigned int>, Result> memoized_values;
};

bool pointer_value(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
         CODE_CONTAINS_STRUCT(TREE_CODE(value), TS_TYPED) &&
         TREE_TYPE(value) != NULL_TREE &&
         POINTER_TYPE_P(TREE_TYPE(value));
}

bool pointer_preserving_cast_p(const gassign *assignment) {
  if (assignment == nullptr || !gimple_assign_cast_p(assignment))
    return false;
  tree lhs = gimple_assign_lhs(assignment);
  tree rhs = gimple_assign_rhs1(assignment);
  if (!pointer_value(lhs) || !pointer_value(rhs))
    return false;

  tree lhs_target = TREE_TYPE(TREE_TYPE(lhs));
  tree rhs_target = TREE_TYPE(TREE_TYPE(rhs));
  if (lhs_target == NULL_TREE || rhs_target == NULL_TREE ||
      TYPE_ADDR_SPACE(lhs_target) != TYPE_ADDR_SPACE(rhs_target))
    return false;
  return tree_nop_conversion_p(TREE_TYPE(lhs), TREE_TYPE(rhs));
}

void collect_reachable_blocks(const gimple *allocation_statement,
                              unsigned int max_blocks, Context *context) {
  basic_block start = gimple_bb(allocation_statement);
  if (start == nullptr || max_blocks == 0) {
    context->cfg_complete = false;
    context->evidence.cfg_limit_exhausted = max_blocks == 0;
    return;
  }

  std::deque<basic_block> worklist;
  worklist.push_back(start);
  while (!worklist.empty()) {
    basic_block block = worklist.front();
    worklist.pop_front();
    if (!context->reachable_blocks.insert(block).second)
      continue;
    if (context->reachable_blocks.size() > max_blocks) {
      context->cfg_complete = false;
      context->evidence.cfg_limit_exhausted = true;
      return;
    }

    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs)
      if (context->reachable_blocks.find(successor->dest) ==
          context->reachable_blocks.end())
        worklist.push_back(successor->dest);
  }
}

void collect_blocks_reaching_allocation(const gimple *allocation_statement,
                                        unsigned int max_blocks,
                                        Context *context) {
  basic_block start = gimple_bb(allocation_statement);
  if (start == nullptr || max_blocks == 0) {
    context->reverse_cfg_complete = false;
    context->evidence.cfg_limit_exhausted = max_blocks == 0;
    return;
  }

  std::deque<basic_block> worklist;
  worklist.push_back(start);
  while (!worklist.empty()) {
    basic_block block = worklist.front();
    worklist.pop_front();
    if (!context->blocks_reaching_allocation.insert(block).second)
      continue;
    if (context->blocks_reaching_allocation.size() > max_blocks) {
      context->reverse_cfg_complete = false;
      context->evidence.cfg_limit_exhausted = true;
      return;
    }

    edge predecessor;
    edge_iterator iterator;
    FOR_EACH_EDGE(predecessor, iterator, block->preds)
      if (context->blocks_reaching_allocation.find(predecessor->src) ==
          context->blocks_reaching_allocation.end())
        worklist.push_back(predecessor->src);
  }
}

Reachability edge_reachability(edge incoming, const Context &context) {
  if (incoming == nullptr)
    return Reachability::unknown;
  if (context.reachable_blocks.find(incoming->src) !=
      context.reachable_blocks.end())
    return Reachability::reachable;
  return context.cfg_complete ? Reachability::unreachable
                              : Reachability::unknown;
}

Result combine_required(Result left, Result right) {
  if (left == Result::different_object ||
      right == Result::different_object)
    return Result::different_object;
  if (left == Result::unknown || right == Result::unknown)
    return Result::unknown;
  return Result::proven_same_object;
}

Result prove_value(tree value, unsigned int depth, Context *context) {
  if (value == NULL_TREE || value == error_mark_node)
    return Result::unknown;
  if (depth == 0) {
    context->evidence.depth_exhausted = true;
    return Result::unknown;
  }
  if (TREE_CODE(value) == INTEGER_CST && integer_zerop(value))
    return Result::different_object;
  if (!pointer_value(value) || TREE_CODE(value) != SSA_NAME)
    return Result::unknown;

  const std::pair<tree, unsigned int> memo_key(value, depth);
  const auto memoized = context->memoized_values.find(memo_key);
  if (memoized != context->memoized_values.end())
    return memoized->second;
  if (++context->evidence.values_examined > context->max_values) {
    context->evidence.value_limit_exhausted = true;
    return Result::unknown;
  }
  if (!context->active_values.insert(value).second) {
    context->evidence.cycle_detected = true;
    return Result::unknown;
  }

  const gimple *definition = SSA_NAME_DEF_STMT(value);
  Result result = Result::unknown;
  if (definition == context->allocation_statement &&
      value == context->allocation && is_gimple_call(definition) &&
      (ownership_allocation_call_p(definition) ||
       ownership_reallocation_call_p(definition))) {
    result = Result::proven_same_object;
  } else if (definition == nullptr ||
             gimple_code(definition) == GIMPLE_NOP) {
    result = Result::unknown;
  } else if (gimple_code(definition) == GIMPLE_PHI) {
    const gphi *phi = as_a<const gphi *>(definition);
    basic_block phi_block = gimple_bb(phi);
    const bool allocation_reaches_phi =
        context->reachable_blocks.find(phi_block) !=
        context->reachable_blocks.end();
    const bool phi_reaches_allocation =
        context->blocks_reaching_allocation.find(phi_block) !=
        context->blocks_reaching_allocation.end();

    // A static SSA name on a loop backedge can denote an allocation produced
    // by an earlier dynamic execution of the same statement.  Structural
    // identity of the defining statement is therefore not allocation-epoch
    // identity.  If allocation and PHI are in the same CFG SCC, or a bounded
    // reverse walk cannot disprove that fact, refuse the release proof.
    if (allocation_reaches_phi && phi_reaches_allocation) {
      context->evidence.epoch_cycle_detected = true;
      result = Result::unknown;
    } else if (allocation_reaches_phi && !context->reverse_cfg_complete) {
      result = Result::unknown;
    } else {
      bool saw_reachable_edge = false;
      result = Result::proven_same_object;
      const unsigned int argument_count = gimple_phi_num_args(phi);
      if (argument_count == 0)
        result = Result::unknown;
      for (unsigned int index = 0; index < argument_count; ++index) {
        edge incoming = gimple_phi_arg_edge(phi, index);
        const Reachability reachability =
            edge_reachability(incoming, *context);
        if (reachability == Reachability::unreachable) {
          ++context->evidence.unreachable_phi_edges;
          continue;
        }
        if (reachability == Reachability::unknown) {
          result = Result::unknown;
          break;
        }

        saw_reachable_edge = true;
        ++context->evidence.reachable_phi_edges;
        result = combine_required(
            result,
            prove_value(gimple_phi_arg_def(phi, index), depth - 1, context));
        if (result == Result::different_object)
          break;
      }
      if (!saw_reachable_edge)
        result = Result::unknown;
    }
  } else if (is_gimple_assign(definition)) {
    const gassign *assignment = as_a<const gassign *>(definition);
    const enum tree_code rhs_code = gimple_assign_rhs_code(assignment);
    if (rhs_code == SSA_NAME || pointer_preserving_cast_p(assignment)) {
      result = prove_value(gimple_assign_rhs1(assignment), depth - 1,
                           context);
    } else if (rhs_code == COND_EXPR) {
      const Result true_result =
          prove_value(gimple_assign_rhs2(assignment), depth - 1, context);
      const Result false_result =
          prove_value(gimple_assign_rhs3(assignment), depth - 1, context);
      result = combine_required(true_result, false_result);
    } else if (rhs_code == POINTER_PLUS_EXPR) {
      // Interior pointers share storage, but free/delete require the exact
      // allocation base.  Never turn a derived pointer into release proof.
      result = Result::different_object;
    } else {
      result = Result::unknown;
    }
  } else if (is_gimple_call(definition)) {
    // A distinct call result may alias according to GCC's may-alias oracle,
    // but it is not proof of this allocation's ownership identity.
    result = Result::different_object;
  }

  context->active_values.erase(value);
  context->memoized_values.insert(std::make_pair(memo_key, result));
  return result;
}

}  // namespace

Evidence prove_same_object_after(tree candidate, tree allocation,
                                 const gimple *allocation_statement,
                                 unsigned int max_depth,
                                 unsigned int max_values,
                                 unsigned int max_cfg_blocks) {
  Context context;
  context.allocation = allocation;
  context.allocation_statement = allocation_statement;
  context.max_depth = max_depth;
  context.max_values = max_values;

  if (!pointer_value(candidate) || !pointer_value(allocation) ||
      allocation_statement == nullptr ||
      gimple_bb(allocation_statement) == nullptr || max_depth == 0 ||
      max_values == 0) {
    context.evidence.depth_exhausted = max_depth == 0;
    context.evidence.value_limit_exhausted = max_values == 0;
    return context.evidence;
  }

  collect_reachable_blocks(allocation_statement, max_cfg_blocks, &context);
  collect_blocks_reaching_allocation(allocation_statement, max_cfg_blocks,
                                     &context);
  context.evidence.result = prove_value(candidate, max_depth, &context);
  return context.evidence;
}

void dump_evidence(tree candidate, tree allocation,
                   const Evidence &evidence) {
  if (dump_file == nullptr || !(dump_flags & TDF_DETAILS))
    return;
  dump_printf(MSG_NOTE,
              "edge-alias-query: result=%s reachable-phi-edges=%u "
              "ignored-phi-edges=%u values=%u cycle=%s epoch-cycle=%s "
              "depth-limit=%s value-limit=%s cfg-limit=%s\n",
              result_name(evidence.result), evidence.reachable_phi_edges,
              evidence.unreachable_phi_edges, evidence.values_examined,
              evidence.cycle_detected ? "yes" : "no",
              evidence.epoch_cycle_detected ? "yes" : "no",
              evidence.depth_exhausted ? "yes" : "no",
              evidence.value_limit_exhausted ? "yes" : "no",
              evidence.cfg_limit_exhausted ? "yes" : "no");
  dump_printf(MSG_NOTE, "  candidate: ");
  dump_generic_expr(MSG_NOTE, TDF_SLIM, candidate);
  dump_printf(MSG_NOTE, "\n  allocation: ");
  dump_generic_expr(MSG_NOTE, TDF_SLIM, allocation);
  dump_printf(MSG_NOTE, "\n");
}

const char *result_name(Result result) {
  switch (result) {
    case Result::proven_same_object:
      return "same-object";
    case Result::different_object:
      return "different-object";
    case Result::unknown:
      return "unknown";
  }
  return "unknown";
}

}  // namespace edge_alias
}  // namespace static_analyzer
