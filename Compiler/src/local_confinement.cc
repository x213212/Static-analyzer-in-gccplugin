#include "gcc-plugin.h"
#include "bversion.h"
#include "tree.h"
#include "tree-dfa.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "gimple-walk.h"
#include "tree-cfg.h"
#include "cgraph.h"
#include "dumpfile.h"
#include "langhooks.h"

#include "static_analyzer/analysis_budget.h"
#include "static_analyzer/local_confinement.h"
#include "static_analyzer/ownership_call.h"
#include "static_analyzer/gcc_call_effect.h"

#include <map>

namespace static_analyzer {
namespace local_confinement {
namespace {

enum class Shape { scalar, pointers, unknown };
enum class State { active, rejected, confined };
std::map<tree, State> cache;
constexpr unsigned int max_functions = 256;
constexpr unsigned int max_call_depth = 24;
constexpr unsigned int max_type_depth = 24;
constexpr unsigned int max_statements = 32768;

struct Query {
  unsigned int statements = 0;
  bool exhausted = false;
  const char *reason = "unproved";
  bool reject(const char *why) {
    reason = why;
    return false;
  }
  bool charge() {
    if (exhausted)
      return false;
    exhausted = ++statements > max_statements ||
                !analysis_budget::consume(
                    analysis_budget::WorkKind::lifetime_discovery);
    return !exhausted;
  }
};

bool pointer_type(tree type) {
  return type != NULL_TREE && POINTER_TYPE_P(type);
}

bool pointer_value(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
         CODE_CONTAINS_STRUCT(TREE_CODE(value), TS_TYPED) &&
         pointer_type(TREE_TYPE(value));
}

bool aggregate_type(tree type) {
  return type != NULL_TREE && AGGREGATE_TYPE_P(type);
}

Shape shape(tree type, Query *query, unsigned int depth = 0) {
  if (type == NULL_TREE || depth >= max_type_depth || !query->charge())
    return Shape::unknown;
  if (pointer_type(type) || TREE_CODE(type) == REFERENCE_TYPE)
    return Shape::pointers;
  switch (TREE_CODE(type)) {
    case ARRAY_TYPE:
      return shape(TREE_TYPE(type), query, depth + 1);
    case RECORD_TYPE: {
      Shape result = Shape::scalar;
      for (tree field = TYPE_FIELDS(type); field != NULL_TREE;
           field = DECL_CHAIN(field)) {
        if (!query->charge())
          return Shape::unknown;
        if (TREE_CODE(field) != FIELD_DECL)
          continue;
        const Shape part = shape(TREE_TYPE(field), query, depth + 1);
        if (part == Shape::unknown)
          return part;
        if (part == Shape::pointers)
          result = part;
      }
      return result;
    }
    // Union type-punning and raw byte/vector representations are not a
    // contained-pointer theorem. In particular u.bits may encode u.pointer.
    case UNION_TYPE:
    case QUAL_UNION_TYPE:
    case VOID_TYPE:
    case VECTOR_TYPE:
      return Shape::unknown;
    case BOOLEAN_TYPE:
    case INTEGER_TYPE:
    case ENUMERAL_TYPE:
    case REAL_TYPE:
    case COMPLEX_TYPE:
      return Shape::scalar;
    default:
      return Shape::unknown;
  }
}

Shape pointed_shape(tree value, Query *query, unsigned int depth = 0) {
  if (!pointer_value(value) || depth >= max_type_depth || !query->charge())
    return Shape::unknown;
  // An address can have a converted pointer type. The actual addressed
  // object, not a cast's destination type, determines whether it hides bits.
  if (TREE_CODE(value) == ADDR_EXPR) {
    tree object = TREE_OPERAND(value, 0);
    return object != NULL_TREE ? shape(TREE_TYPE(object), query)
                               : Shape::unknown;
  }
  if (TREE_CODE(value) == SSA_NAME) {
    gimple *definition = SSA_NAME_DEF_STMT(value);
    if (definition != nullptr && is_gimple_call(definition) &&
        gcc_call_effect::known_library_returns_argument(as_a<gcall *>(definition), 0))
      return pointed_shape(gimple_call_arg(definition, 0), query, depth + 1);
  }
  return shape(TREE_TYPE(TREE_TYPE(value)), query);
}

bool null_pointer(tree value) {
  return value != NULL_TREE && TREE_CODE(value) == INTEGER_CST &&
         integer_zerop(value);
}

bool literal_pointer(tree value) {
  if (null_pointer(value))
    return true;
  return pointer_value(value) && TREE_CODE(value) == ADDR_EXPR &&
         TREE_CODE(TREE_OPERAND(value, 0)) == STRING_CST;
}

bool automatic_storage(tree reference) {
  tree base = get_base_address(reference);
  return base != NULL_TREE && TREE_CODE(base) == VAR_DECL &&
         auto_var_in_fn_p(base, cfun->decl);
}

bool exact_builtin(gimple *, enum built_in_function);

bool fresh_storage_result(tree value) {
  if (value == NULL_TREE || TREE_CODE(value) != SSA_NAME)
    return false;
  gimple *definition = SSA_NAME_DEF_STMT(value);
  return definition != nullptr && is_gimple_call(definition) &&
         gimple_call_lhs(definition) == value &&
         (exact_builtin(definition, BUILT_IN_MALLOC) ||
          exact_builtin(definition, BUILT_IN_CALLOC) ||
          ownership_null_reallocation_call_p(definition) ||
          exact_builtin(definition, BUILT_IN_ALLOCA) ||
          exact_builtin(definition, BUILT_IN_ALLOCA_WITH_ALIGN));
}

bool compatible_pointer_storage(tree source, tree destination_type,
                                Query *query) {
  if (!pointer_value(source) || !pointer_type(destination_type))
    return false;
  if (literal_pointer(source))
    return true;
  // GCC often keeps calloc/malloc's result as void* when its first use
  // stores into an address-taken typed local. Fresh storage has no imported
  // contained-pointer representation to erase; arbitrary void* inputs do.
  if (fresh_storage_result(source))
    return true;
  const Shape from = pointed_shape(source, query);
  const Shape to = shape(TREE_TYPE(destination_type), query);
  // void* may carry an opaque container, but converting that container to
  // scalar-readable storage must not erase its representation provenance.
  if (TREE_CODE(TREE_TYPE(destination_type)) == VOID_TYPE)
    return true;
  if (to == Shape::scalar)
    return from == Shape::scalar;
  return to == Shape::pointers &&
         (from == Shape::pointers ||
          TREE_CODE(TREE_TYPE(TREE_TYPE(source))) == VOID_TYPE);
}

bool inspect_load(gimple *, tree, tree reference, void *data) {
  auto *query = static_cast<Query *>(data);
  if (!query->charge())
    return true;
  const Shape loaded = shape(TREE_TYPE(reference), query);
  tree base = get_base_address(reference);
  if (loaded == Shape::unknown || base == NULL_TREE)
    return true;
  if (TREE_CODE(base) == MEM_REF || TREE_CODE(base) == INDIRECT_REF) {
    tree address = TREE_OPERAND(base, 0);
    if (!pointer_value(address))
      return true;
    // A scalar read through a pointer-containing or opaque region can be
    // uintptr_t/byte serialization of a contained pointer, without a cast
    // from pointer to integer in GIMPLE. Never permit that publication route.
    if (loaded == Shape::scalar &&
        pointed_shape(address, query) != Shape::scalar)
      return true;
    if (aggregate_type(TREE_TYPE(reference))) {
      tree actual_type = TREE_TYPE(TREE_TYPE(address));
      if (TREE_CODE(address) == ADDR_EXPR)
        actual_type = TREE_TYPE(TREE_OPERAND(address, 0));
      if (actual_type == NULL_TREE ||
          TYPE_MAIN_VARIANT(actual_type) !=
              TYPE_MAIN_VARIANT(TREE_TYPE(reference)))
        return true;
    }
  } else if (TREE_CODE(base) == TARGET_MEM_REF) {
    return true;
  } else if (DECL_P(base) || TREE_CODE(base) == SSA_NAME) {
    const Shape storage = shape(TREE_TYPE(base), query);
    // Even a direct local declaration can be read through a representation
    // expression. Reject scalar observations of pointer-bearing containers;
    // proving individual scalar subfields is a future, separate refinement.
    if (storage == Shape::unknown ||
        (loaded == Shape::scalar && storage != Shape::scalar))
      return true;
  }
  return false;
}

bool assignment_confined(gimple *statement, Query *query) {
  if (gimple_clobber_p(statement))
    return true;
  tree lhs = gimple_assign_lhs(statement);
  tree rhs = gimple_assign_rhs1(statement);
  if (lhs == NULL_TREE || rhs == NULL_TREE)
    return query->reject("unknown-assignment");
  const auto code = gimple_assign_rhs_code(statement);
  if (code == VIEW_CONVERT_EXPR || code == BIT_FIELD_REF)
    return query->reject("representation-conversion");
  const bool pointer_result = pointer_value(lhs);
  const bool aggregate_result = aggregate_type(TREE_TYPE(lhs));
  if ((pointer_result || aggregate_result) &&
      TREE_CODE(lhs) != SSA_NAME && !automatic_storage(lhs))
    return query->reject("nonlocal-pointer-or-aggregate-store");
  if (aggregate_result && shape(TREE_TYPE(lhs), query) == Shape::unknown)
    return query->reject("opaque-aggregate-representation");

  bool pointer_operand = false;
  for (unsigned int index = 1; index < gimple_num_ops(statement); ++index)
    pointer_operand = pointer_operand ||
                      pointer_value(gimple_op(statement, index));
  if (pointer_operand && !pointer_result && !aggregate_result) {
    // Only a nullness test may convert pointer information to a scalar.
    // Pointer subtraction or casts can encode an address for later escape.
    if ((code != EQ_EXPR && code != NE_EXPR) ||
        gimple_num_ops(statement) != 3 ||
        (!null_pointer(gimple_assign_rhs1(statement)) &&
         !null_pointer(gimple_assign_rhs2(statement))))
      return query->reject("pointer-to-scalar-representation");
  }
  if (pointer_result) {
    if (TREE_CODE(rhs) == INTEGER_CST && !integer_zerop(rhs))
      return query->reject("integer-to-pointer-representation");
    if (pointer_value(rhs) &&
        !compatible_pointer_storage(rhs, TREE_TYPE(lhs), query))
      return query->reject("pointer-storage-type-erasure");
    if (gimple_assign_cast_p(statement) && !pointer_value(rhs))
      return query->reject("integer-to-pointer-representation");
  }
  if (walk_stmt_load_store_addr_ops(statement, query, inspect_load,
                                   nullptr, nullptr))
    return query->reject("pointer-representation-load");
  return !query->exhausted && analysis_budget::available();
}

bool exact_builtin(gimple *statement, enum built_in_function code) {
  tree declaration = gimple_call_fndecl(statement);
  return declaration != NULL_TREE &&
         fndecl_built_in_p(declaration, BUILT_IN_NORMAL) &&
         DECL_FUNCTION_CODE(declaration) == code;
}

bool termination_sink(gimple *statement) {
  if (!gimple_call_noreturn_p(statement) ||
      gimple_bb(statement) == nullptr ||
      EDGE_COUNT(gimple_bb(statement)->succs) != 0)
    return false;
  gimple_stmt_iterator last = gsi_last_bb(gimple_bb(statement));
  return !gsi_end_p(last) && gsi_stmt(last) == statement &&
         (exact_builtin(statement, BUILT_IN_EXIT) ||
          exact_builtin(statement, BUILT_IN_ABORT) ||
          exact_builtin(statement, BUILT_IN__EXIT) ||
          exact_builtin(statement, BUILT_IN__EXIT2));
}

cgraph_node *bound_callee(gimple *statement, cgraph_node *caller) {
  cgraph_edge *call = caller->get_edge(statement);
  if (call == nullptr || call->callee == nullptr ||
      call->indirect_unknown_callee || call->speculative ||
      !call->binds_to_current_def_p())
    return nullptr;
  cgraph_node *target = call->callee;
  // Do not silently walk thunks, aliases, IPA clones or altered signatures.
#if BUILDING_GCC_MAJOR >= 11
  if (target->thunk)
#else
  if (target->thunk.thunk_p)
#endif
    return nullptr;
  if (target->alias || target->clone_of != nullptr ||
      target->former_clone_of != NULL_TREE || target->decl == NULL_TREE ||
      DECL_WEAK(target->decl) ||
      DECL_ABSTRACT_ORIGIN(target->decl) != NULL_TREE ||
      !gimple_has_body_p(target->decl) || target->get_fun() == nullptr)
    return nullptr;
  enum availability availability = AVAIL_NOT_AVAILABLE;
  if (target->function_symbol(&availability, caller) != target ||
      availability < AVAIL_AVAILABLE)
    return nullptr;
  return target;
}

bool check_function(cgraph_node *, Query *, unsigned int);

bool modeled_scalar_storage_call(gimple *statement, Query *query) {
  const gcall *call = as_a<gcall *>(statement);
  tree result = gimple_call_lhs(call);
  if (result != NULL_TREE && aggregate_type(TREE_TYPE(result)))
    return false;
  bool saw_pointer = false;
  for (unsigned int index = 0; index < gimple_call_num_args(call); ++index) {
    tree argument = gimple_call_arg(call, index);
    if (argument == NULL_TREE || aggregate_type(TREE_TYPE(argument)))
      return false;
    if (!pointer_value(argument))
      continue;
    saw_pointer = true;
    if (!literal_pointer(argument) && pointed_shape(argument, query) != Shape::scalar)
      return false;
    const auto effect = gcc_call_effect::argument_effect(call, index);
    const auto source = effect.no_free_no_capture_source;
    if (!effect.no_free_no_capture ||
        (source != gcc_call_effect::ProofSource::builtin_identity &&
         source != gcc_call_effect::ProofSource::standard_stdio_model &&
         source != gcc_call_effect::ProofSource::standard_library_model))
      return false;
    if (result != NULL_TREE && !pointer_value(result) &&
        !effect.scalar_return_no_pointer)
      return false;
  }
  if (!saw_pointer)
    return false;
  if (result != NULL_TREE && pointer_value(result)) {
    if ((TREE_CODE(result) != SSA_NAME && !automatic_storage(result)) ||
        !gcc_call_effect::known_library_returns_argument(call, 0) ||
        !compatible_pointer_storage(gimple_call_arg(call, 0), TREE_TYPE(result), query))
      return false;
  }
  // Only scalar storage: a borrow of &pointer or a type-erased container
  // cannot hide a free(*pointer) behind this standard-library shortcut.
  return true;
}

bool call_confined(gimple *statement, cgraph_node *caller, Query *query,
                   unsigned int depth) {
  if (gimple_call_fndecl(statement) == NULL_TREE ||
      gimple_call_chain(statement) != NULL_TREE)
    return query->reject("indirect-or-static-chain-call");
  if ((gimple_call_flags(statement) & ECF_RETURNS_TWICE) != 0)
    return query->reject("returns-twice");
  if (gimple_call_noreturn_p(statement))
    return termination_sink(statement) ||
           query->reject("unknown-noreturn");
  if (ownership_deallocation_call_p(statement) ||
      (ownership_reallocation_call_p(statement) &&
       !ownership_null_reallocation_call_p(statement)))
    return query->reject("release-or-reallocation");

  tree result = gimple_call_lhs(statement);
  if (result != NULL_TREE &&
      (pointer_value(result) || aggregate_type(TREE_TYPE(result))) &&
      TREE_CODE(result) != SSA_NAME && !automatic_storage(result))
    return query->reject("nonlocal-pointer-call-result");

  if (exact_builtin(statement, BUILT_IN_MALLOC) ||
      exact_builtin(statement, BUILT_IN_CALLOC) ||
      ownership_null_reallocation_call_p(statement) ||
      exact_builtin(statement, BUILT_IN_ALLOCA) ||
      exact_builtin(statement, BUILT_IN_ALLOCA_WITH_ALIGN))
    return true;
  if (exact_builtin(statement, BUILT_IN_MEMSET) &&
      gimple_call_num_args(statement) == 3 &&
      TREE_CODE(gimple_call_arg(statement, 1)) == INTEGER_CST &&
      gimple_call_lhs(statement) == NULL_TREE)
    return true;

  if (modeled_scalar_storage_call(statement, query))
    return true;

  if (result != NULL_TREE &&
      (pointer_value(result) || aggregate_type(TREE_TYPE(result))))
    return query->reject("pointer-or-aggregate-call-result");
  bool carries_pointer = false;
  for (unsigned int index = 0; index < gimple_call_num_args(statement);
       ++index) {
    tree argument = gimple_call_arg(statement, index);
    if (argument == NULL_TREE)
      return query->reject("unknown-call-argument");
    if (aggregate_type(TREE_TYPE(argument)) ||
        (pointer_value(argument) && !literal_pointer(argument)))
      carries_pointer = true;
  }
  // A scalar-only call cannot obtain the caller's unpublished fresh object.
  // Literal strings are allowed; arbitrary static/local pointer arguments
  // are not. Representation conversions/loads are rejected independently.
  if (!carries_pointer)
    return true;
  cgraph_node *target = bound_callee(statement, caller);
  if (target == nullptr)
    return query->reject("unbound-pointer-bearing-call");
  tree formal = DECL_ARGUMENTS(target->decl);
  for (unsigned int index = 0; index < gimple_call_num_args(statement);
       ++index) {
    if (formal == NULL_TREE)
      return query->reject("call-formal-mismatch");
    tree actual = gimple_call_arg(statement, index);
    tree formal_type = TREE_TYPE(formal);
    if (pointer_type(formal_type)) {
      if (!compatible_pointer_storage(actual, formal_type, query))
        return query->reject("call-storage-type-erasure");
    } else if (aggregate_type(formal_type)) {
      if (TREE_TYPE(actual) == NULL_TREE ||
          TYPE_MAIN_VARIANT(formal_type) !=
              TYPE_MAIN_VARIANT(TREE_TYPE(actual)) ||
          shape(formal_type, query) == Shape::unknown)
        return query->reject("call-aggregate-layout-mismatch");
    } else if (pointer_value(actual) || aggregate_type(TREE_TYPE(actual))) {
      return query->reject("call-pointer-representation");
    }
    formal = DECL_CHAIN(formal);
  }
  if (formal != NULL_TREE)
    return query->reject("call-formal-mismatch");
  return check_function(target, query, depth + 1);
}

bool inspect_function(cgraph_node *node, Query *query, unsigned int depth) {
  basic_block block;
  FOR_EACH_BB_FN(block, cfun) {
    if (!query->charge())
      return query->reject("work-limit");
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs)
      if ((successor->flags &
           (EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_FAKE)) != 0)
        return query->reject("exceptional-control-flow");
    for (gphi_iterator phi = gsi_start_phis(block); !gsi_end_p(phi);
         gsi_next(&phi)) {
      gphi *statement = phi.phi();
      tree result = gimple_phi_result(statement);
      if (!pointer_value(result))
        continue;
      for (unsigned int index = 0; index < gimple_phi_num_args(statement);
           ++index)
        if (!query->charge() ||
            !compatible_pointer_storage(gimple_phi_arg_def(statement, index),
                                        TREE_TYPE(result), query))
          return query->reject("phi-storage-type-erasure");
    }
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!query->charge())
        return query->reject("work-limit");
      gimple *statement = gsi_stmt(gsi);
      if (is_gimple_assign(statement)) {
        if (!assignment_confined(statement, query))
          return false;
      } else if (is_gimple_call(statement)) {
        if (!call_confined(statement, node, query, depth))
          return false;
      } else if (gimple_code(statement) == GIMPLE_RETURN) {
        tree value = gimple_return_retval(as_a<greturn *>(statement));
        if (value != NULL_TREE &&
            (pointer_value(value) || aggregate_type(TREE_TYPE(value))))
          return query->reject("pointer-or-aggregate-return");
      } else if (gimple_code(statement) == GIMPLE_ASM) {
        return query->reject("inline-assembly");
      } else if (gimple_code(statement) == GIMPLE_COND) {
        tree left = gimple_cond_lhs(statement);
        tree right = gimple_cond_rhs(statement);
        if ((pointer_value(left) || pointer_value(right)) &&
            ((gimple_cond_code(statement) != EQ_EXPR &&
              gimple_cond_code(statement) != NE_EXPR) ||
             (!null_pointer(left) && !null_pointer(right))))
          return query->reject("pointer-address-observation");
      } else if (gimple_code(statement) != GIMPLE_SWITCH &&
                 gimple_code(statement) != GIMPLE_LABEL &&
                 gimple_code(statement) != GIMPLE_GOTO &&
                 gimple_code(statement) != GIMPLE_NOP &&
                 gimple_code(statement) != GIMPLE_DEBUG &&
                 gimple_code(statement) != GIMPLE_PREDICT) {
        return query->reject("unsupported-statement");
      }
    }
  }
  return !query->exhausted && analysis_budget::available();
}

