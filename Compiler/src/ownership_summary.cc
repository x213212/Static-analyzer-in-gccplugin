#include "gcc-plugin.h"
#include "bversion.h"
#include "tree.h"
#include "tree-dfa.h"
#include "fold-const.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "gimple-walk.h"
#include "gimple-ssa.h"
#include "tree-cfg.h"
#include "tree-ssa.h"
#include "ssa.h"
#include "cgraph.h"
#include "stringpool.h"
#include "attribs.h"
#include "dumpfile.h"

#include "static_analyzer/gimple_flow.h"
#include "static_analyzer/gimple_value.h"
#include "static_analyzer/gcc_call_effect.h"
#include "static_analyzer/ownership_call.h"
#include "static_analyzer/analysis_budget.h"
#include "static_analyzer/ownership_summary.h"
#include "static_analyzer/provider_transport.h"
#include "static_analyzer/memory_value.h"
#include "static_analyzer/trusted_contract.h"
#include "static_analyzer/report.h"
#include "static_analyzer/options.h"

#include <algorithm>
#include <deque>
#include <functional>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace static_analyzer {
namespace ownership_summary {
namespace {

CalleeModel configured_callee_model = CalleeModel::unknown;
std::string configured_callee_symbol;
unsigned int configured_callee_argument = 0;

enum class ReturnKind {
  null_value,
  fresh_owned,
  parameter,
  realloc_parameter,
  global_object,
  fresh_shared,
  unknown
};

struct ReturnAlternative {
  ReturnKind kind = ReturnKind::unknown;
  unsigned int parameter_index = 0;
  const gimple *fresh_origin = nullptr;
  bool derived = false;
  location_t source_location = UNKNOWN_LOCATION;
};

struct ParameterSummary {
  tree declaration = NULL_TREE;
  // Unloaded LTRANS declarations need not retain DECL_ARGUMENTS.
  tree formal_type = NULL_TREE;
  tree default_definition = NULL_TREE;
  bool pointer = false;
  unsigned int fates = fate_unknown;
  bool may_dereference = true;
  AccessGuarantee access = AccessGuarantee::none;
};

struct FunctionSummary {
  tree declaration = NULL_TREE;
  tree function_type = NULL_TREE;
  tree assembler_name = NULL_TREE;
  bool provider_imported = false;
  bool analyzed = false;
  bool unsupported = false;
  bool recursive = false;
  bool truncated = false;
  bool has_normal_return = false;
  bool returns_fresh_owned = false;
  bool returns_specializable_owned = false;
  std::vector<ParameterSummary> parameters;
  std::vector<ReturnAlternative> returns;
};

struct Meaning {
  std::vector<ReturnAlternative> alternatives;
  bool complete = true;
};

struct Statistics {
  unsigned int functions = 0;
  unsigned int usable = 0;
  unsigned int unsupported = 0;
  unsigned int recursive = 0;
  unsigned int fresh_returners = 0;
  unsigned int fixed_point_rounds = 0;
  unsigned int evaluations = 0;
  unsigned long resolved_queries = 0;
  unsigned long rejected_queries = 0;
  unsigned int provider_imported = 0;
  unsigned int provider_rejected = 0;
};

std::map<tree, FunctionSummary> summaries;
std::map<tree, FunctionSummary> provider_summaries;
// Relative effects are intentionally independent of the pointer-parameter
// summary graph: a zero-argument global sink or an aggregate-only sink is
// relevant here even though neither has an ownership-bearing formal.
struct RelativeReleaseSummary {
  tree declaration = NULL_TREE;
  tree function_type = NULL_TREE;
  tree assembler_name = NULL_TREE;
  int node_uid = 0;
  function *body = nullptr;
  tree reference = NULL_TREE;
  location_t release_location = UNKNOWN_LOCATION;
  std::vector<tree> formals;
  bool usable = false;
};
std::map<tree, RelativeReleaseSummary> relative_release_summaries;
struct RelativeAccessSummary {
  RelativeReleaseSummary signature;
  location_t access_location = UNKNOWN_LOCATION;
  AccessGuarantee access = AccessGuarantee::none;
};
std::map<tree, RelativeAccessSummary> relative_access_summaries;
struct ConditionalBorrowSummary {
  RelativeReleaseSummary signature;
  unsigned int pointer_index = 0;
  unsigned int selector_index = 0;
  bool when_zero = false;
  location_t guard_location = UNKNOWN_LOCATION;
};
std::map<tree, ConditionalBorrowSummary> conditional_borrow_summaries;
struct ReturnedObjectSummary {
  RelativeReleaseSummary signature;
  ReturnedObjectEffect effect;
};
std::map<tree, ReturnedObjectSummary> returned_object_summaries;
struct InvocationEffectKey {
  const gimple *call;
  tree vuse;
  int node_uid;
  unsigned int argument;
  bool operator<(const InvocationEffectKey &other) const {
    if (call != other.call)
      return std::less<const gimple *>()(call, other.call);
    if (vuse != other.vuse)
      return std::less<tree>()(vuse, other.vuse);
    if (node_uid != other.node_uid)
      return node_uid < other.node_uid;
    return argument < other.argument;
  }
};
struct InvocationFreeResult {
  bool proved = false;
  ReleaseOrigin origin;
};
std::map<InvocationEffectKey, InvocationFreeResult> invocation_frees;
std::map<tree, cgraph_node *> summary_nodes;
std::set<tree> recursive_functions;
std::set<tree> budget_unsupported_functions;
Statistics statistics;
bool summaries_built = false;
bool generating_provider_facts = false;

constexpr unsigned int max_return_alternatives = 256;
constexpr unsigned int max_summary_parameters = 256;
constexpr unsigned int max_summary_blocks = 10000;
constexpr unsigned int provider_schema_version = 2;
constexpr unsigned int provider_payload_length = 7;

bool pointer_value(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
         CODE_CONTAINS_STRUCT(TREE_CODE(value), TS_TYPED) &&
         TREE_TYPE(value) != NULL_TREE &&
         POINTER_TYPE_P(TREE_TYPE(value));
}

location_t statement_location(const gimple *statement) {
  if (statement == nullptr)
    return UNKNOWN_LOCATION;
  const location_t nonartificial =
      gimple_nonartificial_location(statement);
  if (LOCATION_LOCUS(nonartificial) != UNKNOWN_LOCATION)
    return nonartificial;
  return gimple_location(statement);
}

bool same_return_alternative(const ReturnAlternative &left,
                             const ReturnAlternative &right,
                             bool include_origin) {
  return left.kind == right.kind &&
         left.parameter_index == right.parameter_index &&
         left.derived == right.derived &&
         left.source_location == right.source_location &&
         (!include_origin || left.fresh_origin == right.fresh_origin);
}

void append_alternative(Meaning *meaning,
                        const ReturnAlternative &alternative) {
  const bool include_origin = alternative.kind == ReturnKind::fresh_owned;
  for (const ReturnAlternative &existing : meaning->alternatives)
    if (same_return_alternative(existing, alternative, include_origin))
      return;
  if (meaning->alternatives.size() >= max_return_alternatives) {
    meaning->alternatives.clear();
    meaning->alternatives.push_back(
        {ReturnKind::unknown, 0, nullptr, false});
    meaning->complete = false;
    return;
  }
  meaning->alternatives.push_back(alternative);
}

void append_meaning(Meaning *destination, const Meaning &source) {
  for (const ReturnAlternative &alternative : source.alternatives)
    append_alternative(destination, alternative);
  destination->complete = destination->complete && source.complete;
}

Meaning unknown_meaning() {
  Meaning meaning;
  meaning.complete = false;
  append_alternative(&meaning, {ReturnKind::unknown, 0, nullptr, false});
  return meaning;
}

bool pointer_preserving_cast_p(const gimple *statement) {
  if (!is_gimple_assign(statement) || !gimple_assign_cast_p(statement))
    return false;
  tree lhs = gimple_assign_lhs(statement);
  tree rhs = gimple_assign_rhs1(statement);
  if (!pointer_value(lhs) || !pointer_value(rhs))
    return false;
  tree lhs_target = TREE_TYPE(TREE_TYPE(lhs));
  tree rhs_target = TREE_TYPE(TREE_TYPE(rhs));
  return lhs_target != NULL_TREE && rhs_target != NULL_TREE &&
         TYPE_ADDR_SPACE(lhs_target) == TYPE_ADDR_SPACE(rhs_target) &&
         tree_nop_conversion_p(TREE_TYPE(lhs), TREE_TYPE(rhs));
}

bool nonlocal_storage_p(tree lhs) {
  if (lhs == NULL_TREE || TREE_CODE(lhs) == SSA_NAME)
    return false;
  tree base = get_base_address(lhs);
  return base == NULL_TREE || !DECL_P(base) ||
         !auto_var_in_fn_p(base, cfun->decl);
}

cgraph_node *bound_target_identity(const gimple *statement,
                                  cgraph_node *caller) {
  if (statement == nullptr || !is_gimple_call(statement) || caller == nullptr)
    return nullptr;
  cgraph_edge *call_edge = caller->get_edge(const_cast<gimple *>(statement));
  if (call_edge == nullptr || call_edge->callee == nullptr ||
      call_edge->indirect_unknown_callee || call_edge->speculative ||
      !call_edge->binds_to_current_def_p())
    return nullptr;

  enum availability availability = AVAIL_NOT_AVAILABLE;
  // An alias preserves the ABI argument values, but a thunk can adjust this
  // and/or the returned pointer.  function_symbol walks through thunks and
  // would incorrectly apply the underlying body's formal/return identities
  // to the thunk's unadjusted actual arguments.  Keep that boundary explicit
  // until thunk argument and return adjustments are modeled.
  cgraph_node *target =
      call_edge->callee->ultimate_alias_target(&availability, caller);
  if (target == nullptr || availability < AVAIL_AVAILABLE ||
      target->decl == NULL_TREE || DECL_WEAK(target->decl))
    return nullptr;
#if BUILDING_GCC_MAJOR >= 11
  if (target->thunk)
#else
  if (target->thunk.thunk_p)
#endif
    return nullptr;
  return target;
}

cgraph_node *bound_target(const gimple *statement, cgraph_node *caller) {
  cgraph_node *target = bound_target_identity(statement, caller);
  if (target == nullptr || !target->has_gimple_body_p() ||
      !gimple_has_body_p(target->decl) || target->get_fun() == nullptr)
    return nullptr;
  return target;
}

bool call_matches_formals(const gimple *statement,
                          const FunctionSummary &summary) {
  if (statement == nullptr || !is_gimple_call(statement) ||
      gimple_call_num_args(statement) != summary.parameters.size())
    return false;
  for (unsigned int index = 0; index < summary.parameters.size(); ++index) {
    if (!summary.parameters[index].pointer)
      continue;
    tree actual = gimple_call_arg(statement, index);
    tree formal_type = summary.parameters[index].formal_type;
    if (!pointer_value(actual) || formal_type == NULL_TREE ||
        !POINTER_TYPE_P(formal_type))
      return false;
    tree actual_target = TREE_TYPE(TREE_TYPE(actual));
    tree formal_target = TREE_TYPE(formal_type);
    if (actual_target == NULL_TREE || formal_target == NULL_TREE ||
        TYPE_ADDR_SPACE(actual_target) != TYPE_ADDR_SPACE(formal_target))
      return false;
  }
  return true;
}

const FunctionSummary *summary_for_call(const gimple *statement) {
  if (!summaries_built && summaries.empty())
    return nullptr;
  cgraph_node *caller =
      cfun != nullptr && cfun->decl != NULL_TREE
          ? cgraph_node::get(cfun->decl)
          : nullptr;
  cgraph_node *target = bound_target(statement, caller);
  if (target == nullptr)
    return nullptr;
  const auto found = summaries.find(target->decl);
  if (found == summaries.end() || !found->second.analyzed ||
      found->second.unsupported || found->second.recursive ||
      !call_matches_formals(statement, found->second))
    return nullptr;
  return &found->second;
}

// The transport is deliberately a closed, versioned theorem schema, not a
// pointer/name lookup.  GCC streams the original complete type together with
// the payload; exact merged type identity also detects IPA signature changes.
bool complete_formal_types(tree function_type, std::vector<tree> *types) {
  if (function_type == NULL_TREE ||
      (TREE_CODE(function_type) != FUNCTION_TYPE &&
       TREE_CODE(function_type) != METHOD_TYPE) ||
      TREE_TYPE(function_type) == NULL_TREE ||
      !TYPE_P(TREE_TYPE(function_type)))
    return false;
  tree arguments = TYPE_ARG_TYPES(function_type);
  // A NULL list is an unprototyped C function, or a variadic terminator.
  for (unsigned int index = 0; arguments != NULL_TREE &&
                               index <= max_summary_parameters;
       ++index, arguments = TREE_CHAIN(arguments)) {
    if (TREE_CODE(arguments) != TREE_LIST)
      return false;
    tree type = TREE_VALUE(arguments);
    if (type == void_type_node)
      return TREE_CHAIN(arguments) == NULL_TREE;
    if (type == NULL_TREE || !TYPE_P(type) ||
        index == max_summary_parameters)
      return false;
    types->push_back(type);
  }
  return false;
}

constexpr unsigned int max_relative_release_summaries = 4096;
constexpr unsigned int max_relative_release_steps = 24;
constexpr unsigned int max_relative_release_statements = 64;

bool relative_step(unsigned int *remaining) {
  if (*remaining == 0 || !analysis_budget::consume(
                             analysis_budget::WorkKind::summary_parameter_cfg))
    return false;
  --*remaining;
  return true;
}

bool fixed_object_type(tree type) {
  return type != NULL_TREE && TYPE_P(type) && !TYPE_VOLATILE(type) &&
         TYPE_SIZE(type) != NULL_TREE &&
         TREE_CODE(TYPE_SIZE(type)) == INTEGER_CST &&
         tree_fits_uhwi_p(TYPE_SIZE(type)) &&
         tree_to_uhwi(TYPE_SIZE(type)) != 0;
}

bool same_pointer_representation(tree left, tree right) {
  return left != NULL_TREE && right != NULL_TREE &&
         POINTER_TYPE_P(left) && POINTER_TYPE_P(right) &&
         TYPE_MODE(left) == TYPE_MODE(right) &&
         TYPE_PRECISION(left) == TYPE_PRECISION(right) &&
         TYPE_ADDR_SPACE(TREE_TYPE(left)) ==
             TYPE_ADDR_SPACE(TREE_TYPE(right)) &&
         tree_nop_conversion_p(left, right);
}

bool same_integral_representation(tree left, tree right) {
  return left != NULL_TREE && right != NULL_TREE && INTEGRAL_TYPE_P(left) &&
         INTEGRAL_TYPE_P(right) && TYPE_MODE(left) == TYPE_MODE(right) &&
         TYPE_PRECISION(left) == TYPE_PRECISION(right) &&
         TYPE_UNSIGNED(left) == TYPE_UNSIGNED(right) &&
         types_compatible_p(left, right);
}

bool relative_original_node(cgraph_node *target) {
  if (target == nullptr || target->decl == NULL_TREE || target->alias ||
      target->clone_of != nullptr || target->former_clone_of != NULL_TREE ||
      target->inlined_to != nullptr || DECL_WEAK(target->decl) ||
      DECL_ABSTRACT_ORIGIN(target->decl) != NULL_TREE ||
      target->get_fun() == nullptr || target->get_fun()->cfg == nullptr ||
      !gimple_in_ssa_p(target->get_fun()))
    return false;
#if BUILDING_GCC_MAJOR >= 11
  return !target->thunk;
#else
  return !target->thunk.thunk_p;
#endif
}

bool empty_nonvariadic_arguments(tree type) {
  if (type == NULL_TREE || TREE_CODE(type) != FUNCTION_TYPE || stdarg_p(type))
    return false;
  tree arguments = TYPE_ARG_TYPES(type);
  return arguments == NULL_TREE ||
         (TREE_CODE(arguments) == TREE_LIST &&
          TREE_VALUE(arguments) == void_type_node &&
          TREE_CHAIN(arguments) == NULL_TREE);
}

bool relative_call_type_matches(tree call_type, tree body_type,
                                 bool no_actual_or_formal_arguments) {
  if (call_type == body_type)
    return true;
  // LTO can retain distinct old-style C function-type trees for a prototype
  // and its prevailing definition.  With no actual or formal arguments and
  // a void result, there is no argument layout to reinterpret.  Preserve all
  // ABI attributes/qualifiers and exclude even C23 unnamed variadic lists.
  return no_actual_or_formal_arguments &&
         empty_nonvariadic_arguments(call_type) &&
         empty_nonvariadic_arguments(body_type) &&
         TREE_TYPE(call_type) == void_type_node &&
         TREE_TYPE(body_type) == void_type_node &&
         TYPE_MODE(call_type) == TYPE_MODE(body_type) &&
         TYPE_QUALS(call_type) == TYPE_QUALS(body_type) &&
         attribute_list_equal(TYPE_ATTRIBUTES(call_type),
                              TYPE_ATTRIBUTES(body_type));
}

bool relative_formal_type_matches(tree signature_type, tree formal_type) {
  if (TYPE_MAIN_VARIANT(signature_type) == TYPE_MAIN_VARIANT(formal_type))
    return true;
  // The streamed signature and DECL_ARGUMENTS may contain distinct pointer
  // type nodes after LTO.  Ask GCC whether conversions are useless in both
  // directions in addition to checking mode/precision/address space.  This
  // exception never applies to an aggregate passed by value: its original
  // field/layout identity remains mandatory for reference substitution.
  return same_pointer_representation(signature_type, formal_type) &&
         types_compatible_p(signature_type, formal_type);
}

// Equality is deliberately stronger than pointer count or convertibility.
// In particular an IPA-adjusted, variadic, or differently declared call must
// never reinterpret the original body's aggregate layout/formal numbering.
bool relative_signature_matches(const gimple *call, cgraph_node *target,
                                const RelativeReleaseSummary &summary,
                                bool permit_integral_actuals = false) {
  if (!relative_original_node(target) || target->decl != summary.declaration ||
      target->get_uid() != summary.node_uid || target->get_fun() != summary.body ||
      TREE_TYPE(target->decl) != summary.function_type ||
      stdarg_p(summary.function_type) ||
      !DECL_ASSEMBLER_NAME_SET_P(target->decl) ||
      DECL_ASSEMBLER_NAME(target->decl) != summary.assembler_name ||
      !relative_call_type_matches(gimple_call_fntype(as_a<const gcall *>(call)),
                                   summary.function_type,
                                   gimple_call_num_args(call) == 0 &&
                                       summary.formals.empty()) ||
      gimple_call_num_args(call) != summary.formals.size())
    return false;
  std::vector<tree> types;
  if (!complete_formal_types(summary.function_type, &types)) {
    // A genuine zero-argument C definition can have an old-style type.
    // Do not extend this exception to a single unknown or variadic argument.
    if (!summary.formals.empty() ||
        TYPE_ARG_TYPES(summary.function_type) != NULL_TREE)
      return false;
  } else if (types.size() != summary.formals.size()) {
    return false;
  }
  tree formal = DECL_ARGUMENTS(target->decl);
  for (unsigned int index = 0; index < summary.formals.size(); ++index) {
    tree actual = gimple_call_arg(call, index);
    if (formal != summary.formals[index] || actual == NULL_TREE)
      return false;
    // GIMPLE discards no-op pointer casts and array decay conversions.  The
    // exact call function type above still fixes the callee ABI and layout;
    // only the pointer address root may have a representationally identical
    // type.  Aggregates and other scalars continue to require exact identity.
    if ((TREE_TYPE(actual) != TREE_TYPE(formal) &&
         !same_pointer_representation(TREE_TYPE(actual), TREE_TYPE(formal)) &&
         !(permit_integral_actuals &&
           same_integral_representation(TREE_TYPE(actual), TREE_TYPE(formal)))) ||
        (!types.empty() &&
         !relative_formal_type_matches(types[index], TREE_TYPE(formal))))
      return false;
    formal = DECL_CHAIN(formal);
  }
  return formal == NULL_TREE;
}

bool relative_copy_assignment(const gimple *statement) {
  if (!is_gimple_assign(statement) || gimple_vdef(statement) != NULL_TREE ||
      gimple_has_volatile_ops(statement) ||
      !pointer_value(gimple_assign_lhs(statement)) ||
      TREE_CODE(gimple_assign_lhs(statement)) != SSA_NAME)
    return false;
  if (gimple_assign_rhs_code(statement) == SSA_NAME)
    return same_pointer_representation(TREE_TYPE(gimple_assign_lhs(statement)),
                                       TREE_TYPE(gimple_assign_rhs1(statement)));
  return pointer_preserving_cast_p(statement);
}

bool returned_normal_edge(edge successor) {
  return successor != nullptr &&
         (successor->flags &
          (EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_FAKE)) == 0;
}

bool returned_builtin_type_matches(tree call_type, tree declaration_type) {
  if (call_type == NULL_TREE || declaration_type == NULL_TREE ||
      TREE_CODE(call_type) != FUNCTION_TYPE ||
      TREE_CODE(declaration_type) != FUNCTION_TYPE ||
      stdarg_p(call_type) || stdarg_p(declaration_type) ||
      TYPE_ARG_TYPES(call_type) == NULL_TREE ||
      TYPE_ARG_TYPES(declaration_type) == NULL_TREE ||
      TYPE_MODE(call_type) != TYPE_MODE(declaration_type) ||
      !attribute_list_equal(TYPE_ATTRIBUTES(call_type),
                            TYPE_ATTRIBUTES(declaration_type)) ||
      !types_compatible_p(call_type, declaration_type) ||
      TYPE_MAIN_VARIANT(TREE_TYPE(call_type)) !=
          TYPE_MAIN_VARIANT(TREE_TYPE(declaration_type)))
    return false;
  // The official gimple_call_builtin_p query below validates the lowered
  // actuals and LHS against builtin_decl_explicit's complete prototype.
  // These additional checks preserve function ABI attributes without
  // rejecting GCC's size_t typedef nodes or noreturn-qualified exit type.
  return true;
}

bool returned_exact_builtin(const gimple *statement,
                            enum built_in_function code,
                            unsigned int arguments, bool terminating = false) {
  if (!is_gimple_call(statement))
    return false;
  tree declaration = gimple_call_fndecl(statement);
  return declaration != NULL_TREE &&
         fndecl_built_in_p(declaration, BUILT_IN_NORMAL) &&
         DECL_FUNCTION_CODE(declaration) == code &&
         gimple_call_builtin_p(statement, code) &&
         gimple_call_num_args(statement) == arguments &&
         returned_builtin_type_matches(
             gimple_call_fntype(as_a<const gcall *>(statement)),
             TREE_TYPE(declaration)) &&
         (gimple_call_flags(statement) & ECF_RETURNS_TWICE) == 0 &&
         gimple_call_noreturn_p(statement) == terminating &&
         gimple_call_nothrow_p(as_a<gcall *>(const_cast<gimple *>(statement)));
}

// Only a single known process-exit statement may inhabit the allocation's
// NULL arm.  This is not a generic noreturn/normal-return-existence theorem.
bool returned_null_failure_sink(basic_block block, basic_block guard,
                                unsigned int *remaining) {
  if (block == nullptr || !single_pred_p(block) ||
      single_pred(block) != guard || EDGE_COUNT(block->succs) != 0 ||
      !gsi_end_p(gsi_start_phis(block)))
    return false;
  bool terminated = false;
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    const gimple *statement = gsi_stmt(iterator);
    if (!relative_step(remaining))
      return false;
    if (is_gimple_debug(statement) || gimple_code(statement) == GIMPLE_LABEL ||
        gimple_code(statement) == GIMPLE_NOP)
      continue;
    if (terminated || gimple_has_volatile_ops(statement) ||
        !is_gimple_call(statement) || gimple_call_lhs(statement) != NULL_TREE)
      return false;
    const bool exit_call =
        returned_exact_builtin(statement, BUILT_IN_EXIT, 1, true) ||
        returned_exact_builtin(statement, BUILT_IN__EXIT, 1, true) ||
        returned_exact_builtin(statement, BUILT_IN__EXIT2, 1, true);
    if (!returned_exact_builtin(statement, BUILT_IN_ABORT, 0, true) &&
        (!exit_call || TREE_CODE(gimple_call_arg(statement, 0)) != INTEGER_CST))
      return false;
    terminated = true;
  }
  return terminated;
}

ReturnedObjectSummary build_returned_object_summary(cgraph_node *target) {
  ReturnedObjectSummary result;
  RelativeReleaseSummary &signature = result.signature;
  signature.declaration = target->decl;
  signature.function_type = TREE_TYPE(target->decl);
  signature.assembler_name = DECL_ASSEMBLER_NAME(target->decl);
  signature.node_uid = target->get_uid();
  signature.body = target->get_fun();
  if (!relative_original_node(target) || TREE_THIS_VOLATILE(target->decl) ||
      !POINTER_TYPE_P(TREE_TYPE(signature.function_type)) ||
      n_basic_blocks_for_fn(signature.body) > 34)
    return result;
  for (tree formal = DECL_ARGUMENTS(target->decl); formal != NULL_TREE;
       formal = DECL_CHAIN(formal)) {
    if (signature.formals.size() >= max_summary_parameters ||
        TREE_CODE(formal) != PARM_DECL || TREE_THIS_VOLATILE(formal))
      return result;
    signature.formals.push_back(formal);
    tree value = ssa_default_def(signature.body, formal);
    // The initial theorem is input-independent.  In particular, freeing a
    // passed-in object and returning it cannot manufacture a new identity.
    if (value != NULL_TREE && !has_zero_uses(value))
      return result;
  }
  basic_block entry = ENTRY_BLOCK_PTR_FOR_FN(signature.body);
  if (!single_succ_p(entry))
    return result;
  edge incoming = single_succ_edge(entry);
  std::set<basic_block> visited;
  std::set<tree> exact_values;
  unsigned int remaining = 64;
  bool allocated = false, nonnull = false, released = false;
  location_t allocation_location = UNKNOWN_LOCATION;
  location_t release_location = UNKNOWN_LOCATION;
  while (incoming != nullptr) {
    basic_block block = incoming->dest;
    if (!returned_normal_edge(incoming) || visited.size() >= 32 ||
        block == EXIT_BLOCK_PTR_FOR_FN(signature.body) ||
        !visited.insert(block).second || !single_pred_p(block) ||
        !gsi_end_p(gsi_start_phis(block)) || !relative_step(&remaining))
      return result;
    edge selected = nullptr;
    for (gimple_stmt_iterator iterator = gsi_start_bb(block);
         !gsi_end_p(iterator); gsi_next(&iterator)) {
      const gimple *statement = gsi_stmt(iterator);
      if (!relative_step(&remaining))
        return result;
      if (is_gimple_debug(statement) || gimple_code(statement) == GIMPLE_LABEL ||
          gimple_code(statement) == GIMPLE_NOP)
        continue;
      if (selected != nullptr || gimple_has_volatile_ops(statement))
        return result;
      if (gimple_code(statement) == GIMPLE_RETURN) {
        tree returned = gimple_return_retval(as_a<const greturn *>(statement));
        if (!allocated || !nonnull || !released ||
            exact_values.count(returned) == 0 || !single_succ_p(block) ||
            !returned_normal_edge(single_succ_edge(block)) ||
            single_succ(block) != EXIT_BLOCK_PTR_FOR_FN(signature.body) ||
            !same_pointer_representation(TREE_TYPE(returned),
                                         TREE_TYPE(signature.function_type)))
          return result;
        result.effect.resolved = true;
        result.effect.lifetime = ReturnedObjectLifetime::released;
        result.effect.may_be_null = false;
        result.effect.allocation_location = allocation_location;
        result.effect.release_location = release_location;
        result.effect.return_location = statement_location(statement);
        return result;
      }
      if (gimple_code(statement) == GIMPLE_COND) {
        if (!allocated || nonnull || released || EDGE_COUNT(block->succs) != 2)
          return result;
        const gcond *condition = as_a<const gcond *>(statement);
        tree value = gimple_cond_lhs(condition), zero = gimple_cond_rhs(condition);
        if (value != NULL_TREE && TREE_CODE(value) == INTEGER_CST)
          std::swap(value, zero);
        const enum tree_code code = gimple_cond_code(condition);
        if (exact_values.count(value) == 0 || zero == NULL_TREE ||
            TREE_CODE(zero) != INTEGER_CST || !pointer_value(zero) ||
            !integer_zerop(zero) || (code != EQ_EXPR && code != NE_EXPR))
          return result;
        const int null_flag = code == EQ_EXPR ? EDGE_TRUE_VALUE : EDGE_FALSE_VALUE;
        edge failure = nullptr;
        edge successor;
        edge_iterator iterator;
        FOR_EACH_EDGE(successor, iterator, block->succs) {
          if (!returned_normal_edge(successor))
            return result;
          const int flags = successor->flags & (EDGE_TRUE_VALUE | EDGE_FALSE_VALUE);
          if (flags == null_flag) {
            if (failure != nullptr)
              return result;
            failure = successor;
          } else if (flags == (null_flag == EDGE_TRUE_VALUE
                                   ? EDGE_FALSE_VALUE : EDGE_TRUE_VALUE)) {
            if (selected != nullptr)
              return result;
            selected = successor;
          } else {
            return result;
          }
        }
        if (failure == nullptr || selected == nullptr ||
            failure->dest == selected->dest || visited.count(failure->dest) ||
            !returned_null_failure_sink(failure->dest, block, &remaining))
          return result;
        nonnull = true;
        continue;
      }
      if (is_gimple_call(statement)) {
        if (!allocated && returned_exact_builtin(statement, BUILT_IN_MALLOC, 1)) {
          tree size = gimple_call_arg(statement, 0);
          tree value = gimple_call_lhs(statement);
          // Positive bounded object size excludes zero-size and impossible
          // unsigned-overflow allocations from the normal-return witness.
          if (size == NULL_TREE || TREE_CODE(size) != INTEGER_CST ||
              !INTEGRAL_TYPE_P(TREE_TYPE(size)) || TREE_OVERFLOW(size) ||
              !tree_fits_shwi_p(size) || tree_to_shwi(size) <= 0 ||
              tree_int_cst_lt(max_object_size(), size) ||
              !pointer_value(value) || TREE_CODE(value) != SSA_NAME ||
              !same_pointer_representation(
                  TREE_TYPE(value),
                  TREE_TYPE(TREE_TYPE(gimple_call_fndecl(statement)))))
            return result;
          allocated = true;
          exact_values.insert(value);
          allocation_location = statement_location(statement);
          continue;
        }
        if (!allocated || !nonnull || released ||
            !returned_exact_builtin(statement, BUILT_IN_FREE, 1) ||
            gimple_call_lhs(statement) != NULL_TREE ||
            exact_values.count(gimple_call_arg(statement, 0)) == 0)
          return result;
        released = true;
        release_location = statement_location(statement);
        continue;
      }
      if (!relative_copy_assignment(statement) ||
          exact_values.count(gimple_assign_rhs1(statement)) == 0)
        return result;
      exact_values.insert(gimple_assign_lhs(statement));
    }
    if (selected != nullptr) {
      incoming = selected;
    } else {
      if (!single_succ_p(block))
        return result;
      incoming = single_succ_edge(block);
    }
  }
  return result;
}

tree relative_copied_value(tree value, basic_block block,
                           unsigned int *remaining) {
  while (value != NULL_TREE && TREE_CODE(value) == SSA_NAME &&
         !SSA_NAME_IS_DEFAULT_DEF(value)) {
    if (!relative_step(remaining))
      return NULL_TREE;
    const gimple *definition = SSA_NAME_DEF_STMT(value);
    if (definition == nullptr || gimple_bb(definition) != block ||
        !relative_copy_assignment(definition))
      break;
    value = gimple_assign_rhs1(definition);
  }
  return value;
}

RelativeReleaseSummary build_relative_release_summary(cgraph_node *target) {
  RelativeReleaseSummary result;
  result.declaration = target->decl;
  result.function_type = TREE_TYPE(target->decl);
  result.assembler_name = DECL_ASSEMBLER_NAME(target->decl);
  result.node_uid = target->get_uid();
  result.body = target->get_fun();
  if (!relative_original_node(target) ||
      TREE_THIS_VOLATILE(target->decl) ||
      n_basic_blocks_for_fn(result.body) != 3)
    return result;
  for (tree formal = DECL_ARGUMENTS(target->decl); formal != NULL_TREE;
       formal = DECL_CHAIN(formal)) {
    if (result.formals.size() >= max_summary_parameters ||
        TREE_CODE(formal) != PARM_DECL || TREE_THIS_VOLATILE(formal))
      return result;
    result.formals.push_back(formal);
  }
  basic_block block = single_succ_p(ENTRY_BLOCK_PTR_FOR_FN(result.body))
                          ? single_succ(ENTRY_BLOCK_PTR_FOR_FN(result.body))
                          : nullptr;
  if (block == nullptr || block == EXIT_BLOCK_PTR_FOR_FN(result.body) ||
      !single_pred_p(block) ||
      single_pred(block) != ENTRY_BLOCK_PTR_FOR_FN(result.body) ||
      !single_succ_p(block) ||
      single_succ(block) != EXIT_BLOCK_PTR_FOR_FN(result.body) ||
      !gsi_end_p(gsi_start_phis(block)))
    return result;
  const int invalid_edges = EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_EH | EDGE_FAKE;
  if ((single_pred_edge(block)->flags & invalid_edges) != 0 ||
      (single_succ_edge(block)->flags & invalid_edges) != 0)
    return result;

  const gimple *load = nullptr;
  const gimple *release = nullptr;
  bool returned = false;
  unsigned int statements = 0;
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    const gimple *statement = gsi_stmt(iterator);
    if (++statements > max_relative_release_statements ||
        !analysis_budget::consume(
            analysis_budget::WorkKind::summary_parameter_cfg))
      return result;
    if (is_gimple_debug(statement) || gimple_code(statement) == GIMPLE_LABEL ||
        gimple_code(statement) == GIMPLE_NOP)
      continue;
    if (returned || gimple_has_volatile_ops(statement))
      return result;
    if (gimple_code(statement) == GIMPLE_RETURN) {
      if (release == nullptr ||
          gimple_return_retval(as_a<const greturn *>(statement)) != NULL_TREE)
        return result;
      returned = true;
      continue;
    }
    if (is_gimple_call(statement)) {
      tree declaration = gimple_call_fndecl(statement);
      if (release != nullptr || load == nullptr || declaration == NULL_TREE ||
          !fndecl_built_in_p(declaration, BUILT_IN_NORMAL) ||
          DECL_FUNCTION_CODE(declaration) != BUILT_IN_FREE ||
          gimple_call_num_args(statement) != 1 ||
          gimple_call_lhs(statement) != NULL_TREE ||
          !gimple_call_nothrow_p(
              as_a<gcall *>(const_cast<gimple *>(statement))) ||
          gimple_call_fntype(as_a<const gcall *>(statement)) !=
              TREE_TYPE(declaration) ||
          gimple_vuse(statement) != gimple_vuse(load))
        return result;
      unsigned int remaining = max_relative_release_steps;
      if (relative_copied_value(gimple_call_arg(statement, 0), block,
                                &remaining) != gimple_assign_lhs(load))
        return result;
      release = statement;
      continue;
    }
    // After release the sole allowed operation is a normal void return.
    // Before it, all writes (including local/aggregate stores) are rejected,
    // so the only load must observe this function's entry memory state.
    if (release != nullptr || !is_gimple_assign(statement) ||
        gimple_vdef(statement) != NULL_TREE)
      return result;
    if (gimple_assign_single_p(statement) && gimple_assign_load_p(statement) &&
        pointer_value(gimple_assign_lhs(statement)) &&
        TREE_CODE(gimple_assign_lhs(statement)) == SSA_NAME &&
        pointer_value(gimple_assign_rhs1(statement))) {
      tree vuse = gimple_vuse(statement);
      if (load != nullptr || vuse == NULL_TREE || TREE_CODE(vuse) != SSA_NAME ||
          !SSA_NAME_IS_DEFAULT_DEF(vuse))
        return result;
      load = statement;
      continue;
    }
    if (relative_copy_assignment(statement))
      continue;
    if (gimple_assign_rhs_code(statement) == POINTER_PLUS_EXPR &&
        pointer_value(gimple_assign_lhs(statement)) &&
        TREE_CODE(gimple_assign_lhs(statement)) == SSA_NAME &&
        same_pointer_representation(TREE_TYPE(gimple_assign_lhs(statement)),
                                     TREE_TYPE(gimple_assign_rhs1(statement))) &&
        TREE_CODE(gimple_assign_rhs2(statement)) == INTEGER_CST)
      continue;
    return result;
  }
  if (!returned || release == nullptr || load == nullptr)
    return result;
  result.reference = gimple_assign_rhs1(load);
  result.release_location = statement_location(release);
  result.usable = true;
  return result;
}

