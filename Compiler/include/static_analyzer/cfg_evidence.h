#ifndef STATIC_ANALYZER_CFG_EVIDENCE_H
#define STATIC_ANALYZER_CFG_EVIDENCE_H

#include "coretypes.h"

namespace static_analyzer {

// Mutually exclusive branch-local frees must not become a double-free merely
// because both pointer arguments alias the same allocation.
bool statements_may_execute_in_sequence(const gimple *first,
                                        const gimple *second);

}  // namespace static_analyzer

#endif
