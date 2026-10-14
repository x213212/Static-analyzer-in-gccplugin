#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "ssa.h"
#include "ssa-iterators.h"
#include "tree-phinodes.h"
#include "gimple-walk.h"
#include "dumpfile.h"

#include "static_analyzer/gimple_flow.h"
#include "static_analyzer/ownership_summary.h"

#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <vector>

namespace static_analyzer {
namespace {

constexpr unsigned int max_use_sites = 4096;
constexpr unsigned int max_use_work = 32768;

struct WorkItem {
  tree value;
  unsigned int depth;
  std::vector<edge> required_edges;
  std::vector<tree> ancestry;
};

struct MemoryReferenceSearch {
  tree value;
  bool found;
};

tree find_exact_value(tree *node, int *walk_subtrees, void *data) {
  MemoryReferenceSearch *search =
      static_cast<MemoryReferenceSearch *>(data);
  if (*node == search->value) {
    search->found = true;
    *walk_subtrees = 0;
    return *node;
  }
  // Declarations and types are not expression children of the reference.
  if (DECL_P(*node) || TYPE_P(*node))
    *walk_subtrees = 0;
  return NULL_TREE;
}

bool memory_reference_contains(gimple *, tree, tree full_operand,
                               void *data) {
  MemoryReferenceSearch *search =
      static_cast<MemoryReferenceSearch *>(data);
  if (search->found || full_operand == NULL_TREE)
    return search->found;
  tree operand = full_operand;
  walk_tree_without_duplicates(&operand, find_exact_value, search);
  return search->found;
}

bool dereferences_value(gimple *statement, tree value) {
  MemoryReferenceSearch search{value, false};
  walk_stmt_load_store_addr_ops(statement, &search,
                                memory_reference_contains,
                                memory_reference_contains, nullptr);
  return search.found;
}

bool pointer_ssa_name(tree value) {
  return value != NULL_TREE && TREE_CODE(value) == SSA_NAME &&
         TREE_TYPE(value) != NULL_TREE && POINTER_TYPE_P(TREE_TYPE(value));
}

struct StatementUses {
  unsigned int occurrences = 0;
  std::vector<edge> phi_edges;
};

StatementUses uses_on_statement(imm_use_iterator *iterator,
                                gimple *statement,
                                tree expected_value,
                                unsigned int *work,
                                bool *exhausted) {
  StatementUses result;
  use_operand_p operand;
  FOR_EACH_IMM_USE_ON_STMT(operand, *iterator) {
    // GCC 10 temporarily links iterator markers into the immediate-use list.
    // Keep traversing the macro even after the cap is reached so its increment
    // expression can remove those markers safely.
    if (*exhausted)
      continue;
    if (*work >= max_use_work) {
      *exhausted = true;
      continue;
    }
    ++*work;
    if (USE_FROM_PTR(operand) != expected_value)
      continue;
    ++result.occurrences;
    if (gimple_code(statement) == GIMPLE_PHI) {
      const unsigned int index = PHI_ARG_INDEX_FROM_USE(operand);
      result.phi_edges.push_back(
          gimple_phi_arg_edge(as_a<gphi *>(statement), index));
    }
  }
  return result;
}

UseSite classify_use(gimple *statement, tree incoming, unsigned int depth,
                     unsigned int occurrences,
                     const std::vector<edge> &required_edges) {
  UseSite site;
  site.statement = statement;
  site.incoming_value = incoming;
  site.depth = depth;
  site.occurrences = occurrences;
  site.required_edges = required_edges;

  if (gimple_code(statement) == GIMPLE_PHI) {
    tree result = gimple_phi_result(statement);
    if (pointer_ssa_name(result)) {
      site.edge = FlowEdgeKind::phi;
      site.produced_value = result;
      site.propagates_value = true;
    }
    return site;
  }

  if (is_gimple_assign(statement)) {
    tree lhs = gimple_assign_lhs(statement);
    const enum tree_code rhs_code = gimple_assign_rhs_code(statement);

    if (pointer_ssa_name(lhs) && gimple_assign_rhs1(statement) == incoming) {
      if (rhs_code == SSA_NAME)
        site.edge = FlowEdgeKind::copy;
      else if (gimple_assign_cast_p(statement))
        site.edge = FlowEdgeKind::cast;
      else if (rhs_code == POINTER_PLUS_EXPR)
        site.edge = FlowEdgeKind::pointer_offset;
      else
        return site;

      site.produced_value = lhs;
      site.propagates_value = true;
      return site;
    }

    if (pointer_ssa_name(lhs) && rhs_code == COND_EXPR &&
        (gimple_assign_rhs2(statement) == incoming ||
         gimple_assign_rhs3(statement) == incoming)) {
      site.edge = FlowEdgeKind::conditional;
      site.produced_value = lhs;
      site.propagates_value = true;
      return site;
    }

    // Ask GCC to enumerate the statement's actual load/store operands.  A
    // generic VUSE/VDEF only says that *some* memory is touched; it does not
    // prove that this incoming pointer supplies that memory address.
    if (dereferences_value(statement, incoming))
      site.edge = FlowEdgeKind::memory_reference;
    return site;
  }

  if (is_gimple_call(statement)) {
    site.edge = FlowEdgeKind::call_argument;
    // Returning an argument preserves the SSA value just as a copy does.
    // Propagate only an exact body/GCC theorem and retain PHI edge witnesses.
    if (ownership_summary::returned_argument(statement) == incoming &&
        pointer_ssa_name(gimple_call_lhs(statement))) {
      site.produced_value = gimple_call_lhs(statement);
      site.propagates_value = true;
    }
    return site;
  }

  if (gimple_code(statement) == GIMPLE_RETURN) {
    site.edge = FlowEdgeKind::return_value;
    return site;
  }

  return site;
}

}  // namespace

UseExpansion expand_immediate_uses(tree root, unsigned int max_depth,
                                   unsigned int max_values) {
  UseExpansion expansion;
  if (!pointer_ssa_name(root))
    return expansion;
  if (max_values == 0) {
    expansion.truncated = true;
    return expansion;
  }

  std::deque<WorkItem> worklist;
  std::map<tree, unsigned int> witnesses_per_value;
  std::set<tree> reachable_values;
  unsigned int work = 0;
  bool exhausted = false;
  worklist.push_back({root, 0, {}, {root}});
  witnesses_per_value[root] = 1;
  reachable_values.insert(root);
  expansion.values_visited = 1;

  while (!worklist.empty() && !exhausted) {
    if (work >= max_use_work) {
      expansion.truncated = true;
      break;
    }
    ++work;
    const WorkItem item = worklist.front();
    worklist.pop_front();

    imm_use_iterator iterator;
    gimple *statement;
    FOR_EACH_IMM_USE_STMT(statement, iterator, item.value) {
      if (exhausted)
        continue;
      if (work >= max_use_work) {
        exhausted = true;
        expansion.truncated = true;
        continue;
      }
      ++work;
      const StatementUses uses =
          uses_on_statement(&iterator, statement, item.value, &work,
                            &exhausted);
      if (exhausted) {
        expansion.truncated = true;
        continue;
      }
      const unsigned int alternatives =
          uses.phi_edges.empty() ? 1u : uses.phi_edges.size();
      for (unsigned int alternative = 0; alternative < alternatives;
           ++alternative) {
        if (expansion.sites.size() >= max_use_sites) {
          exhausted = true;
          expansion.truncated = true;
          break;
        }
        std::vector<edge> required_edges = item.required_edges;
        if (!uses.phi_edges.empty())
          required_edges.push_back(uses.phi_edges[alternative]);
        UseSite site = classify_use(statement, item.value, item.depth,
                                    uses.occurrences, required_edges);
        expansion.sites.push_back(site);

        if (!site.propagates_value)
          continue;
        if (item.depth >= max_depth) {
          if (reachable_values.count(site.produced_value) == 0)
            expansion.truncated = true;
          else
            expansion.witnesses_truncated = true;
          continue;
        }
        if (std::find(item.ancestry.begin(), item.ancestry.end(),
                      site.produced_value) != item.ancestry.end())
          continue;

        // Retain a small number of distinct acyclic SSA witnesses per value.
        // This avoids path explosion at PHI diamonds while not collapsing the
        // first incoming edge into an unrelated release arm.
        unsigned int &witnesses = witnesses_per_value[site.produced_value];
        if (witnesses >= 8) {
          expansion.witnesses_truncated = true;
          continue;
        }
        if (reachable_values.count(site.produced_value) == 0) {
          if (reachable_values.size() >= max_values) {
            expansion.truncated = true;
            continue;
          }
          reachable_values.insert(site.produced_value);
          expansion.values_visited = reachable_values.size();
        }
        ++witnesses;
        std::vector<tree> ancestry = item.ancestry;
        ancestry.push_back(site.produced_value);
        worklist.push_back({site.produced_value, item.depth + 1,
                            required_edges, ancestry});
      }
    }
  }

  if (exhausted)
    expansion.truncated = true;

  return expansion;
}

void dump_use_expansion(tree root, const UseExpansion &expansion) {
  if (dump_file == nullptr)
    return;

  dump_printf(MSG_NOTE,
              "value-flow: values=%u sites=%u truncated=%s "
              "witnesses-truncated=%s root=",
              expansion.values_visited,
              static_cast<unsigned int>(expansion.sites.size()),
              expansion.truncated ? "yes" : "no",
              expansion.witnesses_truncated ? "yes" : "no");
  dump_generic_expr(MSG_NOTE, TDF_SLIM, root);
  dump_printf(MSG_NOTE, "\n");

  if (!(dump_flags & TDF_DETAILS))
    return;

  for (const UseSite &site : expansion.sites) {
    dump_printf(MSG_NOTE,
                "  edge=%s depth=%u occurrences=%u propagates=%s phi-edges=%u",
                flow_edge_name(site.edge), site.depth, site.occurrences,
                site.propagates_value ? "yes" : "no",
                static_cast<unsigned int>(site.required_edges.size()));
    if (site.produced_value != NULL_TREE) {
      dump_printf(MSG_NOTE, " produced=");
      dump_generic_expr(MSG_NOTE, TDF_SLIM, site.produced_value);
    }
    dump_printf(MSG_NOTE, "\n");
    dump_gimple_stmt(MSG_NOTE, TDF_SLIM, site.statement, 0);
  }
}

const char *flow_edge_name(FlowEdgeKind edge) {
  switch (edge) {
    case FlowEdgeKind::direct_use:
      return "direct-use";
    case FlowEdgeKind::copy:
      return "copy";
    case FlowEdgeKind::cast:
      return "cast";
    case FlowEdgeKind::phi:
      return "phi";
    case FlowEdgeKind::conditional:
      return "conditional";
    case FlowEdgeKind::pointer_offset:
      return "pointer-offset";
    case FlowEdgeKind::call_argument:
      return "call-argument";
    case FlowEdgeKind::return_value:
      return "return";
    case FlowEdgeKind::memory_reference:
      return "memory-reference";
    case FlowEdgeKind::memory_load:
      return "memory-load";
  }
  return "direct-use";
}

}  // namespace static_analyzer
