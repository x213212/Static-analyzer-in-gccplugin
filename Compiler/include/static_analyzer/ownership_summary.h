#ifndef STATIC_ANALYZER_OWNERSHIP_SUMMARY_H
#define STATIC_ANALYZER_OWNERSHIP_SUMMARY_H

#include "coretypes.h"

#include <vector>
#include <string>

namespace static_analyzer {
enum class CalleeModel;
namespace ownership_summary {

// Normal-return outcomes for one formal pointer.  These are a set, not an
// ordering: in particular, observing FREED on one path never turns a
// LIVE|FREED result into a must-free fact.
enum ParamFate : unsigned int {
  fate_none = 0,
  fate_live = 1u << 0,
  fate_freed = 1u << 1,
  fate_escaped = 1u << 2,
  fate_unknown = 1u << 3
};

// A positive access guarantee is stronger than the existing
// may_dereference bit.  It is either derived from a body theorem or supplied
// as an explicit trusted declaration contract; ordinary GCC access metadata
// is not a must-access promise.
enum class AccessGuarantee {
  none,
  must_access,
  access_if_nonnull,
  // A library entry precondition, not proof that bytes are physically read.
  // In particular puts requires a valid string even on an I/O error, but may
  // pass that string to callbacks which retain or free it during the call.
  requires_live,
  requires_live_if_nonnull
};

enum class ProofOrigin {
  none,
  body,
  provider_body,
  gcc_metadata,
  user_contract,
  user_assumption,
  library_model
};

// Report-only evidence from a completed release proof.  An unknown or
// ambiguous source stays unknown; this never strengthens an ownership fate.
struct ReleaseOrigin {
  location_t location = UNKNOWN_LOCATION;
  bool through_memory = false;
};

struct ArgumentEffect {
  // False means no complete ownership effect was established from a bound
  // body, GCC metadata, or an explicit declaration contract.  Unmodeled
  // returned ownership also makes this false.  Never infer a release then.
  bool resolved = false;
  unsigned int fates = fate_unknown;
  bool may_dereference = true;
  AccessGuarantee access = AccessGuarantee::none;
  ProofOrigin ownership_origin = ProofOrigin::none;
  ProofOrigin access_origin = ProofOrigin::none;
  ReleaseOrigin release_origin;
  location_t access_location = UNKNOWN_LOCATION;

  bool may_remain_live() const {
    return !resolved || (fates & (fate_live | fate_unknown)) != 0;
  }
  bool may_free() const {
    return resolved && (fates & fate_freed) != 0;
  }
  bool may_escape() const {
    return resolved && (fates & fate_escaped) != 0;
  }
  bool definitely_freed() const {
    return resolved && fates == fate_freed;
  }
  bool definitely_consumed() const {
    return resolved && fates != fate_none &&
           (fates & (fate_live | fate_unknown)) == 0;
  }
  bool has_access_guarantee() const {
    return access != AccessGuarantee::none;
  }
};

// Build translation-unit-local summaries to a fixed point.  This is a
// separate phase from diagnostics so wrapper calls have stable meaning no
// matter which cgraph node GCC happens to enumerate first.
void build();
void reset();
void configure_callee_model(CalleeModel model, const std::string &symbol,
                            unsigned int argument_index);

// Initial LTO producer hook: publish bounded body-derived parameter effects
// through GCC's declaration streamer.  No custom trusted contract or return
// ownership theorem is exported; consumers still require exact body binding.
void generate_provider_facts();

enum class ReturnedObjectLifetime { unknown, live, released };

struct ReturnedObjectEffect {
  bool resolved = false;
  ReturnedObjectLifetime lifetime = ReturnedObjectLifetime::unknown;
  bool may_be_null = true;
  location_t allocation_location = UNKNOWN_LOCATION;
  location_t release_location = UNKNOWN_LOCATION;
  location_t return_location = UNKNOWN_LOCATION;
};

// Independent identity/lifetime theorem for the exact call result, NOT a
// live/fresh ownership or no-alias theorem and NOT a formal-argument effect.
// Initially only a nonnull callee-local allocation released before its
// exact normal return is supported.  Loaded, original, exactly bound bodies
// only; mixed/unknown returns and provider-imported facts stay unresolved.
ReturnedObjectEffect returned_object_effect(const gimple *statement);

// True only for a body-bound call whose every reachable normal return is
// either NULL or a fresh owned object, with at least one fresh alternative.
// The caller's GIMPLE_CALL remains the allocation identity.
bool call_returns_fresh_owned(const gimple *statement);

// Stronger proof-only form: every modeled normal return is fresh-owned.
// Unlike call_returns_fresh_owned(), an explicit NULL alternative is not
// accepted because caller argument specialization is not implemented yet.
bool call_returns_definitely_fresh_owned(const gimple *statement);

// Source sites of the underlying fresh allocations for a summarized wrapper
// call.  The caller call remains the ownership identity; these locations are
// report trace evidence only and never participate in alias decisions.
std::vector<location_t> call_fresh_origin_locations(
    const gimple *statement);

// True when a bound body or GCC ERF_RETURNS_ARG proves the return equals the
// exact, non-derived actual at ARGUMENT_INDEX.  The forward/backward SSA
// walkers substitute this value; a generic may-alias hint is insufficient.
bool call_returns_argument(const gimple *statement,
                           unsigned int argument_index);

// Exact returned actual established by a body-bound summary or GCC's
// ERF_RETURNS_ARG metadata.  NULL_TREE means no such theorem is available.
tree returned_argument(const gimple *statement);

// Query the normal-return effect on the exact lowered formal/actual index.
// DECL_ARGUMENTS order is used by the implementation, including artificial
// C++ parameters.  Count/type mismatches return an unresolved effect.
ArgumentEffect argument_effect(const gimple *statement,
                               unsigned int argument_index);

struct MemoryReleaseEffect {
  // A pointer-valued memory reference instantiated in the caller, NOT the
  // loaded pointer and NOT an effect on the container/formal pointer itself.
  // Consumers must resolve it at this call's entry VUSE and require a unique
  // complete MUST_KILL reaching pointer store before establishing a release.
  tree reference = nullptr;
  location_t release_location = UNKNOWN_LOCATION;
};

// Loaded, exactly bound straight-line body theorem: the value read from
// reference on entry is released before a normal return. No theorem is
// imported from the provider transport by this API.
std::vector<MemoryReleaseEffect> memory_release_effects(const gimple *call);

struct MemoryAccessEffect {
  tree reference = nullptr;
  location_t access_location = UNKNOWN_LOCATION;
  AccessGuarantee access = AccessGuarantee::none;
};

// An entry-memory pointer is loaded and accessed along a bounded straight-line
// body prefix. Later ownership effects are deliberately not inferred here.
std::vector<MemoryAccessEffect> memory_access_effects(const gimple *call);

struct BorrowCondition {
  tree selector = nullptr;
  bool when_zero = false;
  location_t guard_location = UNKNOWN_LOCATION;
};

// Exact loaded-body theorem for a void conditional deallocator. The caller
// must prove this selector relation; a mixed fate alone never grants borrow.
bool conditional_borrow(const gimple *call, unsigned int argument_index,
                        BorrowCondition *condition);

// Emit one compact machine-independent summary through GCC's dump stream.
void dump_statistics();

const char *fate_set_name(unsigned int fates);
const char *access_guarantee_name(AccessGuarantee access);
bool access_is_live_requirement(AccessGuarantee access);
bool access_is_conditional(AccessGuarantee access);
const char *proof_origin_name(ProofOrigin origin);

}  // namespace ownership_summary
}  // namespace static_analyzer

#endif
