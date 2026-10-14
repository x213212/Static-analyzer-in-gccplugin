#ifndef STATIC_ANALYZER_MEMORY_VALUE_H
#define STATIC_ANALYZER_MEMORY_VALUE_H

#include "coretypes.h"

#include <vector>

namespace static_analyzer {

// The outcome of resolving the value produced by a pointer-valued GIMPLE
// memory load.  COMPLETE is deliberately narrow: every path must reach the
// same direct pointer store that GCC proves completely overwrites the load,
// with no entry path or opaque clobber in between.
enum class MemoryValueStatus {
  not_a_pointer_load,
  not_an_integral_reference,
  complete,
  ambiguous,
  incomplete
};

enum class ReachingDefinitionKind {
  pointer_store,
  integral_store,
  imprecise_store,
  call_clobber,
  asm_clobber,
  explicit_clobber,
  other_clobber
};

// Strength of the relationship between the store LHS and the load reference.
// EXACT_REFERENCE proves equal addresses only; MUST_KILL is the sole state
// sufficient for a complete result.  MAY_OVERLAP is never promoted to a
// must-alias fact.
enum class MemoryAddressMatch {
  none,
  exact_reference,
  must_kill,
  may_overlap
};

struct ReachingMemoryDefinition {
  ReachingDefinitionKind kind = ReachingDefinitionKind::other_clobber;
  MemoryAddressMatch address_match = MemoryAddressMatch::none;
  const gimple *statement = nullptr;
  tree memory_reference = nullptr;
  tree stored_value = nullptr;
};

struct MemoryWalkLimits {
  // Maximum number of virtual-SSA states on any explored reaching-def path.
  unsigned int max_depth = 64;

  // Maximum number of virtual SSA states examined, including revisits through
  // separate PHI paths.  This is also used to bound GCC's alias walker.
  unsigned int max_states = 256;
};

struct MemoryValueResult {
  MemoryValueStatus status = MemoryValueStatus::not_a_pointer_load;
  tree loaded_value = nullptr;
  tree load_reference = nullptr;
  std::vector<ReachingMemoryDefinition> definitions;

  unsigned int visited_states = 0;
  unsigned int walked_vdefs = 0;
  bool function_entry_reached = false;
  bool cycle_detected = false;
  bool depth_exhausted = false;
  bool state_exhausted = false;
  bool missing_virtual_operand = false;
  bool volatile_load = false;

  // Returns the sole stored value only for a complete, typed result.
  tree unique_stored_value() const;
};

namespace memory_value {

// Per-function cache telemetry.  A request is every public
// resolve_pointer_load call; evaluations count cache misses that actually walk
// virtual SSA, and hits count results returned from the immutable function
// cache.  Thus requests == evaluations + hits after a reset.
struct Statistics {
  unsigned long requests = 0;
  unsigned long evaluations = 0;
  unsigned long hits = 0;
};

// The cache contains GCC tree/gimple pointers and depends on the active cfun.
// Call this after push_cfun and before resolving values in each function.
void reset_function_cache();
Statistics statistics();
void dump_statistics();

}  // namespace memory_value

// Resolve a pointer-valued GIMPLE load through Memory SSA.  LOAD_STATEMENT is
// expected to be an assignment such as:
//
//   value_7 = MEM[(struct object *)p_3].field;
//
// The result remains ambiguous/incomplete for multiple reaching stores,
// may-only address matches, calls, asm, clobbers, missing entry definitions,
// volatile accesses, cycles, or exhausted limits.  The function is read-only
// with respect to GIMPLE and requires virtual SSA to be available at the pass
// point.
//
// Results are memoized for the current function by LOAD_STATEMENT and both
// walk limits.  The return type intentionally remains a value: callers can
// modify their copy without changing the cached evidence.
MemoryValueResult resolve_pointer_load(
    const gimple *load_statement,
    const MemoryWalkLimits &limits = MemoryWalkLimits{});

// Resolve a pointer cell at the incoming memory version of a statement.
// This is the same MUST_KILL oracle as a real GIMPLE load, without inserting
// a synthetic load into GCC's IR.  Consumers of callee memory effects must
// query the caller's VUSE, not the callee's entry or the call's outgoing VDEF.
MemoryValueResult resolve_pointer_reference(
    tree reference, tree vuse,
    const MemoryWalkLimits &limits = MemoryWalkLimits{});

// The same incoming-memory proof for an integral cell. Pointer/integer
// representation punning is never a value proof: reference, store and RHS
// must have identical precision, mode and signedness. This does not read
// initializers or assume that a callee observes a caller's earlier load.
MemoryValueResult resolve_integral_reference(
    tree reference, tree vuse,
    const MemoryWalkLimits &limits = MemoryWalkLimits{});

// Emit structured evidence through GCC's pass dump stream.  The one-line
// summary is always present when dumping is enabled; reaching definitions are
// included for TDF_DETAILS.
void dump_memory_value_result(const MemoryValueResult &result);

const char *memory_value_status_name(MemoryValueStatus status);
const char *reaching_definition_kind_name(ReachingDefinitionKind kind);
const char *memory_address_match_name(MemoryAddressMatch match);

}  // namespace static_analyzer

#endif