RelativeAccessSummary build_relative_access_summary(cgraph_node *target) {
  RelativeAccessSummary result;
  RelativeReleaseSummary &signature = result.signature;
  signature.declaration = target->decl;
  signature.function_type = TREE_TYPE(target->decl);
  signature.assembler_name = DECL_ASSEMBLER_NAME(target->decl);
  signature.node_uid = target->get_uid();
  signature.body = target->get_fun();
  if (!relative_original_node(target) || TREE_THIS_VOLATILE(target->decl) ||
      n_basic_blocks_for_fn(signature.body) != 3)
    return result;
  for (tree formal = DECL_ARGUMENTS(target->decl); formal != NULL_TREE;
       formal = DECL_CHAIN(formal)) {
    if (signature.formals.size() >= max_summary_parameters ||
        TREE_CODE(formal) != PARM_DECL || TREE_THIS_VOLATILE(formal))
      return result;
    signature.formals.push_back(formal);
  }
  basic_block block = single_succ_p(ENTRY_BLOCK_PTR_FOR_FN(signature.body))
                          ? single_succ(ENTRY_BLOCK_PTR_FOR_FN(signature.body)) : nullptr;
  const int invalid_edges = EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_EH | EDGE_FAKE;
  if (block == nullptr || block == EXIT_BLOCK_PTR_FOR_FN(signature.body) ||
      !single_pred_p(block) || single_pred(block) != ENTRY_BLOCK_PTR_FOR_FN(signature.body) ||
      !single_succ_p(block) || single_succ(block) != EXIT_BLOCK_PTR_FOR_FN(signature.body) ||
      (single_pred_edge(block)->flags & invalid_edges) != 0 ||
      (single_succ_edge(block)->flags & invalid_edges) != 0 ||
      !gsi_end_p(gsi_start_phis(block)))
    return result;
  const gimple *load = nullptr;
  unsigned int statements = 0;
  for (gimple_stmt_iterator iterator = gsi_start_bb(block);
       !gsi_end_p(iterator); gsi_next(&iterator)) {
    gimple *statement = gsi_stmt(iterator);
    if (++statements > max_relative_release_statements ||
        !analysis_budget::consume(analysis_budget::WorkKind::summary_parameter_cfg) ||
        gimple_has_volatile_ops(statement))
      return result;
    if (is_gimple_debug(statement) || gimple_code(statement) == GIMPLE_LABEL ||
        gimple_code(statement) == GIMPLE_NOP)
      continue;
    if (is_gimple_call(statement)) {
      if (load == nullptr || gimple_vuse(statement) != gimple_vuse(load))
        return result;
      for (unsigned int index = 0; index < gimple_call_num_args(statement); ++index) {
        unsigned int remaining = max_relative_release_steps;
        if (relative_copied_value(gimple_call_arg(statement, index), block, &remaining) !=
            gimple_assign_lhs(load))
          continue;
        const ArgumentEffect effect = argument_effect(statement, index);
        if (!effect.has_access_guarantee())
          continue;
        signature.reference = gimple_assign_rhs1(load);
        signature.usable = true;
        result.access_location = statement_location(statement);
        result.access = effect.access;
        return result;
      }
      return result;
    }
    // No write or call may change the loaded entry value before the access.
    if (!is_gimple_assign(statement) || gimple_vdef(statement) != NULL_TREE)
      return result;
    if (gimple_assign_single_p(statement) && gimple_assign_load_p(statement)) {
      tree reference = gimple_assign_rhs1(statement);
      if (load == nullptr && pointer_value(gimple_assign_lhs(statement)) &&
          TREE_CODE(gimple_assign_lhs(statement)) == SSA_NAME && pointer_value(reference)) {
        tree vuse = gimple_vuse(statement);
        if (vuse == NULL_TREE || TREE_CODE(vuse) != SSA_NAME || !SSA_NAME_IS_DEFAULT_DEF(vuse))
          return result;
        load = statement;
        continue;
      }
      if (load != nullptr && TREE_CODE(reference) == MEM_REF &&
          TREE_CODE(TREE_OPERAND(reference, 1)) == INTEGER_CST &&
          integer_zerop(TREE_OPERAND(reference, 1)) && fixed_object_type(TREE_TYPE(reference)) &&
          TYPE_SIZE(TREE_TYPE(reference)) != NULL_TREE &&
          TREE_CODE(TYPE_SIZE(TREE_TYPE(reference))) == INTEGER_CST &&
          !integer_zerop(TYPE_SIZE(TREE_TYPE(reference)))) {
        unsigned int remaining = max_relative_release_steps;
        if (relative_copied_value(TREE_OPERAND(reference, 0), block, &remaining) ==
            gimple_assign_lhs(load)) {
          signature.reference = gimple_assign_rhs1(load);
          signature.usable = true;
          result.access_location = statement_location(statement);
          result.access = AccessGuarantee::must_access;
          return result;
        }
      }
      return result;
    }
    if (!relative_copy_assignment(statement))
      return result;
  }
  return result;
}

