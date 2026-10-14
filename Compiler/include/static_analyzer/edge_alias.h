#ifndef STATIC_ANALYZER_EDGE_ALIAS_H
#define STATIC_ANALYZER_EDGE_ALIAS_H

#include "coretypes.h"

namespace static_analyzer {
namespace edge_alias {

enum class Result {
  proven_same_object,
  different_object,
  unknown
};

struct Evidence {
  Result result = Result::unknown;
  unsigned int reachable_phi_edges = 0;
  unsigned int unreachable_phi_edges = 0;
  unsigned int values_examined = 0;
  bool cycle_detected = false;
  bool epoch_cycle_detected = false;
  bool depth_exhausted = false;
  bool value_limit_exhausted = false;
  bool cfg_limit_exhausted = false;

  bool proven() const { return result == Result::proven_same_object; }
};

// Prove CANDIDATE equal to ALLOCATION on every value-flow alternative that
// can execute after ALLOCATION_STATEMENT.  This is intentionally relative to
// one allocation event: a PHI's incoming edge that cannot be reached from the
// event is irrelevant, while every reachable incoming edge must resolve to
// that exact, non-derived allocation.  Unsupported shapes, cycles and
// exhausted bounds return unknown and therefore never establish a release.
Evidence prove_same_object_after(tree candidate, tree allocation,
                                 const gimple *allocation_statement,
                                 unsigned int max_depth = 64,
                                 unsigned int max_values = 10000,
                                 unsigned int max_cfg_blocks = 4096);

void dump_evidence(tree candidate, tree allocation,
                   const Evidence &evidence);
const char *result_name(Result result);

}  // namespace edge_alias
}  // namespace static_analyzer

#endif
