#ifndef STATIC_ANALYZER_GIMPLE_FLOW_H
#define STATIC_ANALYZER_GIMPLE_FLOW_H

#include "coretypes.h"

#include <vector>

namespace static_analyzer {

enum class FlowEdgeKind {
  direct_use,
  copy,
  cast,
  phi,
  conditional,
  pointer_offset,
  call_argument,
  return_value,
  memory_reference,
  memory_load
};

struct UseSite {
  gimple *statement = nullptr;
  tree incoming_value = NULL_TREE;
  tree produced_value = NULL_TREE;
  FlowEdgeKind edge = FlowEdgeKind::direct_use;
  unsigned int depth = 0;
  unsigned int occurrences = 0;
  bool propagates_value = false;

  // Exact CFG edges that must be taken for this SSA provenance witness.  A
  // PHI argument is an edge use, not an ordinary block-local use; retaining
  // that edge keeps a freed arm correlated with the later merged value.
  std::vector<edge_def *> required_edges;
};

struct UseExpansion {
  std::vector<UseSite> sites;
  unsigned int values_visited = 0;
  // True only when the unique SSA reachability walk was incomplete.  Lifetime
  // leak reporting uses this bit because an unexplored value may hide a
  // release.
  bool truncated = false;

  // Distinct path witnesses for an already-reachable SSA name are capped
  // independently.  Losing an alternative witness can reduce path-sensitive
  // UAF recall, but it does not hide a reachable release and therefore must
  // not suppress otherwise complete leak analysis.
  bool witnesses_truncated = false;
};

// Follow the forward SSA use graph with GCC's immediate-use iterator.  The
// iterator finds uses; this function only decides which statements preserve a
// pointer value and therefore produce another SSA value worth enqueueing.
UseExpansion expand_immediate_uses(tree root, unsigned int max_depth = 64,
                                   unsigned int max_values = 10000);

// Emit the traversed graph through GCC's active pass dump stream.
void dump_use_expansion(tree root, const UseExpansion &expansion);

const char *flow_edge_name(FlowEdgeKind edge);

}  // namespace static_analyzer

#endif
