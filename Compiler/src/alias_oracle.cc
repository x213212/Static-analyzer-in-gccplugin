#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "ssa.h"
#include "tree-ssa-alias.h"
#include "dumpfile.h"

#include "static_analyzer/alias_oracle.h"
#include "static_analyzer/gimple_value.h"

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <utility>

namespace static_analyzer {
namespace alias {
namespace {

struct Statistics {
  unsigned long same_value = 0;
  unsigned long must_alias = 0;
  unsigned long may_alias = 0;
  unsigned long no_alias = 0;
  unsigned long unknown = 0;
  unsigned long cache_requests = 0;
  unsigned long cache_evaluations = 0;
  unsigned long cache_hits = 0;
  unsigned long cache_cap_skips = 0;
};

Statistics statistics;

constexpr unsigned int provenance_depth = 64;

struct QueryKey {
  tree function = NULL_TREE;
  tree first = NULL_TREE;
  tree second = NULL_TREE;
  unsigned int max_provenance_depth = provenance_depth;
};

struct QueryKeyHash {
  std::size_t operator()(const QueryKey &key) const {
    std::size_t result = std::hash<tree>()(key.function);
    const auto combine = [&result](std::size_t value) {
      result ^= value + static_cast<std::size_t>(0x9e3779b9U) +
                (result << 6) + (result >> 2);
    };
    combine(std::hash<tree>()(key.first));
    combine(std::hash<tree>()(key.second));
    combine(std::hash<unsigned int>()(key.max_provenance_depth));
    return result;
  }
};

struct QueryKeyEqual {
  bool operator()(const QueryKey &lhs, const QueryKey &rhs) const {
    return lhs.function == rhs.function && lhs.first == rhs.first &&
           lhs.second == rhs.second &&
           lhs.max_provenance_depth == rhs.max_provenance_depth;
  }
};

struct CachedQuery {
  Evidence evidence;
};

std::unordered_map<QueryKey, CachedQuery, QueryKeyHash, QueryKeyEqual>
    query_cache;

constexpr unsigned int max_query_cache_entries = 8192;

void remember_query(const QueryKey &key, const Evidence &evidence) {
  if (query_cache.size() >= max_query_cache_entries) {
    ++statistics.cache_cap_skips;
    return;
  }
  CachedQuery entry;
  entry.evidence = evidence;
  query_cache.emplace(key, entry);
}

QueryKey query_key(tree lhs, tree rhs, bool *reversed) {
  const std::less<tree> less;
  *reversed = less(rhs, lhs);
  QueryKey key;
  key.function = cfun != nullptr ? cfun->decl : current_function_decl;
  key.first = *reversed ? rhs : lhs;
  key.second = *reversed ? lhs : rhs;
  key.max_provenance_depth = provenance_depth;
  return key;
}

bool pointer_expression(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
         CODE_CONTAINS_STRUCT(TREE_CODE(value), TS_TYPED) &&
         TREE_TYPE(value) != NULL_TREE &&
         POINTER_TYPE_P(TREE_TYPE(value));
}

const ptr_info_def *pointer_info(tree value) {
  if (value == NULL_TREE || TREE_CODE(value) != SSA_NAME ||
      !pointer_expression(value))
    return nullptr;
  return SSA_NAME_PTR_INFO(value);
}

bool precise(const pt_solution &solution) {
  return !solution.anything && !solution.nonlocal && !solution.escaped &&
         !solution.ipa_escaped;
}

bool same_origin_identity(const ValueOrigin &lhs, const ValueOrigin &rhs) {
  if (lhs.kind != rhs.kind)
    return false;
  if (lhs.kind == OriginKind::allocation_call ||
      lhs.kind == OriginKind::reallocation_call ||
      lhs.kind == OriginKind::call_result)
    return lhs.defining_statement != nullptr &&
           lhs.defining_statement == rhs.defining_statement;
  return lhs.value != NULL_TREE && lhs.value == rhs.value;
}

bool same_allocation_identity(const ValueOrigin &lhs,
                              const ValueOrigin &rhs) {
  if (lhs.kind != OriginKind::allocation_call &&
      lhs.kind != OriginKind::reallocation_call)
    return false;
  if (rhs.kind != OriginKind::allocation_call &&
      rhs.kind != OriginKind::reallocation_call)
    return false;
  return lhs.defining_statement != nullptr &&
         lhs.defining_statement == rhs.defining_statement;
}

bool expansion_is_one_allocation(const ValueExpansion &expansion,
                                 const ValueOrigin **identity) {
  if (!expansion.complete || expansion.origins.empty())
    return false;
  const ValueOrigin &first = expansion.origins.front();
  if (first.kind != OriginKind::allocation_call &&
      first.kind != OriginKind::reallocation_call)
    return false;
  for (const ValueOrigin &origin : expansion.origins)
    if (!same_allocation_identity(first, origin))
      return false;
  *identity = &first;
  return true;
}

bool contains_derived_pointer(const ValueExpansion &expansion) {
  for (const ValueOrigin &origin : expansion.origins)
    if (origin.derived_pointer)
      return true;
  return false;
}

bool single_identical_origin(const ValueExpansion &lhs,
                             const ValueExpansion &rhs) {
  if (!lhs.complete || !rhs.complete || lhs.origins.size() != 1 ||
      rhs.origins.size() != 1 || contains_derived_pointer(lhs) ||
      contains_derived_pointer(rhs))
    return false;
  return same_origin_identity(lhs.origins.front(), rhs.origins.front());
}

bool origins_intersect(const ValueExpansion &lhs, const ValueExpansion &rhs) {
  for (const ValueOrigin &left_origin : lhs.origins)
    for (const ValueOrigin &right_origin : rhs.origins)
      if (same_origin_identity(left_origin, right_origin))
        return true;
  return false;
}

void count(Relation relation) {
  switch (relation) {
    case Relation::same_value:
      ++statistics.same_value;
      break;
    case Relation::must_alias:
      ++statistics.must_alias;
      break;
    case Relation::may_alias:
      ++statistics.may_alias;
      break;
    case Relation::no_alias:
      ++statistics.no_alias;
      break;
    case Relation::unknown:
      ++statistics.unknown;
      break;
  }
}

void dump_expression(const char *label, tree value) {
  dump_printf(MSG_NOTE, "  %s: ", label);
  dump_generic_expr(MSG_NOTE, TDF_SLIM, value);
  dump_printf(MSG_NOTE, "\n");
}

void dump_expansion(const char *label, const ValueExpansion &expansion) {
  dump_printf(MSG_NOTE, "  %s-provenance: complete=%s origins=%u\n", label,
              expansion.complete ? "yes" : "no",
              static_cast<unsigned int>(expansion.origins.size()));
  for (const ValueOrigin &origin : expansion.origins) {
    dump_printf(MSG_NOTE, "    %s%s: ", origin_kind_name(origin.kind),
                origin.derived_pointer ? " (derived)" : "");
    if (origin.value != NULL_TREE)
      dump_generic_expr(MSG_NOTE, TDF_SLIM, origin.value);
    else
      dump_printf(MSG_NOTE, "<null-tree>");
    dump_printf(MSG_NOTE, "\n");
  }
}

void dump_evidence(tree lhs, tree rhs, const Evidence &evidence,
                   const ValueExpansion &lhs_expansion,
                   const ValueExpansion &rhs_expansion, bool cache_hit) {
  if (dump_file == nullptr || !(dump_flags & TDF_DETAILS))
    return;

  dump_printf(MSG_NOTE,
              "alias-query: relation=%s gcc-may-alias=%s "
              "points-to=%s precise=%s cache=%s\n",
              relation_name(evidence.relation),
              evidence.gcc_may_alias ? "yes" : "no",
              evidence.points_to_available ? "available" : "missing",
              evidence.points_to_precise ? "yes" : "no",
              cache_hit ? "hit" : "miss");
  dump_expression("lhs", lhs);
  dump_expression("rhs", rhs);

  const ptr_info_def *lhs_info = pointer_info(lhs);
  const ptr_info_def *rhs_info = pointer_info(rhs);
  if (lhs_info != nullptr) {
    dump_printf(MSG_NOTE, "  lhs-points-to:");
    dump_points_to_solution(dump_file,
                            const_cast<pt_solution *>(&lhs_info->pt));
    dump_printf(MSG_NOTE, "\n");
  }
  if (rhs_info != nullptr) {
    dump_printf(MSG_NOTE, "  rhs-points-to:");
    dump_points_to_solution(dump_file,
                            const_cast<pt_solution *>(&rhs_info->pt));
    dump_printf(MSG_NOTE, "\n");
  }
  dump_expansion("lhs", lhs_expansion);
  dump_expansion("rhs", rhs_expansion);
}

}  // namespace

Evidence query(tree lhs, tree rhs) {
  ++statistics.cache_requests;
  bool reversed = false;
  const QueryKey key = query_key(lhs, rhs, &reversed);
  // Internal A/B regression knob only.  Production builds never define this
  // macro; keeping the miss-only path makes cache overhead measurements
  // reproducible without changing the relation algorithm or plugin options.
#ifndef STATIC_ANALYZER_DISABLE_ALIAS_QUERY_CACHE
  const auto cached = query_cache.find(key);
  if (cached != query_cache.end()) {
    ++statistics.cache_hits;
    const CachedQuery &entry = cached->second;
    count(entry.evidence.relation);
    // Normal cache hits need only the compact relation evidence.  Reconstruct
    // expansion vectors solely for an explicitly requested detailed dump;
    // otherwise even a cache hit would copy two bounded origin vectors.
    if (dump_file != nullptr && (dump_flags & TDF_DETAILS)) {
      const ValueExpansion lhs_expansion =
          expand_pointer_value(lhs, key.max_provenance_depth);
      const ValueExpansion rhs_expansion =
          expand_pointer_value(rhs, key.max_provenance_depth);
      dump_evidence(lhs, rhs, entry.evidence, lhs_expansion, rhs_expansion,
                    true);
    }
    return entry.evidence;
  }
#endif

  ++statistics.cache_evaluations;
  ValueExpansion first_expansion =
      expand_pointer_value(key.first, key.max_provenance_depth);
  ValueExpansion second_expansion =
      expand_pointer_value(key.second, key.max_provenance_depth);
  ValueExpansion &lhs_expansion =
      reversed ? second_expansion : first_expansion;
  ValueExpansion &rhs_expansion =
      reversed ? first_expansion : second_expansion;
  Evidence evidence;
  evidence.provenance_complete =
      lhs_expansion.complete && rhs_expansion.complete;

  if (!pointer_expression(lhs) || !pointer_expression(rhs)) {
    evidence.relation = Relation::unknown;
    count(evidence.relation);
    dump_evidence(lhs, rhs, evidence, lhs_expansion, rhs_expansion, false);
    const Evidence result = evidence;
#ifndef STATIC_ANALYZER_DISABLE_ALIAS_QUERY_CACHE
    remember_query(key, evidence);
#endif
    return result;
  }

  const ptr_info_def *lhs_info = pointer_info(lhs);
  const ptr_info_def *rhs_info = pointer_info(rhs);
  evidence.points_to_available = lhs_info != nullptr && rhs_info != nullptr;
  evidence.points_to_precise =
      evidence.points_to_available && precise(lhs_info->pt) &&
      precise(rhs_info->pt);

  // This is GCC's alias oracle from tree-ssa-alias.c.  A false result is a
  // proof of no alias at this pass point; true is intentionally only "may".
  evidence.gcc_may_alias = ptr_derefs_may_alias_p(lhs, rhs);

  if (!evidence.gcc_may_alias)
    evidence.relation = Relation::no_alias;
  else if (lhs == rhs && !contains_derived_pointer(lhs_expansion))
    evidence.relation = Relation::same_value;
  else if (single_identical_origin(lhs_expansion, rhs_expansion))
    evidence.relation = Relation::must_alias;
  else if (origins_intersect(lhs_expansion, rhs_expansion) ||
           evidence.points_to_precise)
    evidence.relation = Relation::may_alias;
  else
    evidence.relation = Relation::unknown;

  count(evidence.relation);
  dump_evidence(lhs, rhs, evidence, lhs_expansion, rhs_expansion, false);
  const Evidence result = evidence;
#ifndef STATIC_ANALYZER_DISABLE_ALIAS_QUERY_CACHE
  remember_query(key, evidence);
#endif
  return result;
}

bool may_alias(tree lhs, tree rhs) {
  return query(lhs, rhs).relation != Relation::no_alias;
}

bool proven_same_object(tree lhs, tree rhs) {
  const Evidence evidence = query(lhs, rhs);
  return evidence.relation == Relation::same_value ||
         evidence.relation == Relation::must_alias;
}

bool proven_same_allocation(tree lhs, tree rhs) {
  const ValueExpansion lhs_expansion = expand_pointer_value(lhs);
  const ValueExpansion rhs_expansion = expand_pointer_value(rhs);
  const ValueOrigin *lhs_identity = nullptr;
  const ValueOrigin *rhs_identity = nullptr;
  if (!expansion_is_one_allocation(lhs_expansion, &lhs_identity) ||
      !expansion_is_one_allocation(rhs_expansion, &rhs_identity))
    return false;
  return same_allocation_identity(*lhs_identity, *rhs_identity);
}

bool may_share_allocation_origin(tree candidate, tree allocation) {
  if (!pointer_expression(candidate) || !pointer_expression(allocation))
    return false;

  const ValueExpansion candidate_expansion = expand_pointer_value(candidate);
  const ValueExpansion allocation_expansion = expand_pointer_value(allocation);
  const ValueOrigin *allocation_identity = nullptr;
  if (!expansion_is_one_allocation(allocation_expansion,
                                   &allocation_identity))
    return false;

  for (const ValueOrigin &origin : candidate_expansion.origins)
    if (same_allocation_identity(origin, *allocation_identity))
      return true;
  return false;
}

void reset_function_cache() { query_cache.clear(); }

void reset_statistics() {
  statistics = Statistics{};
  reset_function_cache();
}

void dump_statistics() {
  if (dump_file == nullptr)
    return;
  dump_printf(MSG_NOTE,
              "alias-oracle summary: same=%lu must=%lu may=%lu no=%lu "
              "unknown=%lu cache-requests=%lu cache-evaluations=%lu "
              "cache-hits=%lu cache-entries=%u cache-cap-skips=%lu "
              "cache-limit=%u\n",
              statistics.same_value, statistics.must_alias,
              statistics.may_alias, statistics.no_alias,
              statistics.unknown, statistics.cache_requests,
              statistics.cache_evaluations, statistics.cache_hits,
              static_cast<unsigned int>(query_cache.size()),
              statistics.cache_cap_skips, max_query_cache_entries);
}

const char *relation_name(Relation relation) {
  switch (relation) {
    case Relation::same_value:
      return "same-value";
    case Relation::must_alias:
      return "must-alias";
    case Relation::may_alias:
      return "may-alias";
    case Relation::no_alias:
      return "no-alias";
    case Relation::unknown:
      return "unknown";
  }
  return "unknown";
}

}  // namespace alias
}  // namespace static_analyzer
