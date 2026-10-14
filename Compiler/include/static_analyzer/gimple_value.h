#ifndef STATIC_ANALYZER_GIMPLE_VALUE_H
#define STATIC_ANALYZER_GIMPLE_VALUE_H

#include "coretypes.h"

#include <vector>

namespace static_analyzer {

enum class OriginKind {
  allocation_call,
  reallocation_call,
  call_result,
  addressed_object,
  null_pointer,
  parameter,
  unknown
};

struct ValueOrigin {
  OriginKind kind = OriginKind::unknown;
  tree value = NULL_TREE;
  const gimple *defining_statement = nullptr;
  bool derived_pointer = false;
};

struct ValueExpansion {
  std::vector<ValueOrigin> origins;
  bool complete = true;
  bool cycle_detected = false;
  bool depth_exhausted = false;
  // A hard provenance work/memory bound stopped this expansion.  Such a
  // result always contains an explicit unknown origin and is never complete,
  // so callers cannot accidentally promote a bounded approximation to a
  // must-alias proof.
  bool budget_exhausted = false;
};

struct ValueExpansionStatistics {
  unsigned long requests = 0;
  unsigned long evaluations = 0;
  unsigned long hits = 0;
  unsigned long nodes = 0;
  unsigned long work = 0;
  unsigned long memo_hits = 0;
  unsigned long memo_entries = 0;
  unsigned long origins = 0;
  unsigned long budget_exhaustions = 0;
  unsigned long node_cap_exhaustions = 0;
  unsigned long work_cap_exhaustions = 0;
  unsigned long origin_cap_exhaustions = 0;
  unsigned long memo_cap_exhaustions = 0;
  unsigned long top_cache_cap_exhaustions = 0;
  unsigned int cache_entries = 0;
};

// Expand a pointer value through the GIMPLE SSA definition graph.  Copies,
// casts, PHIs, conditional values, addresses and pointer arithmetic are
// handled explicitly.  Unknown memory loads and opaque definitions remain
// visible as incomplete origins instead of being guessed through.
ValueExpansion expand_pointer_value(tree value, unsigned int max_depth = 64);

// Drop memoized expansions and function-scope statistics at a function
// boundary.  GIMPLE is immutable while the analysis pass runs, so repeated
// alias queries inside one function can safely reuse the same bounded
// provenance result.
void reset_value_expansion_cache();

ValueExpansionStatistics value_expansion_statistics();

// Writes one stable function-scope summary through GCC's dump machinery.
void dump_value_expansion_statistics();

const char *origin_kind_name(OriginKind kind);

}  // namespace static_analyzer

#endif