ConditionalBorrowSummary build_conditional_borrow(cgraph_node *target) {
  ConditionalBorrowSummary result;
  RelativeReleaseSummary &signature = result.signature;
  signature.declaration = target->decl;
  signature.function_type = TREE_TYPE(target->decl);
  signature.assembler_name = DECL_ASSEMBLER_NAME(target->decl);
  signature.node_uid = target->get_uid();
  signature.body = target->get_fun();
  if (!relative_original_node(target) || n_basic_blocks_for_fn(signature.body) > 8 ||
      TREE_TYPE(signature.function_type) != void_type_node)
    return result;
  for (tree formal = DECL_ARGUMENTS(target->decl); formal != NULL_TREE;
       formal = DECL_CHAIN(formal)) {
    if (signature.formals.size() >= 8 || TREE_THIS_VOLATILE(formal))
      return result;
    signature.formals.push_back(formal);
  }
  const gimple *guard = nullptr;
  const gimple *release = nullptr;
  std::map<basic_block, unsigned int> indegrees;
  unsigned int statements = 0;
  basic_block block;
  FOR_EACH_BB_FN(block, signature.body)
    indegrees.emplace(block, 0);
  FOR_EACH_BB_FN(block, signature.body) {
    bool returned = false;
    for (gimple_stmt_iterator iterator = gsi_start_bb(block); !gsi_end_p(iterator); gsi_next(&iterator)) {
      const gimple *statement = gsi_stmt(iterator);
      if (++statements > max_relative_release_statements ||
          !analysis_budget::consume(analysis_budget::WorkKind::summary_parameter_cfg) ||
          gimple_has_volatile_ops(statement))
        return result;
      if (is_gimple_debug(statement) || gimple_code(statement) == GIMPLE_LABEL ||
          gimple_code(statement) == GIMPLE_NOP)
        continue;
      if (gimple_code(statement) == GIMPLE_RETURN) {
        if (gimple_return_retval(as_a<const greturn *>(statement)) != NULL_TREE)
          return result;
        returned = true;
      } else if (gimple_code(statement) == GIMPLE_COND) {
        if (guard != nullptr)
          return result;
        guard = statement;
      } else if (is_gimple_call(statement)) {
        tree declaration = gimple_call_fndecl(statement);
        cgraph_node *release_target = declaration == NULL_TREE
                                          ? nullptr : cgraph_node::get(declaration);
        if (release != nullptr || declaration == NULL_TREE ||
            !fndecl_built_in_p(declaration, BUILT_IN_NORMAL) ||
            DECL_FUNCTION_CODE(declaration) != BUILT_IN_FREE ||
            gimple_has_body_p(declaration) ||
            (release_target != nullptr && release_target->definition) ||
            gimple_call_num_args(statement) != 1 || gimple_call_lhs(statement) != NULL_TREE)
          return result;
        release = statement;
      } else if (!relative_copy_assignment(statement)) {
        return result;
      }
    }
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if ((successor->flags & (EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_FAKE)) != 0 ||
          (successor->dest == EXIT_BLOCK_PTR_FOR_FN(signature.body) && !returned))
        return result;
      const auto found = indegrees.find(successor->dest);
      if (found != indegrees.end())
        ++found->second;
    }
    if (EDGE_COUNT(block->succs) == 0 && !returned)
      return result;
  }
  if (guard == nullptr || release == nullptr)
    return result;
  std::vector<basic_block> ready;
  for (const auto &entry : indegrees)
    if (entry.second == 0)
      ready.push_back(entry.first);
  unsigned int removed = 0;
  while (!ready.empty()) {
    block = ready.back(); ready.pop_back(); ++removed;
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      const auto found = indegrees.find(successor->dest);
      if (found != indegrees.end() && --found->second == 0)
        ready.push_back(found->first);
    }
  }
  if (removed != indegrees.size())
    return result;
  unsigned int remaining = max_relative_release_steps;
  tree pointer = relative_copied_value(gimple_call_arg(release, 0), gimple_bb(release), &remaining);
  tree selector = gimple_cond_lhs(guard);
  tree zero = gimple_cond_rhs(guard);
  const auto code = gimple_cond_code(guard);
  if (zero == NULL_TREE || TREE_CODE(zero) != INTEGER_CST || !integer_zerop(zero) ||
      (code != EQ_EXPR && code != NE_EXPR) || selector == NULL_TREE ||
      !INTEGRAL_TYPE_P(TREE_TYPE(selector)))
    return result;
  bool found_pointer = false, found_selector = false;
  for (unsigned int index = 0; index < signature.formals.size(); ++index) {
    tree value = ssa_default_def(signature.body, signature.formals[index]);
    if (pointer == value && pointer_value(value)) {
      result.pointer_index = index;
      found_pointer = true;
    }
    if (selector == value) {
      result.selector_index = index;
      found_selector = true;
    }
  }
  if (!found_pointer || !found_selector || EDGE_COUNT(gimple_bb(guard)->succs) != 2)
    return result;
  edge successor;
  edge_iterator iterator;
  unsigned int borrow_edges = 0;
  FOR_EACH_EDGE(successor, iterator, gimple_bb(guard)->succs) {
    const unsigned int selected = successor->flags & (EDGE_TRUE_VALUE | EDGE_FALSE_VALUE);
    if (selected != EDGE_TRUE_VALUE && selected != EDGE_FALSE_VALUE)
      return result;
    std::vector<basic_block> pending{successor->dest};
    std::set<basic_block> seen;
    bool reaches_release = false, reaches_exit = false;
    while (!pending.empty()) {
      block = pending.back(); pending.pop_back();
      if (!seen.insert(block).second)
        continue;
      if (block == EXIT_BLOCK_PTR_FOR_FN(signature.body)) {
        reaches_exit = true;
        continue;
      }
      reaches_release = reaches_release || block == gimple_bb(release);
      edge next;
      edge_iterator next_iterator;
      FOR_EACH_EDGE(next, next_iterator, block->succs)
        pending.push_back(next->dest);
    }
    if (reaches_exit && !reaches_release) {
      ++borrow_edges;
      result.when_zero = (code == EQ_EXPR) == (selected == EDGE_TRUE_VALUE);
    }
  }
  if (borrow_edges == 1) {
    signature.usable = true;
    result.guard_location = statement_location(guard);
  }
  return result;
}

// Rebuild only the reference/address spine.  FIELD_DECLs, MEM_REF offset
// types, array bounds, alias sets and qualifiers are retained from GCC's
// original reference; no synthetic statement or SSA name enters the IR.
class RelativeReferenceInstantiator {
 public:
  RelativeReferenceInstantiator(const RelativeReleaseSummary &summary,
                                const gimple *call, cgraph_node *caller)
      : summary_(summary), call_(call), caller_(caller) {}

  tree reference(tree value) {
    if (value == NULL_TREE || !relative_step(&remaining_) ||
        !fixed_object_type(TREE_TYPE(value)) || TREE_THIS_VOLATILE(value))
      return NULL_TREE;
    if (TREE_CODE(value) == PARM_DECL) {
      if (!AGGREGATE_TYPE_P(TREE_TYPE(value)))
        return NULL_TREE;
      tree actual = formal_actual(value);
      // SSA aggregates and constructor arguments do not identify a caller
      // memory object.  Reject them instead of inventing a temporary store.
      return actual != NULL_TREE &&
                     (TREE_CODE(actual) == VAR_DECL ||
                      TREE_CODE(actual) == PARM_DECL ||
                      TREE_CODE(actual) == COMPONENT_REF ||
                      TREE_CODE(actual) == ARRAY_REF ||
                      TREE_CODE(actual) == MEM_REF) &&
                     !TREE_THIS_VOLATILE(actual)
                 ? actual : NULL_TREE;
    }
    if (TREE_CODE(value) == VAR_DECL) {
      varpool_node *global = varpool_node::get(value);
      if (global == nullptr || global->decl != value || global->alias ||
          (!TREE_STATIC(value) && !DECL_EXTERNAL(value)) || DECL_WEAK(value) ||
          DECL_THREAD_LOCAL_P(value) ||
          !global->binds_to_current_def_p(caller_) ||
          global->get_availability(caller_) < AVAIL_AVAILABLE)
        return NULL_TREE;
      return value;
    }
    tree base = NULL_TREE;
    switch (TREE_CODE(value)) {
      case MEM_REF:
        if (TREE_CODE(TREE_OPERAND(value, 1)) != INTEGER_CST)
          return NULL_TREE;
        base = address(TREE_OPERAND(value, 0));
        break;
      case COMPONENT_REF: {
        tree field = TREE_OPERAND(value, 1);
        if (TREE_CODE(field) != FIELD_DECL || DECL_BIT_FIELD(field) ||
            !constant_or_null(TREE_OPERAND(value, 2)))
          return NULL_TREE;
        base = reference(TREE_OPERAND(value, 0));
        break;
      }
      case ARRAY_REF:
        if (TREE_CODE(TREE_OPERAND(value, 1)) != INTEGER_CST ||
            !constant_or_null(TREE_OPERAND(value, 2)) ||
            !constant_or_null(TREE_OPERAND(value, 3)))
          return NULL_TREE;
        base = reference(TREE_OPERAND(value, 0));
        break;
      default:
        return NULL_TREE;
    }
    if (base == NULL_TREE)
      return NULL_TREE;
    tree result = copy_node(value);
    TREE_OPERAND(result, 0) = base;
    return result;
  }

 private:
  static bool constant_or_null(tree value) {
    return value == NULL_TREE || TREE_CODE(value) == INTEGER_CST;
  }

  tree formal_actual(tree formal) const {
    for (unsigned int index = 0; index < summary_.formals.size(); ++index)
      if (summary_.formals[index] == formal)
        return gimple_call_arg(call_, index);
    return NULL_TREE;
  }

  tree address(tree value) {
    if (!pointer_value(value) || !relative_step(&remaining_) ||
        TREE_THIS_VOLATILE(value))
      return NULL_TREE;
    if (TREE_CODE(value) == ADDR_EXPR) {
      tree object = reference(TREE_OPERAND(value, 0));
      if (object == NULL_TREE)
        return NULL_TREE;
      tree result = copy_node(value);
      TREE_OPERAND(result, 0) = object;
      return result;
    }
    if (TREE_CODE(value) != SSA_NAME)
      return NULL_TREE;
    if (SSA_NAME_IS_DEFAULT_DEF(value)) {
      tree formal = SSA_NAME_VAR(value);
      return formal != NULL_TREE && TREE_CODE(formal) == PARM_DECL &&
                     TREE_TYPE(formal) == TREE_TYPE(value)
                 ? formal_actual(formal) : NULL_TREE;
    }
    const gimple *definition = SSA_NAME_DEF_STMT(value);
    if (definition == nullptr || !is_gimple_assign(definition))
      return NULL_TREE;
    if (relative_copy_assignment(definition)) {
      tree source = address(gimple_assign_rhs1(definition));
      return source == NULL_TREE ? NULL_TREE
                                 : fold_convert(TREE_TYPE(value), source);
    }
    if (gimple_assign_rhs_code(definition) == POINTER_PLUS_EXPR &&
        same_pointer_representation(TREE_TYPE(value),
                                     TREE_TYPE(gimple_assign_rhs1(definition))) &&
        TREE_CODE(gimple_assign_rhs2(definition)) == INTEGER_CST) {
      tree source = address(gimple_assign_rhs1(definition));
      return source == NULL_TREE ? NULL_TREE :
          fold_build2(POINTER_PLUS_EXPR, TREE_TYPE(value), source,
                      gimple_assign_rhs2(definition));
    }
    return NULL_TREE;
  }

  const RelativeReleaseSummary &summary_;
  const gimple *call_;
  cgraph_node *caller_;
  unsigned int remaining_ = max_relative_release_steps;
};

// Invocation specialization is a one-way refinement: prove one exact free
// on a fully determined, terminating trace.  It never publishes a new borrow,
// access or return theorem, and is not part of the context-free fixed point.
class InvocationFreeProof {
 public:
  InvocationFreeProof(const gimple *call, cgraph_node *caller,
                      const RelativeReleaseSummary &signature,
                      unsigned int argument)
      : call_(call), caller_(caller), signature_(signature) {
    tree formal = signature_.formals[argument];
    tree value = ssa_default_def(signature_.body, formal);
    if (pointer_value(value))
      tracked_.insert(value);
  }

  bool prove(ReleaseOrigin *origin) {
    *origin = ReleaseOrigin{};
    if (tracked_.empty() || TREE_THIS_VOLATILE(signature_.declaration))
      return false;
    basic_block entry = ENTRY_BLOCK_PTR_FOR_FN(signature_.body);
    if (!single_succ_p(entry))
      return false;
    edge incoming = single_succ_edge(entry);
    std::set<basic_block> visited;
    while (incoming != nullptr) {
      if (!normal_edge(incoming) || visited.size() >= 32 ||
          !relative_step(&remaining_work_))
        return false;
      basic_block block = incoming->dest;
      if (block == EXIT_BLOCK_PTR_FOR_FN(signature_.body) ||
          !visited.insert(block).second || !interpret_phis(block, incoming))
        return false;
      edge selected = nullptr;
      for (gimple_stmt_iterator iterator = gsi_start_bb(block);
           !gsi_end_p(iterator); gsi_next(&iterator)) {
        const gimple *statement = gsi_stmt(iterator);
        if (++statements_ > 64 || !relative_step(&remaining_work_))
          return false;
        if (is_gimple_debug(statement) || gimple_code(statement) == GIMPLE_LABEL ||
            gimple_code(statement) == GIMPLE_NOP)
          continue;
        if (selected != nullptr || gimple_has_volatile_ops(statement))
          return false;
        if (gimple_code(statement) == GIMPLE_RETURN) {
          const bool completed = frees_ == 1 &&
                 gimple_return_retval(as_a<const greturn *>(statement)) == NULL_TREE &&
                 single_succ_p(block) && normal_edge(single_succ_edge(block)) &&
                 single_succ(block) == EXIT_BLOCK_PTR_FOR_FN(signature_.body);
          // A free seen on a prefix is not evidence until this selected
          // trace has passed every guard and reached its normal return.
          if (completed)
            origin->location = release_location_;
          return completed;
        }
        if (gimple_code(statement) == GIMPLE_COND) {
          const gcond *condition = as_a<const gcond *>(statement);
          tree left = callee_constant(gimple_cond_lhs(condition));
          tree right = callee_constant(gimple_cond_rhs(condition));
          if (left == NULL_TREE || right == NULL_TREE)
            return false;
          tree folded = fold_binary(gimple_cond_code(condition),
                                     boolean_type_node, left, right);
          if (!integer_constant(folded))
            return false;
          const int wanted = integer_zerop(folded) ? EDGE_FALSE_VALUE : EDGE_TRUE_VALUE;
          edge successor;
          edge_iterator edge_iterator;
          FOR_EACH_EDGE(successor, edge_iterator, block->succs) {
            if ((successor->flags & (EDGE_TRUE_VALUE | EDGE_FALSE_VALUE)) == wanted) {
              if (selected != nullptr || !normal_edge(successor))
                return false;
              selected = successor;
            }
          }
          if (selected == nullptr)
            return false;
          continue;
        }
        if (is_gimple_call(statement)) {
          tree declaration = gimple_call_fndecl(statement);
          if (++frees_ != 1 || declaration == NULL_TREE ||
              !fndecl_built_in_p(declaration, BUILT_IN_NORMAL) ||
              DECL_FUNCTION_CODE(declaration) != BUILT_IN_FREE ||
              gimple_call_num_args(statement) != 1 ||
              gimple_call_lhs(statement) != NULL_TREE ||
              (gimple_call_flags(statement) &
               (ECF_RETURNS_TWICE | ECF_NORETURN)) != 0 ||
              !gimple_call_nothrow_p(
                  as_a<gcall *>(const_cast<gimple *>(statement))) ||
              gimple_call_fntype(as_a<const gcall *>(statement)) != TREE_TYPE(declaration) ||
              tracked_.count(gimple_call_arg(statement, 0)) == 0)
            return false;
          release_location_ = statement_location(statement);
          continue;
        }
        if (!interpret_assignment(statement))
          return false;
      }
      if (selected != nullptr) {
        incoming = selected;
      } else {
        if (!single_succ_p(block))
          return false;
        incoming = single_succ_edge(block);
      }
    }
    return false;
  }