bool check_function(cgraph_node *node, Query *query, unsigned int depth) {
  if (node == nullptr || node->decl == NULL_TREE ||
      node->get_fun() == nullptr || depth >= max_call_depth)
    return query->reject("call-depth-or-body-limit");
  const auto existing = cache.find(node->decl);
  if (existing != cache.end())
    return existing->second == State::confined ||
           query->reject(existing->second == State::active
                             ? "recursive-call" : "cached-unproved");
  if (cache.size() >= max_functions)
    return query->reject("function-cache-limit");
  cache[node->decl] = State::active;
  push_cfun(node->get_fun());
  const bool result = inspect_function(node, query, depth);
  pop_cfun();
  cache[node->decl] = result ? State::confined : State::rejected;
  return result;
}

}  // namespace

void reset() { cache.clear(); }

bool fresh_local_ownership_is_confined() {
  if (cfun == nullptr || cfun->decl == NULL_TREE ||
      !analysis_budget::available())
    return false;
  Query query;
  const bool result = check_function(cgraph_node::get(cfun->decl), &query, 0);
  if (dump_file != nullptr && (dump_flags & TDF_DETAILS))
    dump_printf(MSG_NOTE,
                "local-confinement: function=%s result=%s reason=%s "
                "work=%u cached-functions=%u\n",
                lang_hooks.decl_printable_name(cfun->decl, 2),
                result ? "proved" : "unknown",
                result ? "closed-bound-body" : query.reason,
                query.statements, static_cast<unsigned int>(cache.size()));
  return result;
}

}  // namespace local_confinement
}  // namespace static_analyzer
