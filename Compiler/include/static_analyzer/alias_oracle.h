#ifndef STATIC_ANALYZER_ALIAS_ORACLE_H
#define STATIC_ANALYZER_ALIAS_ORACLE_H

#include "coretypes.h"

namespace static_analyzer {
namespace alias {

enum class Relation {
  same_value,
  must_alias,
  may_alias,
  no_alias,
  unknown
};

struct Evidence {
  Relation relation = Relation::unknown;
  bool gcc_may_alias = true;
  bool points_to_available = false;
  bool points_to_precise = false;
  bool provenance_complete = false;
};

// GCC's tree-ssa alias oracle remains the source of truth for may/no-alias.
// Provenance expansion adds the stronger distinction needed by ownership
// diagnostics: GCC may-alias is never promoted to must-alias.
Evidence query(tree lhs, tree rhs);

// Conservative traversal predicate.  Unknown means "keep exploring".
bool may_alias(tree lhs, tree rhs);

// Reporting predicate.  True only when every dynamically expanded GIMPLE
// origin identifies the same non-derived pointer object.
bool proven_same_object(tree lhs, tree rhs);

// Reporting predicate for lifetime uses.  Unlike proven_same_object(), an
// interior pointer derived with POINTER_PLUS_EXPR still belongs to the same
// allocation and is therefore relevant to use-after-free checks.
bool proven_same_allocation(tree lhs, tree rhs);

// Existential provenance predicate for lifetime uses.  This is deliberately
// weaker than proven_same_allocation(): it succeeds when at least one SSA
// origin of CANDIDATE is the allocation identified by ALLOCATION.  It does
// not establish that CANDIDATE always denotes that allocation and therefore
// must never be used to classify free/delete calls as definite releases.
bool may_share_allocation_origin(tree candidate, tree allocation);

// Drop memoized GCC alias/provenance results at a function boundary.  The
// cache is deliberately function-local: SSA names and their points-to facts
// are interpreted in the active cfun, and no relation is reused after that
// context changes.
void reset_function_cache();

void reset_statistics();
void dump_statistics();
const char *relation_name(Relation relation);

}  // namespace alias
}  // namespace static_analyzer

#endif