 private:
  static bool normal_edge(edge edge) {
    return edge != nullptr &&
           (edge->flags & (EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_FAKE)) == 0;
  }

  static bool integer_constant(tree value) {
    return value != NULL_TREE && TREE_CODE(value) == INTEGER_CST &&
           INTEGRAL_TYPE_P(TREE_TYPE(value)) && !TREE_OVERFLOW(value);
  }

  static tree converted_constant(tree type, tree value) {
    if (type == NULL_TREE || !INTEGRAL_TYPE_P(type) || !integer_constant(value))
      return NULL_TREE;
    tree converted = fold_convert(type, value);
    return integer_constant(converted) ? converted : NULL_TREE;
  }

  // A caller SSA load is evaluated at ITS OWN VUSE.  Re-reading its reference
  // at the later invocation would silently change the scalar actual's value
  // after an intervening store.  Default SSA, PHIs and non-copy arithmetic
  // remain unknown in this initial bounded constant domain.
  tree caller_constant(tree value, unsigned int depth) {
    if (value == NULL_TREE || depth == 0 || !relative_step(&remaining_work_))
      return NULL_TREE;
    if (integer_constant(value))
      return value;
    if (TREE_CODE(value) != SSA_NAME || SSA_NAME_IS_DEFAULT_DEF(value) ||
        !INTEGRAL_TYPE_P(TREE_TYPE(value)))
      return NULL_TREE;
    const gimple *definition = SSA_NAME_DEF_STMT(value);
    if (definition == nullptr || !is_gimple_assign(definition) ||
        gimple_vdef(definition) != NULL_TREE || gimple_has_volatile_ops(definition))
      return NULL_TREE;
    tree rhs = gimple_assign_rhs1(definition);
    if (gimple_assign_single_p(definition) && gimple_assign_load_p(definition)) {
      const MemoryValueResult loaded = resolve_integral_reference(
          rhs, gimple_vuse(definition), memory_limits());
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::summary_parameter_cfg,
              1ul + loaded.visited_states + loaded.walked_vdefs))
        return NULL_TREE;
      rhs = loaded.unique_stored_value();
    } else if (!gimple_assign_single_p(definition) &&
               !gimple_assign_cast_p(definition)) {
      return NULL_TREE;
    }
    return converted_constant(TREE_TYPE(value), caller_constant(rhs, depth - 1));
  }

  static MemoryWalkLimits memory_limits() {
    MemoryWalkLimits limits;
    limits.max_depth = 24;
    limits.max_states = 64;
    return limits;
  }

  tree callee_constant(tree value) {
    if (value == NULL_TREE || !relative_step(&remaining_work_))
      return NULL_TREE;
    if (integer_constant(value))
      return value;
    if (TREE_CODE(value) != SSA_NAME || !INTEGRAL_TYPE_P(TREE_TYPE(value)))
      return NULL_TREE;
    const auto found = constants_.find(value);
    if (found != constants_.end())
      return found->second;
    if (!SSA_NAME_IS_DEFAULT_DEF(value))
      return NULL_TREE;
    tree formal = SSA_NAME_VAR(value);
    for (unsigned int index = 0; index < signature_.formals.size(); ++index) {
      if (formal != signature_.formals[index])
        continue;
      tree result = converted_constant(TREE_TYPE(value),
          caller_constant(gimple_call_arg(call_, index), 24));
      constants_[value] = result;
      return result;
    }
    return NULL_TREE;
  }

  tree entry_global_constant(const gimple *load) {
    tree reference = gimple_assign_rhs1(load);
    tree vuse = gimple_vuse(load);
    if (reference == NULL_TREE || TREE_CODE(reference) != VAR_DECL ||
        vuse == NULL_TREE || TREE_CODE(vuse) != SSA_NAME ||
        !SSA_NAME_IS_DEFAULT_DEF(vuse) || TREE_THIS_VOLATILE(reference) ||
        !fixed_object_type(TREE_TYPE(reference)) || DECL_WEAK(reference) ||
        DECL_THREAD_LOCAL_P(reference) ||
        (!TREE_STATIC(reference) && !DECL_EXTERNAL(reference)))
      return NULL_TREE;
    varpool_node *global = varpool_node::get(reference);
    if (global == nullptr || global->decl != reference || global->alias ||
        !global->binds_to_current_def_p(caller_) ||
        global->get_availability(caller_) < AVAIL_AVAILABLE)
      return NULL_TREE;
    const MemoryValueResult loaded = resolve_integral_reference(
        reference, gimple_vuse(call_), memory_limits());
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_parameter_cfg,
            1ul + loaded.visited_states + loaded.walked_vdefs))
      return NULL_TREE;
    return caller_constant(loaded.unique_stored_value(), 24);
  }

  bool interpret_assignment(const gimple *statement) {
    if (!is_gimple_assign(statement) || gimple_vdef(statement) != NULL_TREE)
      return false;
    tree lhs = gimple_assign_lhs(statement);
    if (lhs == NULL_TREE || TREE_CODE(lhs) != SSA_NAME)
      return false;
    if (pointer_value(lhs)) {
      if (!relative_copy_assignment(statement))
        return false;
      if (tracked_.count(gimple_assign_rhs1(statement)) != 0)
        tracked_.insert(lhs);
      return true;
    }
    if (!INTEGRAL_TYPE_P(TREE_TYPE(lhs)))
      return false;
    tree constant = NULL_TREE;
    if (gimple_assign_single_p(statement) && gimple_assign_load_p(statement)) {
      constant = entry_global_constant(statement);
      // Loads not proved to be from immutable entry memory are outside the
      // model even if the eventual branch does not happen to use the value.
      if (constant == NULL_TREE)
        return false;
    } else if (gimple_assign_single_p(statement) ||
               gimple_assign_cast_p(statement)) {
      tree rhs = gimple_assign_rhs1(statement);
      if (rhs == NULL_TREE || !INTEGRAL_TYPE_P(TREE_TYPE(rhs)))
        return false;
      constant = callee_constant(rhs);
    } else {
      return false;
    }
    constants_[lhs] = converted_constant(TREE_TYPE(lhs), constant);
    return true;
  }

  bool interpret_phis(basic_block block, edge incoming) {
    for (gphi_iterator iterator = gsi_start_phis(block);
         !gsi_end_p(iterator); gsi_next(&iterator)) {
      if (++statements_ > 64 || !relative_step(&remaining_work_))
        return false;
      const gphi *phi = iterator.phi();
      tree result = gimple_phi_result(phi);
      if (virtual_operand_p(result))
        continue;
      tree selected = NULL_TREE;
      for (unsigned int index = 0; index < gimple_phi_num_args(phi); ++index) {
        if (gimple_phi_arg_edge(phi, index) == incoming) {
          selected = gimple_phi_arg_def(phi, index);
          break;
        }
      }
      if (selected == NULL_TREE)
        return false;
      if (pointer_value(result)) {
        if (!pointer_value(selected) ||
            !same_pointer_representation(TREE_TYPE(result), TREE_TYPE(selected)))
          return false;
        if (tracked_.count(selected) != 0)
          tracked_.insert(result);
      } else if (INTEGRAL_TYPE_P(TREE_TYPE(result))) {
        constants_[result] = converted_constant(TREE_TYPE(result),
                                                callee_constant(selected));
      } else {
        return false;
      }
    }
    return true;
  }

  const gimple *call_;
  cgraph_node *caller_;
  const RelativeReleaseSummary &signature_;
  std::set<tree> tracked_;
  std::map<tree, tree> constants_;
  unsigned int frees_ = 0;
  location_t release_location_ = UNKNOWN_LOCATION;
  unsigned int statements_ = 0;
  unsigned int remaining_work_ = 256;
};

bool invocation_definitely_frees(const gimple *call, unsigned int argument,
                                 ReleaseOrigin *origin) {
  *origin = ReleaseOrigin{};
  if (!summaries_built || generating_provider_facts || cfun == nullptr ||
      cfun->decl == NULL_TREE || !analysis_budget::available())
    return false;
  cgraph_node *caller = cgraph_node::get(cfun->decl);
  cgraph_node *target = bound_target(call, caller);
  if (!relative_original_node(target))
    return false;
  cgraph_edge *edge = caller->get_edge(const_cast<gimple *>(call));
  if (edge == nullptr || edge->callee != target ||
      gimple_call_fndecl(call) != target->decl)
    return false;
  const InvocationEffectKey key{call, gimple_vuse(call), target->get_uid(), argument};
  const auto cached = invocation_frees.find(key);
  if (cached != invocation_frees.end()) {
    *origin = cached->second.origin;
    return cached->second.proved;
  }
  if (invocation_frees.size() >= 4096)
    return false;

  RelativeReleaseSummary signature;
  signature.declaration = target->decl;
  signature.function_type = TREE_TYPE(target->decl);
  signature.assembler_name = DECL_ASSEMBLER_NAME(target->decl);
  signature.node_uid = target->get_uid();
  signature.body = target->get_fun();
  for (tree formal = DECL_ARGUMENTS(target->decl); formal != NULL_TREE;
       formal = DECL_CHAIN(formal)) {
    if (signature.formals.size() >= max_summary_parameters ||
        TREE_CODE(formal) != PARM_DECL || TREE_THIS_VOLATILE(formal))
      return false;
    signature.formals.push_back(formal);
  }
  if (argument >= signature.formals.size() ||
      !pointer_value(signature.formals[argument]) ||
      !relative_signature_matches(call, target, signature, true))
    return false;
  InvocationFreeProof proof(call, caller, signature, argument);
  InvocationFreeResult result;
  result.proved = proof.prove(&result.origin);
  invocation_frees.emplace(key, result);
  *origin = result.origin;
  if (dump_file != nullptr && (dump_flags & TDF_DETAILS))
    dump_printf(MSG_NOTE,
                "ownership-invocation: function=%s argument=%u "
                "definitely-freed=%s origin=body normal-return-required=yes\n",
                IDENTIFIER_POINTER(signature.assembler_name), argument,
                result.proved ? "yes" : "no");
  return result.proved;
}

bool provider_integer(tree value, unsigned HOST_WIDE_INT maximum,
                      unsigned int *result) {
  if (value == NULL_TREE || TREE_CODE(value) != INTEGER_CST ||
      !tree_fits_uhwi_p(value) || tree_to_uhwi(value) > maximum)
    return false;
  *result = static_cast<unsigned int>(tree_to_uhwi(value));
  return true;
}

bool provider_signature_matches(tree declaration,
                                const FunctionSummary &summary) {
  return declaration != NULL_TREE &&
         summary.function_type == TREE_TYPE(declaration) &&
         DECL_ASSEMBLER_NAME_SET_P(declaration) &&
         summary.assembler_name == DECL_ASSEMBLER_NAME(declaration);
}

// TREE_VEC is not legal in a declaration attribute's streamed SCC: GCC's
// lto-common.cc mentions_vars_p reaches gcc_unreachable for it. Use ordinary
// TREE_LIST nodes, which its declaration fixup/merger explicitly supports.
bool decode_provider_list(tree list, unsigned int count,
                          std::vector<tree> *values) {
  for (unsigned int index = 0; index < count; ++index) {
    if (list == NULL_TREE || TREE_CODE(list) != TREE_LIST ||
        TREE_PURPOSE(list) != NULL_TREE)
      return false;
    values->push_back(TREE_VALUE(list));
    list = TREE_CHAIN(list);
  }
  return list == NULL_TREE;
}

tree encode_provider_list(const std::vector<tree> &values) {
  tree result = NULL_TREE;
  for (auto iterator = values.rbegin(); iterator != values.rend(); ++iterator)
    result = tree_cons(NULL_TREE, *iterator, result);
  return result;
}

bool decode_provider_summary(tree declaration, tree payload,
                             FunctionSummary *summary) {
  std::vector<tree> header;
  if (!decode_provider_list(payload, provider_payload_length, &header))
    return false;
  unsigned int version, count, normal_return, flags;
  if (!provider_integer(header[0], provider_schema_version,
                        &version) || version != provider_schema_version ||
      !provider_integer(header[3], max_summary_parameters,
                        &count) ||
      !provider_integer(header[5], 1, &normal_return) ||
      normal_return != 1 ||
      !provider_integer(header[6], 0, &flags))
    return false;
  summary->declaration = declaration;
  summary->function_type = header[1];
  summary->assembler_name = header[2];
  if (summary->assembler_name == NULL_TREE ||
      TREE_CODE(summary->assembler_name) != IDENTIFIER_NODE ||
      !provider_signature_matches(declaration, *summary))
    return false;
  std::vector<tree> formal_types;
  if (!complete_formal_types(summary->function_type, &formal_types) ||
      formal_types.size() != count)
    return false;
  std::vector<tree> parameters;
  if (!decode_provider_list(header[4], count, &parameters))
    return false;
  for (unsigned int index = 0; index < count; ++index) {
    std::vector<tree> item;
    if (!decode_provider_list(parameters[index], 4, &item))
      return false;
    ParameterSummary parameter;
    parameter.formal_type = item[0];
    if (parameter.formal_type == NULL_TREE || !TYPE_P(parameter.formal_type) ||
        TYPE_MAIN_VARIANT(parameter.formal_type) !=
            TYPE_MAIN_VARIANT(formal_types[index]))
      return false;
    unsigned int may_dereference, access;
    if (!provider_integer(item[1],
                          fate_live | fate_freed | fate_escaped | fate_unknown,
                          &parameter.fates) || parameter.fates == fate_none ||
        !provider_integer(item[2], 1, &may_dereference) ||
        !provider_integer(item[3],
                          static_cast<unsigned int>(
                              AccessGuarantee::requires_live_if_nonnull),
                          &access))
      return false;
    parameter.pointer = POINTER_TYPE_P(parameter.formal_type);
    parameter.may_dereference = may_dereference != 0;
    parameter.access = static_cast<AccessGuarantee>(access);
    if ((!parameter.pointer &&
         (parameter.fates != fate_live || parameter.may_dereference ||
          parameter.access != AccessGuarantee::none)) ||
        (parameter.access != AccessGuarantee::none &&
         !parameter.may_dereference))
      return false;
    summary->parameters.push_back(parameter);
  }
  summary->analyzed = true;
  summary->provider_imported = true;
  summary->has_normal_return = true;
  // No allocation/return-identity theorem is included in schema version 2.
  summary->returns.push_back({ReturnKind::unknown, 0, nullptr, false});
  return true;
}

const FunctionSummary *provider_summary_for_call(const gimple *statement) {
  if (generating_provider_facts)
    return nullptr;
  cgraph_node *caller = cfun != nullptr && cfun->decl != NULL_TREE
                            ? cgraph_node::get(cfun->decl) : nullptr;
  cgraph_node *target = bound_target_identity(statement, caller);
  // An available actual body always wins, including an unsupported body.
  // Never resurrect an early theorem for a transformed or rejected body.
  if (target == nullptr || target->get_fun() != nullptr)
    return nullptr;
  tree payload = provider_transport::lookup(target->decl);
  if (payload == NULL_TREE)
    return nullptr;
  auto found = provider_summaries.find(target->decl);
  if (found == provider_summaries.end()) {
    FunctionSummary imported;
    if (!decode_provider_summary(target->decl, payload, &imported)) {
      ++statistics.provider_rejected;
      return nullptr;
    }
    found = provider_summaries.emplace(target->decl, std::move(imported)).first;
    ++statistics.provider_imported;
    if (dump_file != nullptr && (dump_flags & TDF_DETAILS)) {
      dump_printf(MSG_NOTE,
                  "ownership-provider: imported=%s origin=provider-body "
                  "parameters=%u return-theorem=unknown\n",
                  IDENTIFIER_POINTER(found->second.assembler_name),
                  static_cast<unsigned int>(found->second.parameters.size()));
      for (unsigned int index = 0; index < found->second.parameters.size();
           ++index) {
        const ParameterSummary &parameter = found->second.parameters[index];
        if (parameter.pointer)
          dump_printf(MSG_NOTE,
                      "  param=%u fates=%s may-dereference=%s access=%s\n",
                      index, fate_set_name(parameter.fates),
                      parameter.may_dereference ? "yes" : "no",
                      access_guarantee_name(parameter.access));
      }
    }
  }
  if (!provider_signature_matches(target->decl, found->second) ||
      gimple_call_fntype(as_a<const gcall *>(statement)) !=
          found->second.function_type ||
      !call_matches_formals(statement, found->second)) {
    ++statistics.provider_rejected;
    return nullptr;
  }
  return &found->second;
}

class FunctionContext {
 public:
  explicit FunctionContext(const FunctionSummary &prototype)
      : prototype_(prototype) {
    for (unsigned int index = 0; index < prototype_.parameters.size();
         ++index) {
      tree definition = prototype_.parameters[index].default_definition;
      if (definition != NULL_TREE)
        parameter_definitions_[definition] = index;
    }
  }

  Meaning evaluate(tree value) {
    const auto cached = meaning_cache_.find(value);
    if (cached != meaning_cache_.end())
      return cached->second;
    std::set<tree> active;
    const Meaning result = evaluate_impl(value, 64, false, &active);
    meaning_cache_[value] = result;
    return result;
  }

  bool exact_parameter(const Meaning &meaning, unsigned int index,
                       bool permit_derived = false) const {
    if (!meaning.complete || meaning.alternatives.empty())
      return false;
    for (const ReturnAlternative &alternative : meaning.alternatives)
      if (alternative.kind != ReturnKind::parameter ||
          alternative.parameter_index != index ||
          (!permit_derived && alternative.derived))
        return false;
    return true;
  }

  bool may_reference_parameter(const Meaning &meaning,
                               unsigned int index) const {
    for (const ReturnAlternative &alternative : meaning.alternatives)
      if (alternative.kind == ReturnKind::parameter &&
          alternative.parameter_index == index)
        return true;
    return false;
  }

