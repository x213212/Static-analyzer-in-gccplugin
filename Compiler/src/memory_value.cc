#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "tree-ssa-alias.h"
#include "dumpfile.h"
#include "gimple-pretty-print.h"

#include "static_analyzer/memory_value.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <set>

namespace static_analyzer {
namespace {

struct MemoryValueCacheKey {
  const gimple *load_statement;
  unsigned int max_depth;
  unsigned int max_states;

  bool operator<(const MemoryValueCacheKey &other) const {
    if (load_statement != other.load_statement)
      return std::less<const gimple *>()(load_statement,
                                         other.load_statement);
    if (max_depth != other.max_depth)
      return max_depth < other.max_depth;
    return max_states < other.max_states;
  }
};

std::map<MemoryValueCacheKey, MemoryValueResult> function_cache;
struct ReferenceCacheKey {
  tree reference;
  tree vuse;
  unsigned int max_depth;
  unsigned int max_states;
  bool integral = false;

  bool operator<(const ReferenceCacheKey &other) const {
    if (reference != other.reference)
      return std::less<tree>()(reference, other.reference);
    if (vuse != other.vuse)
      return std::less<tree>()(vuse, other.vuse);
    if (max_depth != other.max_depth)
      return max_depth < other.max_depth;
    if (max_states != other.max_states)
      return max_states < other.max_states;
    return integral < other.integral;
  }
};
std::map<ReferenceCacheKey, MemoryValueResult> reference_cache;
memory_value::Statistics function_cache_statistics;

bool pointer_type(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
         TREE_TYPE(value) != NULL_TREE && POINTER_TYPE_P(TREE_TYPE(value));
}

bool integral_type(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
         TREE_TYPE(value) != NULL_TREE && INTEGRAL_TYPE_P(TREE_TYPE(value));
}

bool same_integral_representation(tree left, tree right) {
  return integral_type(left) && integral_type(right) &&
         TYPE_PRECISION(TREE_TYPE(left)) == TYPE_PRECISION(TREE_TYPE(right)) &&
         TYPE_MODE(TREE_TYPE(left)) == TYPE_MODE(TREE_TYPE(right)) &&
         TYPE_UNSIGNED(TREE_TYPE(left)) == TYPE_UNSIGNED(TREE_TYPE(right));
}

bool ordinary_integral_reference(tree reference) {
  if (!integral_type(reference))
    return false;
  // GCC's reverse-storage query is shallow. Audit the complete storage spine
  // on both sides of MUST_KILL; equal byte ranges alone do not prove a scalar
  // value across bitfields, atomic storage, or representation barriers.
  for (unsigned int depth = 0; depth < 24; ++depth) {
    if (reference == NULL_TREE || reference == error_mark_node ||
        TREE_TYPE(reference) == NULL_TREE)
      return false;
    tree type = TREE_TYPE(reference);
    if (TREE_THIS_VOLATILE(reference) || TYPE_VOLATILE(type) ||
        TYPE_ATOMIC(type) ||
        (AGGREGATE_TYPE_P(type) && TYPE_REVERSE_STORAGE_ORDER(type)) ||
        reverse_storage_order_for_component_p(reference))
      return false;
    switch (TREE_CODE(reference)) {
      case VAR_DECL:
      case PARM_DECL:
      case RESULT_DECL:
        return true;
      case COMPONENT_REF: {
        tree field = TREE_OPERAND(reference, 1);
        if (field == NULL_TREE || TREE_CODE(field) != FIELD_DECL ||
            DECL_BIT_FIELD(field) || TREE_THIS_VOLATILE(field) ||
            TYPE_VOLATILE(TREE_TYPE(field)) || TYPE_ATOMIC(TREE_TYPE(field)))
          return false;
        reference = TREE_OPERAND(reference, 0);
        break;
      }
      case ARRAY_REF:
        reference = TREE_OPERAND(reference, 0);
        break;
      case MEM_REF: {
        tree address = TREE_OPERAND(reference, 0);
        if (!pointer_type(address))
          return false;
        tree pointee = TREE_TYPE(TREE_TYPE(address));
        if (TYPE_VOLATILE(pointee) || TYPE_ATOMIC(pointee))
          return false;
        if (TREE_CODE(address) == SSA_NAME)
          return true;
        if (TREE_CODE(address) != ADDR_EXPR)
          return false;
        reference = TREE_OPERAND(address, 0);
        break;
      }
      default:
        return false;
    }
  }
  return false;
}

bool null_pointer_constant(tree value) {
  return value != NULL_TREE && TREE_CODE(value) == INTEGER_CST &&
         integer_zerop(value);
}

bool direct_pointer_rhs(const gimple *statement, tree *value) {
  if (!is_gimple_assign(statement))
    return false;

  // A normal pointer store is a single RHS in GIMPLE.  Permit a conversion as
  // well, since pointer-preserving casts can occasionally remain on a store.
  if (!gimple_assign_single_p(statement) &&
      !gimple_assign_cast_p(statement))
    return false;

  tree rhs = gimple_assign_rhs1(statement);
  if (!pointer_type(rhs) && !null_pointer_constant(rhs))
    return false;

  *value = rhs;
  return true;
}

MemoryAddressMatch address_match(gimple *statement, ao_ref *load_ref) {
  tree lhs = gimple_get_lhs(statement);
  if (lhs == NULL_TREE || TREE_CODE(lhs) == SSA_NAME)
    return MemoryAddressMatch::none;

  // stmt_kills_ref_p is GCC's must-overwrite query.  Unlike
  // stmt_may_clobber_ref_p, success proves that the store covers the queried
  // reference; a conservative may-alias result alone is not enough.
  if (stmt_kills_ref_p(statement, load_ref))
    return MemoryAddressMatch::must_kill;

  // GCC considers these reference expressions equal as addresses.  Address
  // equality alone says nothing about access width, so this is useful dump
  // evidence but is not sufficient to prove a complete overwrite.
  if (load_ref->ref != NULL_TREE &&
      operand_equal_p(lhs, load_ref->ref,
                      OEP_ADDRESS_OF | OEP_MATCH_SIDE_EFFECTS))
    return MemoryAddressMatch::exact_reference;

  return MemoryAddressMatch::may_overlap;
}

bool same_definition(const ReachingMemoryDefinition &left,
                     const ReachingMemoryDefinition &right) {
  return left.statement == right.statement && left.kind == right.kind &&
         left.address_match == right.address_match &&
         left.memory_reference == right.memory_reference &&
         left.stored_value == right.stored_value;
}

void append_definition(MemoryValueResult *result,
                       const ReachingMemoryDefinition &definition,
                       unsigned int max_definitions) {
  const auto existing =
      std::find_if(result->definitions.begin(), result->definitions.end(),
                   [&definition](const ReachingMemoryDefinition &candidate) {
                     return same_definition(candidate, definition);
                   });
  if (existing != result->definitions.end())
    return;

  if (result->definitions.size() >= max_definitions) {
    result->state_exhausted = true;
    return;
  }
  result->definitions.push_back(definition);
}

ReachingMemoryDefinition classify_definition(ao_ref *load_ref, tree vdef) {
  ReachingMemoryDefinition definition;
  if (vdef == NULL_TREE || TREE_CODE(vdef) != SSA_NAME)
    return definition;

  gimple *statement = SSA_NAME_DEF_STMT(vdef);
  definition.statement = statement;
  if (statement == nullptr)
    return definition;

  if (gimple_code(statement) == GIMPLE_ASM) {
    definition.kind = ReachingDefinitionKind::asm_clobber;
    return definition;
  }

  if (is_gimple_call(statement)) {
    definition.kind = ReachingDefinitionKind::call_clobber;
    return definition;
  }

  if (gimple_clobber_p(statement)) {
    definition.kind = ReachingDefinitionKind::explicit_clobber;
    definition.memory_reference = gimple_get_lhs(statement);
    definition.address_match = address_match(statement, load_ref);
    return definition;
  }

  if (!is_gimple_assign(statement) || !gimple_store_p(statement)) {
    definition.kind = ReachingDefinitionKind::other_clobber;
    return definition;
  }

  definition.memory_reference = gimple_assign_lhs(statement);
  definition.address_match = address_match(statement, load_ref);

  tree stored_value = NULL_TREE;
  if (pointer_type(definition.memory_reference) &&
      direct_pointer_rhs(statement, &stored_value)) {
    definition.kind = ReachingDefinitionKind::pointer_store;
    definition.stored_value = stored_value;
  } else if (ordinary_integral_reference(load_ref->ref) &&
             ordinary_integral_reference(definition.memory_reference) &&
             !gimple_has_volatile_ops(statement) &&
             gimple_assign_single_p(statement) &&
             same_integral_representation(load_ref->ref,
                                           definition.memory_reference) &&
             same_integral_representation(load_ref->ref,
                                           gimple_assign_rhs1(statement)) &&
             (TREE_CODE(gimple_assign_rhs1(statement)) == SSA_NAME ||
              TREE_CODE(gimple_assign_rhs1(statement)) == INTEGER_CST)) {
    definition.kind = ReachingDefinitionKind::integral_store;
    definition.stored_value = gimple_assign_rhs1(statement);
  } else {
    // Aggregate stores, memset/memcpy-like effects, and stores whose RHS is
    // not representable by one pointer tree do reach the load, but their
    // pointer value cannot be recovered here.
    definition.kind = ReachingDefinitionKind::imprecise_store;
  }

  return definition;
}

struct CollectionContext {
  MemoryValueResult *result;
  unsigned int max_definitions;
};

bool collect_reaching_definition(ao_ref *load_ref, tree vdef, void *data) {
  CollectionContext *context = static_cast<CollectionContext *>(data);
  append_definition(context->result,
                    classify_definition(load_ref, vdef),
                    context->max_definitions);

  gimple *statement =
      vdef != NULL_TREE && TREE_CODE(vdef) == SSA_NAME
          ? SSA_NAME_DEF_STMT(vdef)
          : nullptr;

  // walk_aliased_vdefs calls us for MAY-clobbers.  A may-clobber is a possible
  // source but, on executions where it does not overlap, an older definition
  // can still reach the load.  Record it and continue.  Stop a branch only at
  // GCC's stronger must-overwrite proof.
  return statement != nullptr && stmt_kills_ref_p(statement, load_ref);
}

struct AuditContext {
  ao_ref *load_ref;
  const MemoryWalkLimits *limits;
  MemoryValueResult *result;
  std::set<tree> active;
};

// Audit the virtual SSA graph before invoking GCC's recursive alias walker.
// This supplies a hard per-path depth cap in addition to the walker's state
// cap and its own visited bitmap.
bool audit_vuse(tree vuse, unsigned int depth, AuditContext *context) {
  if (vuse == NULL_TREE || TREE_CODE(vuse) != SSA_NAME) {
    context->result->missing_virtual_operand = true;
    return false;
  }

  if (depth > context->limits->max_depth) {
    context->result->depth_exhausted = true;
    return false;
  }

  if (context->active.find(vuse) != context->active.end()) {
    context->result->cycle_detected = true;
    return true;
  }

  if (context->result->visited_states >= context->limits->max_states) {
    context->result->state_exhausted = true;
    return false;
  }
  ++context->result->visited_states;
  context->active.insert(vuse);

  bool complete_walk = true;
  gimple *definition = SSA_NAME_DEF_STMT(vuse);
  if (definition == nullptr) {
    context->result->missing_virtual_operand = true;
    complete_walk = false;
  } else if (gimple_nop_p(definition)) {
    context->result->function_entry_reached = true;
  } else if (gimple_code(definition) == GIMPLE_PHI) {
    const gphi *phi = as_a<gphi *>(definition);
    const unsigned int argument_count = gimple_phi_num_args(phi);
    if (argument_count == 0) {
      context->result->missing_virtual_operand = true;
      complete_walk = false;
    }
    for (unsigned int index = 0;
         index < argument_count && complete_walk; ++index)
      complete_walk =
          audit_vuse(gimple_phi_arg_def(phi, index), depth + 1, context);
  } else {
    const bool may_clobber =
        stmt_may_clobber_ref_p_1(definition, context->load_ref, true);
    const bool must_kill =
        may_clobber && stmt_kills_ref_p(definition, context->load_ref);
    if (!must_kill)
      complete_walk =
          audit_vuse(gimple_vuse(definition), depth + 1, context);
  }

  context->active.erase(vuse);
  return complete_walk;
}

bool must_quality(MemoryAddressMatch match) {
  // Only GCC's must-overwrite predicate proves that all bytes read by the
  // load were defined by this store.  OEP_ADDRESS_OF deliberately compares
  // addresses rather than access extents.
  return match == MemoryAddressMatch::must_kill;
}

void finish_status(MemoryValueResult *result) {
  if (result->depth_exhausted || result->state_exhausted ||
      result->cycle_detected || result->missing_virtual_operand ||
      result->function_entry_reached) {
    result->status = MemoryValueStatus::incomplete;
    return;
  }

  if (result->definitions.size() != 1) {
    result->status = result->definitions.empty()
                         ? MemoryValueStatus::incomplete
                         : MemoryValueStatus::ambiguous;
    return;
  }

  const ReachingMemoryDefinition &definition = result->definitions.front();
  if ((definition.kind == ReachingDefinitionKind::pointer_store ||
       definition.kind == ReachingDefinitionKind::integral_store) &&
      definition.stored_value != NULL_TREE &&
      must_quality(definition.address_match))
    result->status = MemoryValueStatus::complete;
  else
    result->status = MemoryValueStatus::ambiguous;
}

}  // namespace

tree MemoryValueResult::unique_stored_value() const {
  if (status != MemoryValueStatus::complete || definitions.size() != 1)
    return NULL_TREE;
  return definitions.front().stored_value;
}

namespace {

MemoryValueResult evaluate_reference(tree reference, tree vuse,
                                     const MemoryWalkLimits &limits,
                                     bool integral = false) {
  MemoryValueResult result;
  if (integral)
    result.status = MemoryValueStatus::not_an_integral_reference;
  result.load_reference = reference;
  if ((integral ? !ordinary_integral_reference(reference)
                : !pointer_type(reference)) ||
      TREE_CODE(reference) == SSA_NAME ||
      TREE_CODE(reference) == INTEGER_CST || TREE_CODE(reference) == ADDR_EXPR)
    return result;

  result.volatile_load = TREE_THIS_VOLATILE(reference);
  if (result.volatile_load) {
    result.status = MemoryValueStatus::incomplete;
    return result;
  }

  if (vuse == NULL_TREE || TREE_CODE(vuse) != SSA_NAME) {
    result.status = MemoryValueStatus::incomplete;
    result.missing_virtual_operand = true;
    return result;
  }

  if (limits.max_depth == 0 || limits.max_states == 0) {
    result.status = MemoryValueStatus::incomplete;
    result.depth_exhausted = limits.max_depth == 0;
    result.state_exhausted = limits.max_states == 0;
    return result;
  }

  ao_ref load_ref;
  ao_ref_init(&load_ref, result.load_reference);
  // Force decomposition once so an invalid/unsupported reference is rejected
  // before either traversal.  A null base means GCC cannot describe it.
  if (ao_ref_base(&load_ref) == NULL_TREE) {
    result.status = MemoryValueStatus::incomplete;
    return result;
  }

  AuditContext audit{&load_ref, &limits, &result, {}};
  const bool audit_finished = audit_vuse(vuse, 1, &audit);
  if (!audit_finished || result.depth_exhausted || result.state_exhausted) {
    result.status = MemoryValueStatus::incomplete;
    return result;
  }

  CollectionContext collection{&result, limits.max_states};
  bool entry_reached = false;
  const unsigned int walker_limit =
      limits.max_states == std::numeric_limits<unsigned int>::max()
          ? limits.max_states
          : limits.max_states + 1;
  const int walked =
      walk_aliased_vdefs(&load_ref, vuse, collect_reaching_definition,
                         &collection, nullptr, &entry_reached, walker_limit);
  result.function_entry_reached =
      result.function_entry_reached || entry_reached;
  if (walked < 0) {
    result.walked_vdefs = limits.max_states;
    result.state_exhausted = true;
  } else {
    result.walked_vdefs = static_cast<unsigned int>(walked);
  }

  finish_status(&result);
  return result;
}

MemoryValueResult evaluate_pointer_load(const gimple *load_statement,
                                        const MemoryWalkLimits &limits) {
  MemoryValueResult result;
  if (load_statement == nullptr || !is_gimple_assign(load_statement) ||
      !gimple_assign_load_p(load_statement) ||
      !pointer_type(gimple_assign_lhs(load_statement)))
    return result;
  if (gimple_has_volatile_ops(load_statement)) {
    result.loaded_value = gimple_assign_lhs(load_statement);
    result.load_reference = gimple_assign_rhs1(load_statement);
    result.volatile_load = true;
    result.status = MemoryValueStatus::incomplete;
    return result;
  }
  result = evaluate_reference(gimple_assign_rhs1(load_statement),
                                      gimple_vuse(load_statement), limits);
  result.loaded_value = gimple_assign_lhs(load_statement);
  return result;
}

}  // namespace

MemoryValueResult resolve_pointer_load(const gimple *load_statement,
                                       const MemoryWalkLimits &limits) {
  ++function_cache_statistics.requests;
  const MemoryValueCacheKey key{load_statement, limits.max_depth,
                                limits.max_states};
  const auto cached = function_cache.find(key);
  if (cached != function_cache.end()) {
    ++function_cache_statistics.hits;
    return cached->second;
  }

  ++function_cache_statistics.evaluations;
  const MemoryValueResult result =
      evaluate_pointer_load(load_statement, limits);
  function_cache.insert(std::make_pair(key, result));
  return result;
}

MemoryValueResult resolve_pointer_reference(tree reference, tree vuse,
                                           const MemoryWalkLimits &limits) {
  ++function_cache_statistics.requests;
  const ReferenceCacheKey key{reference, vuse, limits.max_depth,
                              limits.max_states};
  const auto cached = reference_cache.find(key);
  if (cached != reference_cache.end()) {
    ++function_cache_statistics.hits;
    return cached->second;
  }
  ++function_cache_statistics.evaluations;
  const MemoryValueResult result =
      evaluate_reference(reference, vuse, limits);
  reference_cache.insert(std::make_pair(key, result));
  return result;
}

MemoryValueResult resolve_integral_reference(tree reference, tree vuse,
                                             const MemoryWalkLimits &limits) {
  ++function_cache_statistics.requests;
  const ReferenceCacheKey key{reference, vuse, limits.max_depth,
                              limits.max_states, true};
  const auto cached = reference_cache.find(key);
  if (cached != reference_cache.end()) {
    ++function_cache_statistics.hits;
    return cached->second;
  }
  ++function_cache_statistics.evaluations;
  MemoryValueResult result = evaluate_reference(reference, vuse, limits, true);
  // A full-width pointer store over an integer cell can be a MUST_KILL of
  // the bytes, but its representation is not an exact integer value.
  if (result.status == MemoryValueStatus::complete &&
      result.definitions.front().kind != ReachingDefinitionKind::integral_store)
    result.status = MemoryValueStatus::ambiguous;
  reference_cache.insert(std::make_pair(key, result));
  return result;
}

namespace memory_value {

void reset_function_cache() {
  function_cache.clear();
  reference_cache.clear();
  function_cache_statistics = Statistics{};
}

Statistics statistics() { return function_cache_statistics; }

void dump_statistics() {
  if (dump_file == nullptr)
    return;
  dump_printf(MSG_NOTE,
              "memory-value-cache summary: requests=%lu evaluations=%lu "
              "hits=%lu entries=%u\n",
              function_cache_statistics.requests,
              function_cache_statistics.evaluations,
              function_cache_statistics.hits,
              static_cast<unsigned int>(function_cache.size() +
                                        reference_cache.size()));
}

}  // namespace memory_value

void dump_memory_value_result(const MemoryValueResult &result) {
  if (dump_file == nullptr)
    return;

  dump_printf(MSG_NOTE,
              "memory-value: status=%s definitions=%u visited-vssa=%u "
              "walked-vdefs=%u entry=%s cycle=%s depth-limit=%s "
              "state-limit=%s volatile=%s\n",
              memory_value_status_name(result.status),
              static_cast<unsigned int>(result.definitions.size()),
              result.visited_states, result.walked_vdefs,
              result.function_entry_reached ? "yes" : "no",
              result.cycle_detected ? "yes" : "no",
              result.depth_exhausted ? "yes" : "no",
              result.state_exhausted ? "yes" : "no",
              result.volatile_load ? "yes" : "no");

  if (!(dump_flags & TDF_DETAILS))
    return;

  if (result.load_reference != NULL_TREE) {
    dump_printf(MSG_NOTE, "  load-reference: ");
    dump_generic_expr(MSG_NOTE, TDF_SLIM, result.load_reference);
    dump_printf(MSG_NOTE, "\n");
  }
  for (const ReachingMemoryDefinition &definition : result.definitions) {
    dump_printf(MSG_NOTE, "  reaching-definition: kind=%s match=%s",
                reaching_definition_kind_name(definition.kind),
                memory_address_match_name(definition.address_match));
    if (definition.stored_value != NULL_TREE) {
      dump_printf(MSG_NOTE, " stored-value=");
      dump_generic_expr(MSG_NOTE, TDF_SLIM, definition.stored_value);
    }
    dump_printf(MSG_NOTE, "\n");
    if (definition.statement != nullptr)
      dump_gimple_stmt(MSG_NOTE, TDF_SLIM,
                       const_cast<gimple *>(definition.statement), 0);
  }
}

const char *memory_value_status_name(MemoryValueStatus status) {
  switch (status) {
    case MemoryValueStatus::not_a_pointer_load:
      return "not-a-pointer-load";
    case MemoryValueStatus::not_an_integral_reference:
      return "not-an-integral-reference";
    case MemoryValueStatus::complete:
      return "complete";
    case MemoryValueStatus::ambiguous:
      return "ambiguous";
    case MemoryValueStatus::incomplete:
      return "incomplete";
  }
  return "incomplete";
}

const char *reaching_definition_kind_name(ReachingDefinitionKind kind) {
  switch (kind) {
    case ReachingDefinitionKind::pointer_store:
      return "pointer-store";
    case ReachingDefinitionKind::integral_store:
      return "integral-store";
    case ReachingDefinitionKind::imprecise_store:
      return "imprecise-store";
    case ReachingDefinitionKind::call_clobber:
      return "call-clobber";
    case ReachingDefinitionKind::asm_clobber:
      return "asm-clobber";
    case ReachingDefinitionKind::explicit_clobber:
      return "explicit-clobber";
    case ReachingDefinitionKind::other_clobber:
      return "other-clobber";
  }
  return "other-clobber";
}

const char *memory_address_match_name(MemoryAddressMatch match) {
  switch (match) {
    case MemoryAddressMatch::none:
      return "none";
    case MemoryAddressMatch::exact_reference:
      return "exact-reference";
    case MemoryAddressMatch::must_kill:
      return "must-kill";
    case MemoryAddressMatch::may_overlap:
      return "may-overlap";
  }
  return "none";
}

}  // namespace static_analyzer
