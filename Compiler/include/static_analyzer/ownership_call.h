#ifndef STATIC_ANALYZER_OWNERSHIP_CALL_H
#define STATIC_ANALYZER_OWNERSHIP_CALL_H

#include "coretypes.h"

namespace static_analyzer {

// Select the ownership classification contract for the current translation
// unit.  Proof-only rejects allocator/deallocator spelling conventions and
// plain no-alias "malloc" attributes that do not identify a compatible
// release function.
void set_ownership_proof_only(bool enabled);

// Classify calls that create or release heap ownership.  The implementation
// uses GCC's FUNCTION_DECL metadata for C++ operators.  In particular, an
// arbitrary function whose spelling happens to contain "operator new" is not
// enough to create an allocation site.
bool ownership_allocation_call_p(const gimple *statement);
// True when a call has fresh/no-alias metadata or a historical xmalloc-like
// spelling, but proof-only cannot establish a compatible release contract.
// It is report metadata only and must never create an allocation site.
bool ownership_unproved_allocation_call_p(const gimple *statement);
bool ownership_deallocation_call_p(const gimple *statement);
// Exact GCC realloc(NULL, positive-constant-size) has malloc semantics.  NULL
// may be restored through bounded representation-preserving SSA copies and
// complete MUST_KILL virtual-SSA stores, never PHIs or may-alias evidence.
// This call is classified as allocation, not as an old-object release.
bool ownership_null_reallocation_call_p(const gimple *statement);
bool ownership_reallocation_call_p(const gimple *statement);

}  // namespace static_analyzer

#endif