 private:
  Meaning evaluate_call(const gimple *statement, unsigned int depth,
                        bool derived, std::set<tree> *active) {
    // Prefer a proved body-bound summary over GCC's propagated malloc bit so
    // report evidence can retain the allocator inside a wrapper chain.  The
    // caller call still becomes the local SSA/allocation identity below.
    const FunctionSummary *callee = summary_for_call(statement);
    if (callee == nullptr && ownership_allocation_call_p(statement)) {
      Meaning result;
      append_alternative(
          &result, {derived ? ReturnKind::fresh_shared
                            : ReturnKind::fresh_owned,
                    0, statement, derived, statement_location(statement)});
      return result;
    }

    if (callee == nullptr && ownership_reallocation_call_p(statement)) {
      if (gimple_call_num_args(statement) == 0)
        return unknown_meaning();
      Meaning argument = evaluate_impl(gimple_call_arg(statement, 0),
                                       depth - 1, false, active);
      for (unsigned int index = 0; index < prototype_.parameters.size();
           ++index) {
        if (exact_parameter(argument, index)) {
          Meaning result;
          append_alternative(
              &result, {ReturnKind::realloc_parameter, index, statement,
                        derived});
          return result;
        }
      }
      return unknown_meaning();
    }

    if (callee == nullptr) {
      tree returned = returned_argument(statement);
      return returned == NULL_TREE
                 ? unknown_meaning()
                 : evaluate_impl(returned, depth - 1, derived, active);
    }

    Meaning result;
    for (const ReturnAlternative &alternative : callee->returns) {
      switch (alternative.kind) {
        case ReturnKind::null_value:
          append_alternative(
              &result, {ReturnKind::null_value, 0, nullptr, derived});
          break;
        case ReturnKind::fresh_owned:
          // A recorded allocation origin is not proof that its object is
          // still live at the callee's return.  Do not resurrect a released
          // inner object when composing an outer wrapper's return meaning.
          if (!callee->returns_fresh_owned) {
            append_meaning(&result, unknown_meaning());
            break;
          }
          append_alternative(
              &result, {derived ? ReturnKind::fresh_shared
                                : ReturnKind::fresh_owned,
                        0, statement, derived,
                        alternative.source_location});
          break;
        case ReturnKind::parameter:
          if (alternative.parameter_index >=
              gimple_call_num_args(statement)) {
            append_meaning(&result, unknown_meaning());
          } else {
            append_meaning(
                &result,
                evaluate_impl(gimple_call_arg(statement,
                                               alternative.parameter_index),
                              depth - 1,
                              derived || alternative.derived, active));
          }
          break;
        case ReturnKind::realloc_parameter: {
          if (alternative.parameter_index >=
              gimple_call_num_args(statement)) {
            append_meaning(&result, unknown_meaning());
            break;
          }
          Meaning argument = evaluate_impl(
              gimple_call_arg(statement, alternative.parameter_index),
              depth - 1, false, active);
          bool mapped = false;
          for (unsigned int index = 0; index < prototype_.parameters.size();
               ++index) {
            if (!exact_parameter(argument, index))
              continue;
            append_alternative(
                &result,
                {ReturnKind::realloc_parameter, index, statement, derived});
            mapped = true;
            break;
          }
          if (!mapped)
            append_meaning(&result, unknown_meaning());
          break;
        }
        case ReturnKind::global_object:
          append_alternative(
              &result,
              {ReturnKind::global_object, 0, nullptr, derived});
          break;
        case ReturnKind::fresh_shared:
          append_alternative(
              &result,
              {ReturnKind::fresh_shared, 0, statement, true,
               alternative.source_location});
          break;
        case ReturnKind::unknown:
          append_meaning(&result, unknown_meaning());
          break;
      }
    }
    if (callee->returns.empty())
      append_meaning(&result, unknown_meaning());
    return result;
  }

  Meaning evaluate_impl(tree value, unsigned int depth, bool derived,
                        std::set<tree> *active) {
    if (value == NULL_TREE || depth == 0)
      return unknown_meaning();

    if (TREE_CODE(value) == INTEGER_CST && integer_zerop(value)) {
      Meaning result;
      append_alternative(
          &result, {ReturnKind::null_value, 0, nullptr, derived});
      return result;
    }

    if (TREE_CODE(value) == ADDR_EXPR) {
      tree object = TREE_OPERAND(value, 0);
      tree base = get_base_address(object);
      if (base != NULL_TREE && DECL_P(base) &&
          !auto_var_in_fn_p(base, cfun->decl)) {
        Meaning result;
        append_alternative(
            &result, {ReturnKind::global_object, 0, nullptr, derived});
        return result;
      }
      return unknown_meaning();
    }

    if (TREE_CODE(value) != SSA_NAME)
      return unknown_meaning();

    const auto parameter = parameter_definitions_.find(value);
    if (parameter != parameter_definitions_.end()) {
      Meaning result;
      append_alternative(
          &result,
          {ReturnKind::parameter, parameter->second, nullptr, derived});
      return result;
    }

    if (!active->insert(value).second)
      return unknown_meaning();

    const gimple *definition = SSA_NAME_DEF_STMT(value);
    Meaning result;
    if (definition == nullptr || gimple_code(definition) == GIMPLE_NOP) {
      result = unknown_meaning();
    } else if (gimple_code(definition) == GIMPLE_PHI) {
      const gphi *phi = as_a<const gphi *>(definition);
      if (gimple_phi_num_args(phi) == 0)
        result = unknown_meaning();
      for (unsigned int index = 0; index < gimple_phi_num_args(phi);
           ++index)
        append_meaning(&result,
                       evaluate_impl(gimple_phi_arg_def(phi, index),
                                     depth - 1, derived, active));
    } else if (is_gimple_call(definition)) {
      result = evaluate_call(definition, depth, derived, active);
    } else if (is_gimple_assign(definition)) {
      const enum tree_code code = gimple_assign_rhs_code(definition);
      tree rhs1 = gimple_assign_rhs1(definition);
      if (gimple_assign_load_p(definition)) {
        result = unknown_meaning();
      } else if (code == SSA_NAME || pointer_preserving_cast_p(definition) ||
                 code == ADDR_EXPR) {
        result = evaluate_impl(rhs1, depth - 1, derived, active);
      } else if (code == POINTER_PLUS_EXPR) {
        result = evaluate_impl(rhs1, depth - 1, true, active);
      } else if (code == COND_EXPR) {
        append_meaning(&result,
                       evaluate_impl(gimple_assign_rhs2(definition),
                                     depth - 1, derived, active));
        append_meaning(&result,
                       evaluate_impl(gimple_assign_rhs3(definition),
                                     depth - 1, derived, active));
      } else if (rhs1 != NULL_TREE && TREE_CODE(rhs1) == INTEGER_CST &&
                 integer_zerop(rhs1)) {
        result = evaluate_impl(rhs1, depth - 1, derived, active);
      } else {
        result = unknown_meaning();
      }
    } else {
      result = unknown_meaning();
    }
    active->erase(value);
    return result;
  }

  const FunctionSummary &prototype_;
  std::map<tree, unsigned int> parameter_definitions_;
  std::map<tree, Meaning> meaning_cache_;
};

unsigned int apply_known_effect(unsigned int state, unsigned int effect) {
  unsigned int result = fate_none;
  if ((state & fate_live) != 0)
    result |= effect;
  // FREED is terminal for ownership.  A later no-op/escape call cannot make
  // the object live again; a later free is still a freed outcome.
  if ((state & fate_freed) != 0)
    result |= fate_freed;
  // An escaped pointer is still locally available.  A subsequent proven
  // free can consume it; otherwise it remains escaped.  This preserves the
  // useful may-free fact for "publish, then free" wrappers without claiming
  // that an earlier escape was itself a deallocation.
  if ((state & fate_escaped) != 0) {
    if ((effect & fate_freed) != 0)
      result |= fate_freed;
    if ((effect & (fate_live | fate_escaped)) != 0)
      result |= fate_escaped;
    if ((effect & fate_unknown) != 0)
      result |= fate_unknown;
  }
  if ((state & fate_unknown) != 0)
    result |= fate_unknown;
  return result;
}

unsigned int apply_relation(unsigned int state, bool exact, bool possible,
                            unsigned int effect) {
  if (!possible)
    return state;
  const unsigned int transformed = apply_known_effect(state, effect);
  return exact ? transformed : state | transformed;
}

unsigned int transfer_parameter(FunctionContext *context,
                                unsigned int parameter_index,
                                gimple *statement, unsigned int state) {
  if (is_gimple_call(statement)) {
    if ((gimple_call_flags(statement) & ECF_RETURNS_TWICE) != 0)
      return fate_unknown;

    for (unsigned int argument_index = 0;
         argument_index < gimple_call_num_args(statement);
         ++argument_index) {
      Meaning actual = context->evaluate(
          gimple_call_arg(statement, argument_index));
      const bool exact =
          context->exact_parameter(actual, parameter_index);
      const bool possible =
          context->may_reference_parameter(actual, parameter_index);
      if (!possible) {
        // The early producer has no caller-specific alias facts: another
        // formal, or a pointer loaded from memory, may equal this parameter.
        // A missing SSA provenance witness must not become a no-free proof.
        if (generating_provider_facts &&
            pointer_value(gimple_call_arg(statement, argument_index))) {
          const ArgumentEffect other =
              argument_effect(statement, argument_index);
          if (!other.resolved || other.fates != fate_live)
            state = apply_known_effect(state, fate_unknown);
        }
        continue;
      }

      unsigned int effect = fate_unknown;
      if (ownership_deallocation_call_p(statement) && argument_index == 0) {
        effect = fate_freed;
      } else if (ownership_reallocation_call_p(statement) &&
                 argument_index == 0) {
        effect = fate_live | fate_freed;
      } else {
        const ArgumentEffect callee_effect =
            argument_effect(statement, argument_index);
        if (callee_effect.resolved)
          effect = callee_effect.fates;
      }
      state = apply_relation(state, exact, possible, effect);
    }
    return state;
  }

  if (is_gimple_assign(statement) &&
      gimple_assign_single_p(statement) &&
      nonlocal_storage_p(gimple_assign_lhs(statement))) {
    Meaning stored = context->evaluate(gimple_assign_rhs1(statement));
    const bool exact = context->exact_parameter(stored, parameter_index,
                                                true);
    const bool possible =
        context->may_reference_parameter(stored, parameter_index);
    state = apply_relation(state, exact, possible, fate_escaped);
  }
  return state;
}

unsigned int solve_parameter_fates(FunctionContext *context,
                                   unsigned int parameter_index,
                                   bool *has_normal_exit) {
  std::map<basic_block, unsigned int> input;
  std::deque<basic_block> worklist;
  basic_block entry = ENTRY_BLOCK_PTR_FOR_FN(cfun);
  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, entry->succs) {
    input[successor->dest] |= fate_live;
    worklist.push_back(successor->dest);
  }

  unsigned int exits = fate_none;
  const unsigned int max_steps =
      static_cast<unsigned int>(n_basic_blocks_for_fn(cfun)) * 32u + 128u;
  unsigned int steps = 0;
  while (!worklist.empty()) {
    if (++steps > max_steps ||
        !analysis_budget::consume(
            analysis_budget::WorkKind::summary_parameter_cfg))
      return fate_unknown;
    basic_block block = worklist.front();
    worklist.pop_front();
    if (block == EXIT_BLOCK_PTR_FOR_FN(cfun)) {
      exits |= input[block];
      *has_normal_exit = true;
      continue;
    }

    unsigned int output = input[block];
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::summary_parameter_cfg))
        return fate_unknown;
      output = transfer_parameter(context, parameter_index, gsi_stmt(gsi),
                                  output);
    }

    FOR_EACH_EDGE(successor, iterator, block->succs) {
      const unsigned int previous = input[successor->dest];
      const unsigned int combined = previous | output;
      if (combined == previous)
        continue;
      input[successor->dest] = combined;
      worklist.push_back(successor->dest);
    }
  }
  return exits == fate_none ? fate_unknown : exits;
}

bool argument_is_value(const gimple *statement, unsigned int index,
                       tree value) {
  return index < gimple_call_num_args(statement) &&
         gimple_call_arg(statement, index) == value;
}

bool parameter_may_dereference(const UseExpansion &expansion) {
  if (expansion.truncated)
    return true;
  for (const UseSite &site : expansion.sites) {
    if (site.edge == FlowEdgeKind::memory_reference)
      return true;
    if (site.edge == FlowEdgeKind::direct_use &&
        site.statement != nullptr &&
        gimple_code(site.statement) == GIMPLE_ASM)
      return true;
    if (site.edge != FlowEdgeKind::call_argument ||
        !is_gimple_call(site.statement))
      continue;
    for (unsigned int index = 0;
         index < gimple_call_num_args(site.statement); ++index) {
      if (!argument_is_value(site.statement, index, site.incoming_value))
        continue;
      if ((ownership_deallocation_call_p(site.statement) ||
           ownership_reallocation_call_p(site.statement)) &&
          index == 0)
        continue;
      const ArgumentEffect effect = argument_effect(site.statement, index);
      if (!effect.resolved || effect.may_dereference)
        return true;
    }
  }
  return false;
}

struct ParameterMemoryAccess {
  FunctionContext *context;
  unsigned int index;
};

bool accesses_parameter_storage(gimple *, tree, tree operand, void *data) {
  auto *search = static_cast<ParameterMemoryAccess *>(data);
  HOST_WIDE_INT offset = 0;
  HOST_WIDE_INT size = 0;
  bool reverse = false;
  tree base = get_ref_base_and_extent_hwi(operand, &offset, &size, &reverse);
  // Ask GCC about the actual access width, not the assignment result type:
  // an empty subobject can be loaded into a nonempty result.  TARGET_MEM_REF
  // needs separate address reasoning and is deliberately not accepted here.
  if (base == NULL_TREE || size <= 0 ||
      (TREE_CODE(base) != MEM_REF && TREE_CODE(base) != INDIRECT_REF))
    return false;
  return search->context->exact_parameter(
      search->context->evaluate(TREE_OPERAND(base, 0)), search->index);
}

// Prove an access cut, not merely the existence of a dereference somewhere
// in the body.  This uses the same GCC SSA/use graph as lifetime discovery.
// A mixed PHI is not exact even when one use-expansion witness names this
// parameter; never discard that distinction when publishing a must fact.
std::map<gimple *, AccessGuarantee> parameter_access_events(
    FunctionContext *context, unsigned int index,
    const UseExpansion &expansion) {
  std::map<gimple *, AccessGuarantee> events;
  if (expansion.truncated)
    return events;
  for (const UseSite &site : expansion.sites) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_parameter_cfg))
      return {};
    if (site.statement == nullptr ||
        !context->exact_parameter(context->evaluate(site.incoming_value),
                                  index))
      continue;
    AccessGuarantee access = AccessGuarantee::none;
    if (site.edge == FlowEdgeKind::memory_reference &&
        is_gimple_assign(site.statement) &&
        !gimple_clobber_p(site.statement)) {
      ParameterMemoryAccess search{context, index};
      if (walk_stmt_load_store_addr_ops(site.statement, &search,
                                       accesses_parameter_storage,
                                       accesses_parameter_storage, nullptr))
        access = AccessGuarantee::must_access;
    } else if (site.edge == FlowEdgeKind::call_argument &&
               is_gimple_call(site.statement)) {
      for (unsigned int argument = 0;
           argument < gimple_call_num_args(site.statement); ++argument) {
        if (!argument_is_value(site.statement, argument, site.incoming_value))
          continue;
        const ArgumentEffect effect = argument_effect(site.statement, argument);
        if (effect.access == AccessGuarantee::must_access) {
          access = effect.access;
          break;
        }
        if (effect.has_access_guarantee() &&
            (access == AccessGuarantee::none ||
             !access_is_conditional(effect.access)))
          access = effect.access;
      }
    }
    if (access != AccessGuarantee::none) {
      AccessGuarantee &existing = events[site.statement];
      // Different SSA use sites can name the same call. Preserve the
      // strongest unconditional theorem instead of letting visitation order
      // replace it with a conditional guarantee from another argument.
      if (existing == AccessGuarantee::none ||
          access == AccessGuarantee::must_access ||
          (existing != AccessGuarantee::must_access &&
           (access_is_conditional(existing) ||
            !access_is_conditional(access))))
        existing = access;
    }
  }
  return events;
}

bool nonnull_parameter_edge_impossible(FunctionContext *context,
                                       unsigned int index, edge successor) {
  gimple_stmt_iterator gsi = gsi_last_bb(successor->src);
  gimple *last = gsi_end_p(gsi) ? nullptr : gsi_stmt(gsi);
  if (last == nullptr || gimple_code(last) != GIMPLE_COND)
    return false;
  const gcond *condition = as_a<gcond *>(last);
  const enum tree_code code = gimple_cond_code(condition);
  if (code != EQ_EXPR && code != NE_EXPR)
    return false;
  tree value = gimple_cond_lhs(condition);
  tree zero = gimple_cond_rhs(condition);
  if (TREE_CODE(value) == INTEGER_CST)
    std::swap(value, zero);
  if (zero == NULL_TREE || TREE_CODE(zero) != INTEGER_CST ||
      !integer_zerop(zero) || !pointer_value(value) ||
      !context->exact_parameter(context->evaluate(value), index))
    return false;
  const unsigned int branch_flags =
      successor->flags & (EDGE_TRUE_VALUE | EDGE_FALSE_VALUE);
  return code == EQ_EXPR ? branch_flags == EDGE_TRUE_VALUE
                         : branch_flags == EDGE_FALSE_VALUE;
}

bool all_paths_access_parameter(
    FunctionContext *context, unsigned int index,
    const std::map<gimple *, AccessGuarantee> &events, bool assume_nonnull) {
  // The graph contains only prefixes before the first guaranteed access.
  // Kahn's algorithm rejects any remaining cycle without path enumeration or
  // C++ recursion.  Unknown guards retain both successors.  An opaque call
  // before the cut is a barrier: its return/termination is not a theorem.
  std::map<basic_block, std::vector<basic_block>> graph;
  std::map<basic_block, unsigned int> indegree;
  std::deque<basic_block> pending;
  basic_block entry = ENTRY_BLOCK_PTR_FOR_FN(cfun);
  indegree[entry] = 0;
  pending.push_back(entry);
  bool saw_access = false;
  while (!pending.empty()) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_parameter_cfg))
      return false;
    basic_block block = pending.front();
    pending.pop_front();
    if (block == EXIT_BLOCK_PTR_FOR_FN(cfun))
      return false;
    bool accessed = false;
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::summary_parameter_cfg))
        return false;
      gimple *statement = gsi_stmt(gsi);
      const auto event = events.find(statement);
      if (event != events.end() &&
          (!access_is_conditional(event->second) || assume_nonnull)) {
        accessed = true;
        saw_access = true;
        break;
      }
      if (is_gimple_call(statement) ||
          gimple_code(statement) == GIMPLE_ASM ||
          gimple_code(statement) == GIMPLE_RETURN)
        return false;
    }
    graph[block];
    if (accessed)
      continue;
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::summary_parameter_cfg) ||
          (successor->flags &
           (EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_FAKE)) != 0)
        return false;
      if (assume_nonnull &&
          nonnull_parameter_edge_impossible(context, index, successor))
        continue;
      graph[block].push_back(successor->dest);
      if (indegree.emplace(successor->dest, 0).second)
        pending.push_back(successor->dest);
      ++indegree[successor->dest];
    }
    if (graph[block].empty())
      return false;
  }
  for (const auto &item : indegree)
    if (item.second == 0)
      pending.push_back(item.first);
  unsigned int removed = 0;
  while (!pending.empty()) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_parameter_cfg))
      return false;
    basic_block block = pending.front();
    pending.pop_front();
    ++removed;
    for (basic_block successor : graph[block])
      if (--indegree[successor] == 0)
        pending.push_back(successor);
  }
  return saw_access && removed == graph.size();
}

AccessGuarantee parameter_access_guarantee(FunctionContext *context,
                                          unsigned int index,
                                          const ParameterSummary &parameter,
                                          const UseExpansion &expansion) {
  if (!parameter.pointer || parameter.default_definition == NULL_TREE)
    return AccessGuarantee::none;
  const auto events = parameter_access_events(context, index, expansion);
  if (events.empty())
    return AccessGuarantee::none;
  // Do not relabel a library validity precondition as a physical-read proof.
  // Requiring liveness is also justified by a direct memory access, so mixed
  // cuts conservatively publish the precondition dimension.
  const bool requires_live = std::any_of(
      events.begin(), events.end(), [](const auto &event) {
        return access_is_live_requirement(event.second);
      });
  if (all_paths_access_parameter(context, index, events, false))
    return requires_live ? AccessGuarantee::requires_live
                         : AccessGuarantee::must_access;
  if (analysis_budget::available() &&
      all_paths_access_parameter(context, index, events, true))
    return requires_live ? AccessGuarantee::requires_live_if_nonnull
                         : AccessGuarantee::access_if_nonnull;
  return AccessGuarantee::none;
}

bool fresh_origin_safe(const gimple *origin) {
  if (origin == nullptr || !is_gimple_call(origin))
    return false;
  tree value = gimple_call_lhs(origin);
  if (!pointer_value(value))
    return false;
  const UseExpansion expansion = expand_immediate_uses(value);
  if (expansion.truncated)
    return false;
  for (const UseSite &site : expansion.sites) {
    if (site.edge == FlowEdgeKind::return_value ||
        site.edge == FlowEdgeKind::memory_reference)
      continue;
    if (is_gimple_assign(site.statement) &&
        gimple_assign_single_p(site.statement) &&
        gimple_assign_rhs1(site.statement) == site.incoming_value &&
        nonlocal_storage_p(gimple_assign_lhs(site.statement)))
      return false;
    if (site.edge != FlowEdgeKind::call_argument ||
        !is_gimple_call(site.statement))
      continue;
    for (unsigned int index = 0;
         index < gimple_call_num_args(site.statement); ++index) {
      if (!argument_is_value(site.statement, index, site.incoming_value))
        continue;
      if ((ownership_deallocation_call_p(site.statement) ||
           ownership_reallocation_call_p(site.statement)) &&
          index == 0)
        return false;
      const ArgumentEffect effect = argument_effect(site.statement, index);
      if (!effect.resolved)
        return false;
      const unsigned int fates = effect.fates;
      if ((fates & (fate_freed | fate_escaped | fate_unknown)) != 0)
        return false;
    }
  }
  return true;
}

bool local_pointer_spill_statement_p(const gimple *statement) {
  if (!is_gimple_assign(statement) || !gimple_assign_single_p(statement))
    return false;
  tree lhs = gimple_assign_lhs(statement);
  tree rhs = gimple_assign_rhs1(statement);
  if (lhs == NULL_TREE || rhs == NULL_TREE || TREE_CODE(lhs) == SSA_NAME ||
      !pointer_value(rhs))
    return false;
  tree base = get_base_address(lhs);
  return base != NULL_TREE && DECL_P(base) &&
         auto_var_in_fn_p(base, cfun->decl);
}

bool opaque_summary_call_p(const gimple *statement) {
  if (!is_gimple_call(statement) ||
      ownership_allocation_call_p(statement) ||
      ownership_deallocation_call_p(statement) ||
      ownership_reallocation_call_p(statement))
    return false;
  return summary_for_call(statement) == nullptr;
}

struct NormalContinuation {
  unsigned int statements = 0;
  std::set<tree> active;
  std::set<tree> complete;
};

bool normal_return_continuation(const gimple *statement,
                                NormalContinuation *query, unsigned int depth = 0) {
  if (depth >= 16 || !is_gimple_call(statement) ||
      gimple_call_noreturn_p(statement) ||
      (gimple_call_flags(statement) & ECF_RETURNS_TWICE) != 0)
    return false;
  if (gcc_call_effect::standard_call_has_normal_return(as_a<const gcall *>(statement)))
    return true;
  cgraph_node *target = bound_target(statement, cgraph_node::get(cfun->decl));
  if (target == nullptr || target->get_fun()->cfg == nullptr ||
      gimple_call_fntype(as_a<const gcall *>(statement)) != TREE_TYPE(target->decl) ||
      n_basic_blocks_for_fn(target->get_fun()) > 256 ||
      query->active.count(target->decl) != 0 || query->complete.size() >= 64)
    return false;
  if (query->complete.count(target->decl) != 0)
    return true;
  query->active.insert(target->decl);
  push_cfun(target->get_fun());
  std::map<basic_block, unsigned int> indegrees;
  bool valid = true;
  bool saw_return = false;
  basic_block block;
  FOR_EACH_BB_FN(block, cfun)
    indegrees.emplace(block, 0);
  FOR_EACH_BB_FN(block, cfun) {
    if (!valid)
      break;
    bool returns = false;
    for (gimple_stmt_iterator iterator = gsi_start_bb(block);
         !gsi_end_p(iterator); gsi_next(&iterator)) {
      if (++query->statements > 1024 ||
          !analysis_budget::consume(analysis_budget::WorkKind::summary_dependency)) {
        valid = false;
        break;
      }
      gimple *item = gsi_stmt(iterator);
      if (gimple_code(item) == GIMPLE_ASM ||
          (is_gimple_call(item) && !normal_return_continuation(item, query, depth + 1))) {
        valid = false;
        break;
      }
      returns = returns || gimple_code(item) == GIMPLE_RETURN;
    }
    saw_return = saw_return || returns;
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      if (++query->statements > 1024 ||
          !analysis_budget::consume(analysis_budget::WorkKind::summary_dependency) ||
          (successor->flags & (EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_FAKE)) != 0 ||
          (successor->dest == EXIT_BLOCK_PTR_FOR_FN(cfun) && !returns)) {
        valid = false;
        break;
      }
      const auto found = indegrees.find(successor->dest);
      if (found != indegrees.end())
        ++found->second;
    }
    if (EDGE_COUNT(block->succs) == 0 && !returns)
      valid = false;
  }
  std::vector<basic_block> ready;
  for (const auto &entry : indegrees)
    if (entry.second == 0)
      ready.push_back(entry.first);
  unsigned int removed = 0;
  while (valid && !ready.empty()) {
    block = ready.back();
    ready.pop_back();
    ++removed;
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs) {
      const auto found = indegrees.find(successor->dest);
      if (found != indegrees.end() && --found->second == 0)
        ready.push_back(found->first);
    }
  }
  valid = valid && saw_return && removed == indegrees.size();
  pop_cfun();
  query->active.erase(target->decl);
  if (valid)
    query->complete.insert(target->decl);
  return valid;
}

bool process_termination_sink_p(const gimple *statement) {
  if (!is_gimple_call(statement) || !gimple_call_noreturn_p(statement))
    return false;
  tree declaration = gimple_call_fndecl(statement);
  if (declaration == NULL_TREE ||
      !fndecl_built_in_p(declaration, BUILT_IN_NORMAL))
    return false;
  // Only the compiler's actual process-termination identities, never an
  // arbitrary noreturn function (which might unwind or longjmp). Such a
  // sink contributes no normal return to a conditional ownership summary.
  // exit may invoke callbacks; this does NOT infer any ownership effect for
  // them or discard an edge on which control could rejoin the caller.
  const auto code = static_cast<enum built_in_function>(
      DECL_FUNCTION_CODE(declaration));
  if (code != BUILT_IN_EXIT && code != BUILT_IN_ABORT &&
      code != BUILT_IN__EXIT && code != BUILT_IN__EXIT2)
    return false;
  basic_block block = gimple_bb(statement);
  if (block == nullptr || EDGE_COUNT(block->succs) != 0 ||
      EDGE_COUNT(block->preds) != 1)
    return false;
  gimple_stmt_iterator last = gsi_last_bb(block);
  if (gsi_end_p(last) || gsi_stmt(last) != statement)
    return false;

  // A merely conditional normal-return summary is not an existence proof:
  // consume_or_exit(p, 0) must not invent a later double-free. Admit only
  // input-independent allocation wrappers whose sole branch terminates exactly
  // on failure of the allocation returned on the other, linear CFG arm.
  edge failure_edge = single_pred_edge(block);
  basic_block guard = failure_edge->src;
  gimple_stmt_iterator guard_last = gsi_last_bb(guard);
  if (gsi_end_p(guard_last) || EDGE_COUNT(guard->succs) != 2 ||
      gimple_code(gsi_stmt(guard_last)) != GIMPLE_COND)
    return false;
  const gcond *condition = as_a<gcond *>(gsi_stmt(guard_last));
  tree allocated = gimple_cond_lhs(condition);
  tree zero = gimple_cond_rhs(condition);
  if (allocated != NULL_TREE && TREE_CODE(allocated) == INTEGER_CST)
    std::swap(allocated, zero);
  if (!pointer_value(allocated) || TREE_CODE(allocated) != SSA_NAME ||
      zero == NULL_TREE || TREE_CODE(zero) != INTEGER_CST ||
      !integer_zerop(zero))
    return false;
  const auto comparison = gimple_cond_code(condition);
  const unsigned int selected =
      failure_edge->flags & (EDGE_TRUE_VALUE | EDGE_FALSE_VALUE);
  if (!((comparison == EQ_EXPR && selected == EDGE_TRUE_VALUE) ||
        (comparison == NE_EXPR && selected == EDGE_FALSE_VALUE)))
    return false;
  gimple *allocation = SSA_NAME_DEF_STMT(allocated);
  tree allocator = allocation != nullptr && is_gimple_call(allocation)
                       ? gimple_call_fndecl(allocation) : NULL_TREE;
  if (allocator == NULL_TREE ||
      !fndecl_built_in_p(allocator, BUILT_IN_NORMAL) ||
      (DECL_FUNCTION_CODE(allocator) != BUILT_IN_MALLOC &&
       DECL_FUNCTION_CODE(allocator) != BUILT_IN_CALLOC &&
       DECL_FUNCTION_CODE(allocator) != BUILT_IN_REALLOC))
    return false;

  const bool reallocation = DECL_FUNCTION_CODE(allocator) == BUILT_IN_REALLOC;
  tree input = reallocation ? gimple_call_arg(allocation, 0) : NULL_TREE;
  bool parameter_input = false;
  for (tree formal = DECL_ARGUMENTS(cfun->decl); formal != NULL_TREE;
       formal = DECL_CHAIN(formal)) {
    tree value = ssa_default_def(cfun, formal);
    if (value == NULL_TREE || has_zero_uses(value))
      continue;
    use_operand_p use;
    gimple *user = nullptr;
    if (!reallocation || value != input || !pointer_value(value) ||
        !single_imm_use(value, &use, &user) || user != allocation)
      return false;
    parameter_input = true;
  }
  // The only admitted input dependence is realloc's original pointer. It
  // establishes fresh ownership only when the caller supplies exact NULL;
  // ordinary non-NULL callers do not gain a fresh-object candidate.
  if (reallocation && !parameter_input &&
      (input == NULL_TREE || TREE_CODE(input) != INTEGER_CST || !integer_zerop(input)))
    return false;

  basic_block candidate;
  NormalContinuation continuation;
  FOR_EACH_BB_FN(candidate, cfun) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_dependency) ||
        (candidate != guard && EDGE_COUNT(candidate->succs) > 1))
      return false;
    for (gimple_stmt_iterator gsi = gsi_start_bb(candidate); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      gimple *item = gsi_stmt(gsi);
      // A summary saying that some path returns is insufficient. Require a
      // bounded acyclic continuation whose calls all have normal-return API
      // models or recursively bound total bodies. Unknown callbacks, loops,
      // termination, abnormal edges and checked-library failure remain barriers.
      if (is_gimple_call(item) && item != allocation && item != statement &&
          !normal_return_continuation(item, &continuation))
        return false;
    }
  }

  basic_block cursor = nullptr;
  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, guard->succs)
    if (successor != failure_edge)
      cursor = successor->dest;
  std::set<basic_block> visited;
  while (cursor != nullptr && cursor != EXIT_BLOCK_PTR_FOR_FN(cfun)) {
    if (!visited.insert(cursor).second ||
        !analysis_budget::consume(
            analysis_budget::WorkKind::summary_dependency))
      return false;
    for (gimple_stmt_iterator gsi = gsi_start_bb(cursor); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      gimple *item = gsi_stmt(gsi);
      if (gimple_code(item) == GIMPLE_RETURN)
        return gimple_return_retval(as_a<greturn *>(item)) == allocated;
      if (is_gimple_call(item) && gimple_call_noreturn_p(item))
        return false;
    }
    if (EDGE_COUNT(cursor->succs) != 1)
      return false;
    cursor = single_succ(cursor);
  }
  return false;
}

bool body_is_unsupported() {
  // Scalar SSA use-def chains cannot see a pointer after it is embedded in an
  // automatic aggregate.  If an opaque call can observe that aggregate, it
  // may retain, free, or dereference the hidden pointer.  Publishing a fresh
  // return or no-op parameter summary in that situation would let callers
  // turn missing aggregate evidence into a false proof.
  bool has_local_pointer_spill = false;
  bool has_opaque_call = false;
  basic_block block;
  FOR_EACH_BB_FN(block, cfun) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_dependency))
      return true;
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, block->succs)
      if ((successor->flags &
           (EDGE_EH | EDGE_ABNORMAL | EDGE_ABNORMAL_CALL | EDGE_FAKE)) != 0) {
        if (dump_file != nullptr && (dump_flags & TDF_DETAILS))
          dump_printf(MSG_NOTE,
                      "ownership-summary reject: abnormal-edge flags=%x\n",
                      successor->flags);
        return true;
      }
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::summary_dependency))
        return true;
      gimple *statement = gsi_stmt(gsi);
      has_local_pointer_spill =
          has_local_pointer_spill ||
          local_pointer_spill_statement_p(statement);
      has_opaque_call = has_opaque_call || opaque_summary_call_p(statement);
      if (gimple_code(statement) == GIMPLE_ASM)
        return true;
      if (is_gimple_call(statement) &&
          ((gimple_call_flags(statement) & ECF_RETURNS_TWICE) != 0 ||
           (gimple_call_noreturn_p(statement) &&
            !process_termination_sink_p(statement)))) {
        if (dump_file != nullptr && (dump_flags & TDF_DETAILS)) {
          dump_printf(MSG_NOTE,
                      "ownership-summary reject: exceptional-call flags=%x\n",
                      gimple_call_flags(statement));
          dump_gimple_stmt(MSG_NOTE, TDF_SLIM, statement, 0);
        }
        return true;
      }
    }
  }
  if (has_local_pointer_spill && has_opaque_call) {
    if (dump_file != nullptr && (dump_flags & TDF_DETAILS))
      dump_printf(MSG_NOTE,
                  "ownership-summary reject: local pointer spill reaches "
                  "opaque aggregate-capable call\n");
    return true;
  }
  return false;
}

FunctionSummary make_prototype(cgraph_node *node) {
  FunctionSummary summary;
  summary.declaration = node->decl;
  summary.function_type = TREE_TYPE(node->decl);
  if (DECL_ASSEMBLER_NAME_SET_P(node->decl))
    summary.assembler_name = DECL_ASSEMBLER_NAME(node->decl);
  for (tree parameter = DECL_ARGUMENTS(node->decl);
       parameter != NULL_TREE; parameter = DECL_CHAIN(parameter)) {
    ParameterSummary item;
    item.declaration = parameter;
    item.formal_type = TREE_TYPE(parameter);
    item.default_definition = ssa_default_def(node->get_fun(), parameter);
    item.pointer = TREE_TYPE(parameter) != NULL_TREE &&
                   POINTER_TYPE_P(TREE_TYPE(parameter));
    item.fates = item.pointer ? fate_unknown : fate_live;
    item.may_dereference = item.pointer;
    summary.parameters.push_back(item);
  }
  return summary;
}

bool provider_fate_expansion_complete(const UseExpansion &expansion) {
  if (expansion.truncated)
    return false;
  for (const UseSite &site : expansion.sites) {
    const gimple *statement = site.statement;
    if (statement == nullptr)
      return false;
    if (local_pointer_spill_statement_p(statement) ||
        (site.edge == FlowEdgeKind::cast &&
         !pointer_preserving_cast_p(statement)))
      return false;
    if (site.edge != FlowEdgeKind::direct_use)
      continue;
    // Conditions/comparisons inspect the address but do not publish it.
    if (gimple_code(statement) == GIMPLE_COND ||
        is_gimple_debug(statement))
      continue;
    if (is_gimple_assign(statement) &&
        (TREE_CODE_CLASS(gimple_assign_rhs_code(statement)) == tcc_comparison ||
         (gimple_assign_single_p(statement) &&
          gimple_assign_rhs1(statement) == site.incoming_value &&
          nonlocal_storage_p(gimple_assign_lhs(statement)))))
      continue;
    // In particular pointer -> integer arithmetic is not an SSA pointer
    // edge. Retain unknown ownership rather than forgetting a hidden escape.
    return false;
  }
  return true;
}

FunctionSummary unsupported_summary(const FunctionSummary &prototype,
                                    bool recursive) {
  FunctionSummary result = prototype;
  result.analyzed = true;
  result.unsupported = true;
  result.recursive = recursive;
  result.truncated = false;
  result.has_normal_return = false;
  result.returns_fresh_owned = false;
  result.returns_specializable_owned = false;
  result.returns.clear();
  result.returns.push_back({ReturnKind::unknown, 0, nullptr, false});
  for (ParameterSummary &parameter : result.parameters) {
    parameter.fates = parameter.pointer ? fate_unknown : fate_live;
    parameter.may_dereference = parameter.pointer;
    parameter.access = AccessGuarantee::none;
  }
  return result;
}

FunctionSummary compute_summary(cgraph_node *node,
                                const FunctionSummary &prototype) {
  if (budget_unsupported_functions.count(node->decl) != 0)
    return unsupported_summary(prototype, false);
  if (!analysis_budget::begin_function(node->decl))
    return unsupported_summary(prototype, false);
  if (recursive_functions.count(node->decl) != 0)
  {
    analysis_budget::end_function();
    return unsupported_summary(prototype, true);
  }

  if (prototype.parameters.size() > max_summary_parameters ||
      static_cast<unsigned int>(n_basic_blocks_for_fn(node->get_fun())) >
          max_summary_blocks) {
    analysis_budget::end_function();
    return unsupported_summary(prototype, false);
  }

  push_cfun(node->get_fun());
  reset_value_expansion_cache();
  if (body_is_unsupported()) {
    pop_cfun();
    analysis_budget::end_function();
    return unsupported_summary(prototype, false);
  }

  FunctionSummary result = prototype;
  result.analyzed = true;
  result.unsupported = false;
  result.recursive = false;
  result.truncated = false;
  result.has_normal_return = false;
  result.returns_fresh_owned = false;
  result.returns_specializable_owned = false;
  result.returns.clear();
  FunctionContext context(prototype);
  bool returns_truncated = false;

  basic_block block;
  FOR_EACH_BB_FN(block, cfun) {
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::summary_dependency)) {
        pop_cfun();
        analysis_budget::end_function();
        return unsupported_summary(prototype, false);
      }
      gimple *statement = gsi_stmt(gsi);
      if (gimple_code(statement) != GIMPLE_RETURN)
        continue;
      result.has_normal_return = true;
      tree returned =
          gimple_return_retval(as_a<greturn *>(statement));
      Meaning meaning = returned == NULL_TREE ? unknown_meaning()
                                              : context.evaluate(returned);
      if (!meaning.complete)
        append_alternative(
            &meaning, {ReturnKind::unknown, 0, nullptr, false});
      for (const ReturnAlternative &alternative : meaning.alternatives) {
        if (returns_truncated)
          break;
        bool duplicate = false;
        for (const ReturnAlternative &existing : result.returns)
          if (same_return_alternative(
                  existing, alternative,
                  alternative.kind == ReturnKind::fresh_owned)) {
            duplicate = true;
            break;
          }
        if (!duplicate) {
          if (result.returns.size() < max_return_alternatives) {
            result.returns.push_back(alternative);
          } else {
            result.returns.clear();
            result.returns.push_back(
                {ReturnKind::unknown, 0, nullptr, false});
            returns_truncated = true;
            result.truncated = true;
          }
        }
      }
    }
  }

  bool has_fresh = false;
  bool only_fresh_or_null = result.has_normal_return &&
                            !result.returns.empty();
  std::set<const gimple *> checked_origins;
  for (const ReturnAlternative &alternative : result.returns) {
    if (alternative.kind == ReturnKind::fresh_owned) {
      has_fresh = true;
      if (checked_origins.insert(alternative.fresh_origin).second &&
          !fresh_origin_safe(alternative.fresh_origin))
        only_fresh_or_null = false;
    } else if (alternative.kind != ReturnKind::null_value) {
      only_fresh_or_null = false;
    }
  }
  result.returns_fresh_owned = has_fresh && only_fresh_or_null;
  const bool has_specialized_origin = std::any_of(
      result.returns.begin(), result.returns.end(), [](const ReturnAlternative &alternative) {
        return alternative.kind == ReturnKind::realloc_parameter;
      });
  bool safe_specialized_origins = result.has_normal_return && has_specialized_origin;
  if (has_specialized_origin) for (const ReturnAlternative &alternative : result.returns) {
    if (alternative.kind == ReturnKind::null_value)
      continue;
    if ((alternative.kind != ReturnKind::fresh_owned &&
         alternative.kind != ReturnKind::realloc_parameter) || alternative.derived ||
        alternative.fresh_origin == nullptr || !fresh_origin_safe(alternative.fresh_origin)) {
      safe_specialized_origins = false;
      break;
    }
  }
  result.returns_specializable_owned = has_specialized_origin && safe_specialized_origins;

  for (unsigned int index = 0; index < result.parameters.size(); ++index) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_parameter_cfg)) {
      pop_cfun();
      analysis_budget::end_function();
      return unsupported_summary(prototype, false);
    }
    ParameterSummary &parameter = result.parameters[index];
    if (!parameter.pointer) {
      parameter.fates = fate_live;
      parameter.may_dereference = false;
      continue;
    }
    bool normal_exit = false;
    parameter.fates = solve_parameter_fates(&context, index, &normal_exit);
    if (!normal_exit)
      parameter.fates = fate_unknown;
    // Reuse one GCC immediate-use walk for the may and must dimensions.
    const UseExpansion expansion =
        expand_immediate_uses(parameter.default_definition);
    parameter.may_dereference = parameter_may_dereference(expansion);
    parameter.access =
        parameter_access_guarantee(&context, index, parameter, expansion);
    if (generating_provider_facts &&
        !provider_fate_expansion_complete(expansion)) {
      parameter.fates = fate_unknown;
      parameter.may_dereference = true;
    }
    if (!analysis_budget::available()) {
      pop_cfun();
      analysis_budget::end_function();
      return unsupported_summary(prototype, false);
    }
  }

  pop_cfun();
  analysis_budget::end_function();
  return result;
}

void collect_nodes() {
  cgraph_node *node;
  FOR_EACH_DEFINED_FUNCTION(node) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_node))
      return;
    if (node->decl == NULL_TREE || !gimple_has_body_p(node->decl) ||
        node->get_fun() == nullptr || node->get_fun()->cfg == nullptr ||
        !gimple_in_ssa_p(node->get_fun()))
      continue;
    tree function_type = TREE_TYPE(node->decl);
    tree return_type = function_type == NULL_TREE
                           ? NULL_TREE
                           : TREE_TYPE(function_type);
    bool ownership_relevant =
        return_type != NULL_TREE && POINTER_TYPE_P(return_type);
    for (tree parameter = DECL_ARGUMENTS(node->decl);
         !ownership_relevant && parameter != NULL_TREE;
         parameter = DECL_CHAIN(parameter))
      ownership_relevant = TREE_TYPE(parameter) != NULL_TREE &&
                           POINTER_TYPE_P(TREE_TYPE(parameter));
    if (ownership_relevant && summary_nodes.count(node->decl) == 0)
      summary_nodes[node->decl] = node;
  }
}

std::map<tree, std::set<tree>> collect_dependencies() {
  std::map<tree, std::set<tree>> dependencies;
  for (const auto &entry : summary_nodes) {
    tree declaration = entry.first;
    cgraph_node *node = entry.second;
    dependencies[declaration];
    if (!analysis_budget::begin_function(declaration))
      break;
    push_cfun(node->get_fun());
    bool complete = true;
    basic_block block;
    FOR_EACH_BB_FN(block, cfun) {
      for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
        if (!analysis_budget::consume(
                analysis_budget::WorkKind::summary_dependency)) {
          complete = false;
          break;
        }
        gimple *statement = gsi_stmt(gsi);
        if (!is_gimple_call(statement))
          continue;
        cgraph_node *target = bound_target(statement, node);
        if (target != nullptr && summary_nodes.count(target->decl) != 0)
          dependencies[declaration].insert(target->decl);
      }
      if (!complete)
        break;
    }
    pop_cfun();
    analysis_budget::end_function();
    if (!complete)
      budget_unsupported_functions.insert(declaration);
    if (!complete && !analysis_budget::tu_available())
      break;
  }
  return dependencies;
}

class Tarjan {
 public:
  explicit Tarjan(const std::map<tree, std::set<tree>> &graph)
      : graph_(graph) {}

  void run() {
    for (const auto &entry : graph_)
      if (indices_.count(entry.first) == 0 &&
          analysis_budget::consume(
              analysis_budget::WorkKind::summary_scc))
        visit(entry.first);
  }

 private:
  void visit(tree vertex) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_scc))
      return;
    const int index = next_index_++;
    indices_[vertex] = index;
    lowlinks_[vertex] = index;
    stack_.push_back(vertex);
    on_stack_.insert(vertex);

    const auto found = graph_.find(vertex);
    if (found != graph_.end()) {
      for (tree target : found->second) {
        if (!analysis_budget::consume(
                analysis_budget::WorkKind::summary_scc))
          return;
        if (indices_.count(target) == 0) {
          visit(target);
          lowlinks_[vertex] =
              std::min(lowlinks_[vertex], lowlinks_[target]);
        } else if (on_stack_.count(target) != 0) {
          lowlinks_[vertex] =
              std::min(lowlinks_[vertex], indices_[target]);
        }
      }
    }

    if (lowlinks_[vertex] != indices_[vertex])
      return;
    std::vector<tree> component;
    while (!stack_.empty()) {
      tree member = stack_.back();
      stack_.pop_back();
      on_stack_.erase(member);
      component.push_back(member);
      if (member == vertex)
        break;
    }
    bool recursive = component.size() > 1;
    if (!recursive) {
      const auto outgoing = graph_.find(vertex);
      recursive = outgoing != graph_.end() &&
                  outgoing->second.count(vertex) != 0;
    }
    if (recursive)
      recursive_functions.insert(component.begin(), component.end());
  }

  const std::map<tree, std::set<tree>> &graph_;
  int next_index_ = 0;
  std::map<tree, int> indices_;
  std::map<tree, int> lowlinks_;
  std::vector<tree> stack_;
  std::set<tree> on_stack_;
};

void append_callee_first(
    tree declaration, const std::map<tree, std::set<tree>> &dependencies,
    std::set<tree> *visited, std::vector<tree> *order) {
  if (!analysis_budget::consume(
          analysis_budget::WorkKind::summary_order) ||
      !visited->insert(declaration).second)
    return;
  const auto found = dependencies.find(declaration);
  if (found != dependencies.end())
    for (tree callee : found->second)
      if (recursive_functions.count(callee) == 0)
        append_callee_first(callee, dependencies, visited, order);
  order->push_back(declaration);
}

const char *return_kind_name(ReturnKind kind) {
  switch (kind) {
    case ReturnKind::null_value:
      return "null";
    case ReturnKind::fresh_owned:
      return "fresh-owned";
    case ReturnKind::parameter:
      return "parameter";
    case ReturnKind::realloc_parameter:
      return "realloc-parameter";
    case ReturnKind::global_object:
      return "global";
    case ReturnKind::fresh_shared:
      return "fresh-shared";
    case ReturnKind::unknown:
      return "unknown";
  }
  return "unknown";
}

void dump_summaries() {
  if (dump_file == nullptr || !(dump_flags & TDF_DETAILS))
    return;
  for (const auto &entry : summaries) {
    const FunctionSummary &summary = entry.second;
    const char *name =
        summary.declaration != NULL_TREE &&
                DECL_NAME(summary.declaration) != NULL_TREE
            ? IDENTIFIER_POINTER(DECL_NAME(summary.declaration))
            : "<anonymous>";
    dump_printf(MSG_NOTE,
                "ownership-summary: function=%s unsupported=%s "
                "recursive=%s truncated=%s fresh-return=%s returns=",
                name, summary.unsupported ? "yes" : "no",
                summary.recursive ? "yes" : "no",
                summary.truncated ? "yes" : "no",
                summary.returns_fresh_owned ? "yes" : "no");
    for (unsigned int index = 0; index < summary.returns.size(); ++index) {
      if (index != 0)
        dump_printf(MSG_NOTE, "|");
      dump_printf(MSG_NOTE, "%s", return_kind_name(summary.returns[index].kind));
      if (summary.returns[index].kind == ReturnKind::parameter ||
          summary.returns[index].kind == ReturnKind::realloc_parameter)
        dump_printf(MSG_NOTE, "(%u)",
                    summary.returns[index].parameter_index);
    }
    dump_printf(MSG_NOTE, "\n");
    for (unsigned int index = 0; index < summary.parameters.size(); ++index) {
      const ParameterSummary &parameter = summary.parameters[index];
      if (!parameter.pointer)
        continue;
      dump_printf(MSG_NOTE,
                  "  param=%u fates=%s may-dereference=%s access=%s\n", index,
                  fate_set_name(parameter.fates),
                  parameter.may_dereference ? "yes" : "no",
                  access_guarantee_name(parameter.access));
    }
  }
}

}  // namespace

void reset() {
  summaries.clear();
  provider_summaries.clear();
  relative_release_summaries.clear();
  relative_access_summaries.clear();
  conditional_borrow_summaries.clear();
  returned_object_summaries.clear();
  invocation_frees.clear();
  summary_nodes.clear();
  recursive_functions.clear();
  budget_unsupported_functions.clear();
  statistics = Statistics{};
  summaries_built = false;
}

void configure_callee_model(CalleeModel model, const std::string &symbol,
                            unsigned int argument_index) {
  configured_callee_model = model;
  configured_callee_symbol = symbol;
  configured_callee_argument = argument_index;
}

void build() {
  reset();
  collect_nodes();
  statistics.functions = summary_nodes.size();
  for (const auto &entry : summary_nodes)
    summaries[entry.first] = make_prototype(entry.second);
  if (!analysis_budget::tu_available() ||
      !analysis_budget::accept_summary_node_count(summary_nodes.size())) {
    for (auto &entry : summaries)
      entry.second = unsupported_summary(entry.second, false);
    summaries_built = true;
    return;
  }
  const std::map<tree, std::set<tree>> dependencies = collect_dependencies();
  if (!analysis_budget::tu_available()) {
    for (auto &entry : summaries)
      entry.second = unsupported_summary(entry.second, false);
    summaries_built = true;
    return;
  }
  Tarjan(dependencies).run();
  if (!analysis_budget::tu_available()) {
    for (auto &entry : summaries)
      entry.second = unsupported_summary(entry.second, false);
    summaries_built = true;
    return;
  }

  // Recursive SCCs are an explicit Phase-1 boundary.  Publish their unknown
  // summaries first so an acyclic caller can still be summarized without
  // ever treating a recursive callee as bottom/no-effect.
  for (tree declaration : recursive_functions)
    summaries[declaration] =
        unsupported_summary(summaries[declaration], true);

  // The remaining condensation graph is a DAG.  A callee-first worklist
  // reaches the same fixed point as synchronous whole-TU rounds while
  // evaluating each body once instead of O(functions * call-depth).
  std::set<tree> visited;
  std::vector<tree> order;
  for (const auto &entry : summary_nodes)
    if (recursive_functions.count(entry.first) == 0)
      append_callee_first(entry.first, dependencies, &visited, &order);
  for (tree declaration : order) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_order))
      break;
    const auto node = summary_nodes.find(declaration);
    if (node == summary_nodes.end())
      continue;
    summaries[declaration] =
        compute_summary(node->second, summaries[declaration]);
    ++statistics.evaluations;
  }
  statistics.fixed_point_rounds = order.empty() ? 0 : 1;

  // Any node omitted after a function/TU budget exhaustion remains an
  // explicit unknown summary; it must never be queried as a body proof.
  for (auto &entry : summaries)
    if (!entry.second.analyzed)
      entry.second = unsupported_summary(entry.second, false);

  summaries_built = true;
  for (const auto &entry : summaries) {
    const FunctionSummary &summary = entry.second;
    if (summary.recursive)
      ++statistics.recursive;
    if (summary.unsupported)
      ++statistics.unsupported;
    else
      ++statistics.usable;
    if (summary.returns_fresh_owned)
      ++statistics.fresh_returners;
  }
  dump_summaries();
}

void generate_provider_facts() {
  analysis_budget::reset();
  set_ownership_proof_only(true);
  generating_provider_facts = true;
  build();
  unsigned int exported = 0;
  unsigned int rejected = 0;
  for (const auto &entry : summaries) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::summary_dependency))
      break;
    const FunctionSummary &summary = entry.second;
    std::vector<tree> formal_types;
    if (!summary.analyzed || summary.unsupported || summary.recursive ||
        summary.truncated || !summary.has_normal_return ||
        !provider_signature_matches(entry.first, summary) ||
        !complete_formal_types(summary.function_type, &formal_types) ||
        formal_types.size() != summary.parameters.size()) {
      ++rejected;
      continue;
    }
    bool useful = false;
    bool valid = true;
    for (unsigned int index = 0; index < summary.parameters.size(); ++index) {
      const ParameterSummary &parameter = summary.parameters[index];
      if (parameter.formal_type == NULL_TREE ||
          TYPE_MAIN_VARIANT(parameter.formal_type) !=
              TYPE_MAIN_VARIANT(formal_types[index])) {
        valid = false;
        break;
      }
      useful = useful ||
          (parameter.pointer &&
           (parameter.fates != fate_unknown || !parameter.may_dereference ||
            parameter.access != AccessGuarantee::none));
    }
    if (!valid || !useful) {
      ++rejected;
      continue;
    }
    std::vector<tree> parameters;
    for (unsigned int index = 0; index < summary.parameters.size(); ++index) {
      const ParameterSummary &parameter = summary.parameters[index];
      parameters.push_back(encode_provider_list({
          parameter.formal_type,
          build_int_cstu(unsigned_type_node, parameter.fates),
          build_int_cstu(unsigned_type_node,
                         parameter.may_dereference ? 1 : 0),
          build_int_cstu(unsigned_type_node,
                         static_cast<unsigned int>(parameter.access))}));
    }
    tree payload = encode_provider_list({
        build_int_cstu(unsigned_type_node, provider_schema_version),
        summary.function_type, summary.assembler_name,
        build_int_cstu(unsigned_type_node, summary.parameters.size()),
        encode_provider_list(parameters),
        build_int_cstu(unsigned_type_node, 1),
        build_int_cstu(unsigned_type_node, 0)});
    provider_transport::record(entry.first, payload);
    if (provider_transport::lookup(entry.first) == payload)
      ++exported;
    else
      ++rejected;
  }
  if (dump_file != nullptr)
    dump_printf(MSG_NOTE,
                "ownership-provider producer: schema=%u exported=%u "
                "rejected=%u custom-contracts=disabled "
                "return-theorems=unknown\n",
                provider_schema_version, exported, rejected);
  generating_provider_facts = false;
  // Discard GIMPLE/SSA pointers before GCC transforms or unloads the bodies.
  // The generic transport owns and GC-marks its independent tree payloads.
  reset();
  reset_value_expansion_cache();
}

ReturnedObjectEffect returned_object_effect(const gimple *statement) {
  ReturnedObjectEffect unknown;
  if (!summaries_built || generating_provider_facts || cfun == nullptr ||
      cfun->decl == NULL_TREE || statement == nullptr ||
      !is_gimple_call(statement) || !pointer_value(gimple_call_lhs(statement)) ||
      (gimple_call_flags(statement) & (ECF_RETURNS_TWICE | ECF_NORETURN)) != 0 ||
      !analysis_budget::available())
    return unknown;
  // IPA has not necessarily propagated NOTHROW to the outer call yet.  A
  // completed bound-body theorem below proves every admitted builtin is
  // nothrow.  Still reject any caller exceptional/abnormal continuation so
  // consumers cannot initialize the returned lifetime on such an edge.
  basic_block call_block = gimple_bb(statement);
  if (call_block == nullptr)
    return unknown;
  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, call_block->succs)
    if (!returned_normal_edge(successor))
      return unknown;
  cgraph_node *caller = cgraph_node::get(cfun->decl);
  cgraph_node *target = bound_target(statement, caller);
  if (!relative_original_node(target))
    return unknown;
  cgraph_edge *edge = caller->get_edge(const_cast<gimple *>(statement));
  if (edge == nullptr || edge->callee != target ||
      gimple_call_fndecl(statement) != target->decl)
    return unknown;
  auto found = returned_object_summaries.find(target->decl);
  if (found == returned_object_summaries.end()) {
    if (returned_object_summaries.size() >= max_relative_release_summaries)
      return unknown;
    found = returned_object_summaries.emplace(
        target->decl, build_returned_object_summary(target)).first;
  }
  const ReturnedObjectSummary &summary = found->second;
  if (!summary.effect.resolved ||
      !relative_signature_matches(statement, target, summary.signature) ||
      !same_pointer_representation(TREE_TYPE(gimple_call_lhs(statement)),
                                   TREE_TYPE(summary.signature.function_type)))
    return unknown;
  if (dump_file != nullptr && (dump_flags & TDF_DETAILS))
    dump_printf(MSG_NOTE,
                "ownership-returned-object: function=%s lifetime=released "
                "nonnull=yes origin=body exact-local-identity=yes "
                "normal-return=yes live-fresh=no\n",
                IDENTIFIER_POINTER(summary.signature.assembler_name));
  return summary.effect;
}

namespace {

bool specialized_fresh_return(const gimple *statement, const FunctionSummary *summary) {
  if (summary == nullptr || !summary->returns_specializable_owned ||
      !summary->has_normal_return)
    return false;
  for (const ReturnAlternative &alternative : summary->returns) {
    if (alternative.kind != ReturnKind::realloc_parameter)
      continue;
    if (alternative.parameter_index >= gimple_call_num_args(statement))
      return false;
    tree actual = gimple_call_arg(statement, alternative.parameter_index);
    if (!pointer_value(actual) || TREE_CODE(actual) != INTEGER_CST || !integer_zerop(actual))
      return false;
  }
  return true;
}

}  // namespace

bool call_returns_fresh_owned(const gimple *statement) {
  if (statement == nullptr || !is_gimple_call(statement) ||
      !pointer_value(gimple_call_lhs(statement))) {
    ++statistics.rejected_queries;
    return false;
  }
  const FunctionSummary *summary = summary_for_call(statement);
  if (summary == nullptr) {
    ++statistics.rejected_queries;
    return false;
  }
  ++statistics.resolved_queries;
  return summary->returns_fresh_owned || specialized_fresh_return(statement, summary);
}

bool call_returns_definitely_fresh_owned(const gimple *statement) {
  const FunctionSummary *summary = summary_for_call(statement);
  if (summary == nullptr ||
      (!summary->returns_fresh_owned && !specialized_fresh_return(statement, summary)) ||
      !summary->has_normal_return ||
      summary->returns.empty())
    return false;
  for (const ReturnAlternative &alternative : summary->returns)
    if ((alternative.kind != ReturnKind::fresh_owned &&
         alternative.kind != ReturnKind::realloc_parameter) || alternative.derived)
      return false;
  return true;
}

std::vector<location_t> call_fresh_origin_locations(
    const gimple *statement) {
  std::vector<location_t> result;
  const FunctionSummary *summary = summary_for_call(statement);
  if (summary == nullptr ||
      (!summary->returns_fresh_owned && !specialized_fresh_return(statement, summary)))
    return result;
  for (const ReturnAlternative &alternative : summary->returns) {
    if ((alternative.kind != ReturnKind::fresh_owned &&
         alternative.kind != ReturnKind::realloc_parameter) ||
        LOCATION_LOCUS(alternative.source_location) == UNKNOWN_LOCATION)
      continue;
    if (std::find(result.begin(), result.end(),
                  alternative.source_location) == result.end())
      result.push_back(alternative.source_location);
  }
  return result;
}

bool call_returns_argument(const gimple *statement,
                           unsigned int argument_index) {
  if (statement == nullptr || !is_gimple_call(statement) ||
      argument_index >= gimple_call_num_args(statement) ||
      !pointer_value(gimple_call_arg(statement, argument_index)))
    return false;
  const FunctionSummary *summary = summary_for_call(statement);
  if (summary == nullptr) {
    if (gcc_call_effect::known_library_returns_argument(as_a<const gcall *>(statement), argument_index))
      return true;
    const int flags =
        gimple_call_return_flags(as_a<const gcall *>(statement));
    return (flags & ERF_RETURNS_ARG) != 0 &&
           static_cast<unsigned int>(flags & ERF_RETURN_ARG_MASK) ==
               argument_index;
  }
  if (!summary->has_normal_return ||
      summary->returns.empty() ||
      argument_index >= summary->parameters.size() ||
      !summary->parameters[argument_index].pointer)
    return false;
  for (const ReturnAlternative &alternative : summary->returns)
    if (alternative.kind != ReturnKind::parameter ||
        alternative.parameter_index != argument_index ||
        alternative.derived)
      return false;
  return true;
}

tree returned_argument(const gimple *statement) {
  if (statement == nullptr || !is_gimple_call(statement) ||
      !pointer_value(gimple_call_lhs(statement)))
    return NULL_TREE;
  for (unsigned int index = 0; index < gimple_call_num_args(statement); ++index)
    if (call_returns_argument(statement, index))
      return gimple_call_arg(statement, index);
  return NULL_TREE;
}

bool conditional_borrow(const gimple *call, unsigned int argument_index,
                        BorrowCondition *condition) {
  if (generating_provider_facts || call == nullptr || !is_gimple_call(call) ||
      cfun == nullptr || condition == nullptr || !analysis_budget::available() ||
      (gimple_call_flags(call) & (ECF_NORETURN | ECF_RETURNS_TWICE)) != 0)
    return false;
  cgraph_node *caller = cgraph_node::get(cfun->decl);
  cgraph_node *target = bound_target(call, caller);
  if (!relative_original_node(target) || gimple_call_fndecl(call) != target->decl)
    return false;
  auto found = conditional_borrow_summaries.find(target->decl);
  if (found == conditional_borrow_summaries.end()) {
    if (conditional_borrow_summaries.size() >= max_relative_release_summaries)
      return false;
    found = conditional_borrow_summaries.emplace(target->decl, build_conditional_borrow(target)).first;
  }
  const ConditionalBorrowSummary &summary = found->second;
  if (!summary.signature.usable || argument_index != summary.pointer_index ||
      !relative_signature_matches(call, target, summary.signature))
    return false;
  tree selector = gimple_call_arg(call, summary.selector_index);
  // An unconstrained function input is a witness only at a public entry.
  // Private callers need an invocation-context proof; do not invent an input
  // that their actual callers might never supply.
  if (TREE_CODE(selector) == SSA_NAME && !TREE_PUBLIC(cfun->decl))
    return false;
  if (!INTEGRAL_TYPE_P(TREE_TYPE(selector)) ||
      (TREE_CODE(selector) != INTEGER_CST &&
       (TREE_CODE(selector) != SSA_NAME || !SSA_NAME_IS_DEFAULT_DEF(selector) ||
        SSA_NAME_VAR(selector) == NULL_TREE || TREE_CODE(SSA_NAME_VAR(selector)) != PARM_DECL ||
        DECL_CONTEXT(SSA_NAME_VAR(selector)) != cfun->decl)))
    return false;
  *condition = {selector, summary.when_zero, summary.guard_location};
  return true;
}

std::vector<MemoryAccessEffect> memory_access_effects(const gimple *call) {
  std::vector<MemoryAccessEffect> result;
  if (generating_provider_facts || call == nullptr || !is_gimple_call(call) ||
      cfun == nullptr || cfun->decl == NULL_TREE || gimple_has_volatile_ops(call) ||
      gimple_vuse(call) == NULL_TREE || !analysis_budget::available())
    return result;
  cgraph_node *caller = cgraph_node::get(cfun->decl);
  cgraph_node *target = bound_target(call, caller);
  if (!relative_original_node(target))
    return result;
  cgraph_edge *edge = caller->get_edge(const_cast<gimple *>(call));
  if (edge == nullptr || edge->callee != target || gimple_call_fndecl(call) != target->decl)
    return result;
  auto found = relative_access_summaries.find(target->decl);
  if (found == relative_access_summaries.end()) {
    if (relative_access_summaries.size() >= max_relative_release_summaries)
      return result;
    push_cfun(target->get_fun());
    RelativeAccessSummary built = build_relative_access_summary(target);
    pop_cfun();
    found = relative_access_summaries.emplace(target->decl, built).first;
  }
  const RelativeAccessSummary &summary = found->second;
  if (!summary.signature.usable || !relative_signature_matches(call, target, summary.signature))
    return result;
  RelativeReferenceInstantiator instantiator(summary.signature, call, caller);
  tree reference = instantiator.reference(summary.signature.reference);
  if (!pointer_value(reference) || TREE_TYPE(reference) != TREE_TYPE(summary.signature.reference))
    return result;
  result.push_back({reference, summary.access_location, summary.access});
  return result;
}

std::vector<MemoryReleaseEffect> memory_release_effects(const gimple *call) {
  std::vector<MemoryReleaseEffect> result;
  if (generating_provider_facts || call == nullptr || !is_gimple_call(call) ||
      cfun == nullptr || cfun->decl == NULL_TREE ||
      gimple_has_volatile_ops(call) || gimple_vuse(call) == NULL_TREE ||
      !analysis_budget::available())
    return result;
  cgraph_node *caller = cgraph_node::get(cfun->decl);
  cgraph_node *target = bound_target(call, caller);
  if (!relative_original_node(target))
    return result;
  // An alias edge may share a body but is outside this initial exact-decl
  // theorem.  Do not silently map aliases, thunks or changed signatures.
  cgraph_edge *edge = caller->get_edge(const_cast<gimple *>(call));
  if (edge == nullptr || edge->callee != target ||
      gimple_call_fndecl(call) != target->decl)
    return result;
  auto found = relative_release_summaries.find(target->decl);
  if (found == relative_release_summaries.end()) {
    if (relative_release_summaries.size() >= max_relative_release_summaries)
      return result;
    found = relative_release_summaries.emplace(
        target->decl, build_relative_release_summary(target)).first;
  }
  const RelativeReleaseSummary &summary = found->second;
  if (!summary.usable || !relative_signature_matches(call, target, summary))
    return result;
  RelativeReferenceInstantiator instantiator(summary, call, caller);
  tree reference = instantiator.reference(summary.reference);
  if (!pointer_value(reference) ||
      TREE_TYPE(reference) != TREE_TYPE(summary.reference))
    return result;
  result.push_back({reference, summary.release_location});
  if (dump_file != nullptr && (dump_flags & TDF_DETAILS))
    dump_printf(MSG_NOTE,
                "ownership-relative-release: function=%s origin=body "
                "entry-memory=yes normal-return=yes effects=1\n",
                IDENTIFIER_POINTER(summary.assembler_name));
  return result;
}

ArgumentEffect argument_effect(const gimple *statement,
                               unsigned int argument_index) {
  if (statement == nullptr || !is_gimple_call(statement) ||
      argument_index >= gimple_call_num_args(statement) ||
      !pointer_value(gimple_call_arg(statement, argument_index))) {
    ++statistics.rejected_queries;
    return ArgumentEffect{};
  }
  ArgumentEffect result;
  const FunctionSummary *summary = summary_for_call(statement);
  if (summary == nullptr)
    summary = provider_summary_for_call(statement);
  if (summary != nullptr && argument_index < summary->parameters.size() &&
      summary->parameters[argument_index].pointer) {
    result.resolved = true;
    result.fates = summary->parameters[argument_index].fates;
    result.may_dereference =
        summary->parameters[argument_index].may_dereference;
    const ProofOrigin origin = summary->provider_imported
                                   ? ProofOrigin::provider_body
                                   : ProofOrigin::body;
    result.ownership_origin = origin;
    result.access = summary->parameters[argument_index].access;
    if (result.has_access_guarantee()) {
      result.may_dereference = true;
      result.access_origin = origin;
    }
  }

  const bool mixed_fates = result.fates != fate_none &&
                           (result.fates & (result.fates - 1)) != 0;
  if ((!result.resolved || result.fates == fate_unknown || mixed_fates) &&
      invocation_definitely_frees(statement, argument_index,
                                   &result.release_origin)) {
    result.resolved = true;
    result.fates = fate_freed;
    result.ownership_origin = ProofOrigin::body;
    // Only ownership was refined.  Access/return guarantees and the ordinary
    // metadata/trusted-contract overlays below retain their existing meaning.
  }

  const gcc_call_effect::ArgumentEffect gcc_effect =
      gcc_call_effect::argument_effect(as_a<const gcall *>(statement),
                                       argument_index);
  if (gcc_effect.no_free_no_capture) {
    result.resolved = true;
    result.fates = fate_live;
    result.ownership_origin =
        (gcc_effect.no_free_no_capture_source == gcc_call_effect::ProofSource::standard_stdio_model ||
         gcc_effect.no_free_no_capture_source == gcc_call_effect::ProofSource::standard_library_model)
            ? ProofOrigin::library_model : ProofOrigin::gcc_metadata;
  }
  if (gcc_effect.must_read) {
    result.access = AccessGuarantee::must_access;
    result.may_dereference = true;
    result.access_origin = ProofOrigin::gcc_metadata;
  }
  if (gcc_effect.requires_live && !result.has_access_guarantee()) {
    result.access = AccessGuarantee::requires_live;
    result.may_dereference = true;
    result.access_origin = ProofOrigin::gcc_metadata;
  }

  // These declaration attributes are explicit trusted axioms.  They are
  // intentionally distinct from GCC's ordinary access attribute, whose
  // documented semantics do not guarantee that an access occurs.
  const trusted_contract::PointerArgumentEffect contract =
      generating_provider_facts
          ? trusted_contract::PointerArgumentEffect{}
          : trusted_contract::pointer_argument_effect_for_call(statement,
                                                              argument_index);
  if (contract.no_free_no_capture) {
    result.resolved = true;
    result.fates = fate_live;
    result.ownership_origin = ProofOrigin::user_contract;
  }
  switch (contract.access) {
    case trusted_contract::AccessGuarantee::none:
      break;
    case trusted_contract::AccessGuarantee::must_access:
      result.access = AccessGuarantee::must_access;
      result.may_dereference = true;
      result.access_origin = ProofOrigin::user_contract;
      break;
    case trusted_contract::AccessGuarantee::access_if_nonnull:
      result.access = AccessGuarantee::access_if_nonnull;
      result.may_dereference = true;
      result.access_origin = ProofOrigin::user_contract;
      break;
  }

  // Counterfactuals refine only unresolved external effects. Source/provider
  // facts and authoritative metadata/contracts are never overridden, and
  // producer facts never incorporate a user-selected assumption.
  tree modeled_declaration = gimple_call_fndecl(statement);
  if (!generating_provider_facts && summary == nullptr && !result.resolved &&
      configured_callee_model != CalleeModel::unknown &&
      argument_index == configured_callee_argument &&
      modeled_declaration != NULL_TREE && !gimple_has_body_p(modeled_declaration) &&
      configured_callee_symbol == IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(modeled_declaration))) {
    result.resolved = true;
    result.fates = configured_callee_model == CalleeModel::borrow ? fate_live : fate_freed;
    result.ownership_origin = ProofOrigin::user_assumption;
  }

  // A side-effect-free or borrowing call may return ownership, including
  // through an integer representation or an aggregate.  Unless the returned
  // value is expanded as an exact argument, require GCC's independent proof
  // that this argument cannot reach the result before preserving ownership.
  const gcall *call = as_a<const gcall *>(statement);
  const int return_flags = gimple_call_return_flags(call);
  const int argument_flags = gimple_call_arg_flags(call, argument_index);
#if defined(EAF_NOT_RETURNED_DIRECTLY) && defined(EAF_NOT_RETURNED_INDIRECTLY)
  const int not_returned =
      EAF_NOT_RETURNED_DIRECTLY | EAF_NOT_RETURNED_INDIRECTLY;
  const bool proves_not_returned =
      (argument_flags & not_returned) == not_returned;
#else
  // GCC 10 has no separate returned-argument EAF facts.  Its noescape bit
  // alone is not a replacement; only an unused argument excludes return.
  const bool proves_not_returned = (argument_flags & EAF_UNUSED) != 0;
#endif
  if (result.resolved && result.fates == fate_live &&
      gimple_call_lhs(statement) != NULL_TREE &&
      returned_argument(statement) == NULL_TREE &&
      (return_flags & ERF_NOALIAS) == 0 &&
      !gcc_effect.scalar_return_no_pointer &&
      !proves_not_returned) {
    result.resolved = false;
    result.fates = fate_unknown;
  }

  // Declaration overlays may replace the body effect.  Never retain a
  // release trace after ownership has become borrowing or unresolved.
  if (!result.definitely_freed() || result.ownership_origin != ProofOrigin::body)
    result.release_origin = ReleaseOrigin{};

  if (!generating_provider_facts) {
    const bool needs_review = !result.resolved ||
        (result.fates & fate_unknown) != 0 ||
        (result.fates != fate_none && (result.fates & (result.fates - 1)) != 0);
    report::record_callee_effect(statement, argument_index,
                                fate_set_name(result.fates),
                                summary != nullptr && !summary->provider_imported,
                                needs_review);
  }

  if (result.resolved || result.has_access_guarantee())
    ++statistics.resolved_queries;
  else
    ++statistics.rejected_queries;
  return result;
}

void dump_statistics() {
  if (dump_file == nullptr)
    return;
  dump_printf(MSG_NOTE,
              "ownership-summary totals: functions=%u usable=%u "
              "unsupported=%u recursive=%u fresh-returners=%u rounds=%u "
              "evaluations=%u resolved-queries=%lu rejected-queries=%lu\n",
              statistics.functions, statistics.usable,
              statistics.unsupported, statistics.recursive,
              statistics.fresh_returners, statistics.fixed_point_rounds,
              statistics.evaluations,
              statistics.resolved_queries, statistics.rejected_queries);
  dump_printf(MSG_NOTE,
              "ownership-provider consumer: imported=%u rejected=%u "
              "return-theorems=unknown\n",
              statistics.provider_imported, statistics.provider_rejected);
}

const char *fate_set_name(unsigned int fates) {
  switch (fates) {
    case fate_none:
      return "none";
    case fate_live:
      return "live";
    case fate_freed:
      return "freed";
    case fate_escaped:
      return "escaped";
    case fate_unknown:
      return "unknown";
    case fate_live | fate_freed:
      return "live|freed";
    case fate_live | fate_escaped:
      return "live|escaped";
    case fate_freed | fate_escaped:
      return "freed|escaped";
    case fate_live | fate_freed | fate_escaped:
      return "live|freed|escaped";
    default:
      return "mixed-with-unknown";
  }
}

const char *access_guarantee_name(AccessGuarantee access) {
  switch (access) {
    case AccessGuarantee::none:
      return "none";
    case AccessGuarantee::must_access:
      return "must-access";
    case AccessGuarantee::access_if_nonnull:
      return "access-if-nonnull";
    case AccessGuarantee::requires_live:
      return "requires-live";
    case AccessGuarantee::requires_live_if_nonnull:
      return "requires-live-if-nonnull";
  }
  return "none";
}

bool access_is_live_requirement(AccessGuarantee access) {
  return access == AccessGuarantee::requires_live ||
         access == AccessGuarantee::requires_live_if_nonnull;
}

bool access_is_conditional(AccessGuarantee access) {
  return access == AccessGuarantee::access_if_nonnull ||
         access == AccessGuarantee::requires_live_if_nonnull;
}

const char *proof_origin_name(ProofOrigin origin) {
  switch (origin) {
    case ProofOrigin::none:
      return "none";
    case ProofOrigin::body:
      return "body";
    case ProofOrigin::provider_body:
      return "provider-body";
    case ProofOrigin::gcc_metadata:
      return "gcc-metadata";
    case ProofOrigin::user_contract:
      return "user-contract";
    case ProofOrigin::user_assumption:
      return "user-assumption";
    case ProofOrigin::library_model:
      return "library-model";
  }
  return "none";
}

}  // namespace ownership_summary
}  // namespace static_analyzer
