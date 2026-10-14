#include "gcc-plugin.h"
#include "bversion.h"
#include "tree.h"
#include "gimple.h"
#include "basic-block.h"
#include "function.h"
#include "stringpool.h"
#include "attribs.h"

#include "static_analyzer/analysis_budget.h"
#include "static_analyzer/memory_value.h"
#include "static_analyzer/ownership_call.h"

#include <cstring>

namespace static_analyzer {
namespace {

bool proof_only = true;

tree direct_callee(const gimple *statement) {
  if (statement == nullptr || !is_gimple_call(statement))
    return NULL_TREE;
  tree declaration = gimple_call_fndecl(statement);
  return declaration != NULL_TREE && TREE_CODE(declaration) == FUNCTION_DECL
             ? declaration
             : NULL_TREE;
}

const char *callee_name(tree declaration) {
  if (declaration == NULL_TREE || DECL_NAME(declaration) == NULL_TREE)
    return nullptr;
  return IDENTIFIER_POINTER(DECL_NAME(declaration));
}

bool named(tree declaration, const char *expected) {
  const char *actual = callee_name(declaration);
  return actual != nullptr && std::strcmp(actual, expected) == 0;
}

bool normal_builtin(tree declaration, enum built_in_function code) {
  return fndecl_built_in_p(declaration, BUILT_IN_NORMAL) &&
         DECL_FUNCTION_CODE(declaration) == code;
}

bool same_pointer_representation(tree left, tree right) {
  if (left == NULL_TREE || right == NULL_TREE ||
      TREE_TYPE(left) == NULL_TREE || TREE_TYPE(right) == NULL_TREE)
    return false;
  tree left_type = TREE_TYPE(left);
  tree right_type = TREE_TYPE(right);
  return POINTER_TYPE_P(left_type) && POINTER_TYPE_P(right_type) &&
         TYPE_PRECISION(left_type) == TYPE_PRECISION(right_type) &&
         TYPE_MODE(left_type) == TYPE_MODE(right_type) &&
         TYPE_ADDR_SPACE(TREE_TYPE(left_type)) ==
             TYPE_ADDR_SPACE(TREE_TYPE(right_type));
}

bool current_function_statement(const gimple *statement) {
  basic_block block = statement == nullptr ? nullptr : gimple_bb(statement);
  return cfun != nullptr && cfun->cfg != nullptr && block != nullptr &&
         block->index >= 0 && block->index < last_basic_block_for_fn(cfun) &&
         BASIC_BLOCK_FOR_FN(cfun, block->index) == block;
}

bool proved_null_pointer(tree value) {
  // Keep this proof independent of allocation classification and of generic
  // origin expansion: asking whether realloc's input is NULL must not recurse
  // back into classifying the very realloc result being inspected.  No PHI
  // selection or call-result inference is part of this call-local theorem.
  for (unsigned int depth = 0; depth < 32; ++depth) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::lifetime_discovery) ||
        value == NULL_TREE || TREE_TYPE(value) == NULL_TREE ||
        !POINTER_TYPE_P(TREE_TYPE(value)))
      return false;
    if (TREE_CODE(value) == INTEGER_CST)
      return integer_zerop(value);
    if (TREE_CODE(value) != SSA_NAME)
      return false;
    gimple *definition = SSA_NAME_DEF_STMT(value);
    if (!current_function_statement(definition) ||
        !is_gimple_assign(definition) || gimple_has_volatile_ops(definition))
      return false;
    tree rhs = gimple_assign_rhs1(definition);
    const enum tree_code code = gimple_assign_rhs_code(definition);
    if ((gimple_assign_single_p(definition) || code == NOP_EXPR ||
         code == CONVERT_EXPR) &&
        same_pointer_representation(value, rhs) &&
        (TREE_CODE(rhs) == SSA_NAME || TREE_CODE(rhs) == INTEGER_CST)) {
      value = rhs;
      continue;
    }
    if (!gimple_assign_load_p(definition))
      return false;
    const MemoryValueResult memory = resolve_pointer_load(definition);
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::lifetime_discovery,
            memory.visited_states + memory.walked_vdefs + 1))
      return false;
    tree stored = memory.unique_stored_value();
    if (stored == NULL_TREE || memory.definitions.size() != 1 ||
        memory.definitions.front().statement == nullptr ||
        gimple_has_volatile_ops(memory.definitions.front().statement) ||
        !same_pointer_representation(value, stored) ||
        !same_pointer_representation(value, memory.load_reference) ||
        !same_pointer_representation(
            value, memory.definitions.front().memory_reference))
      return false;
    value = stored;
  }
  return false;
}

bool c_allocation_builtin(tree declaration) {
  return normal_builtin(declaration, BUILT_IN_MALLOC) ||
         normal_builtin(declaration, BUILT_IN_CALLOC) ||
         normal_builtin(declaration, BUILT_IN_ALIGNED_ALLOC) ||
         normal_builtin(declaration, BUILT_IN_STRDUP) ||
         normal_builtin(declaration, BUILT_IN_STRNDUP);
}

bool stack_allocation_builtin(tree declaration) {
  // GCC marks alloca-like declarations as malloc-like because the returned
  // storage is fresh/no-alias.  DECL_IS_MALLOC is therefore not, by itself,
  // proof of heap ownership.  These objects have automatic lifetime and must
  // never become SA001 allocation sites.
  return normal_builtin(declaration, BUILT_IN_ALLOCA) ||
         normal_builtin(declaration, BUILT_IN_ALLOCA_WITH_ALIGN) ||
         normal_builtin(declaration, BUILT_IN_ALLOCA_WITH_ALIGN_AND_MAX);
}

bool c_allocation_name(const gimple *statement, tree declaration) {
  if (proof_only)
    return false;
  const unsigned int arguments = gimple_call_num_args(statement);
  return ((named(declaration, "malloc") || named(declaration, "xmalloc")) &&
          arguments == 1) ||
         ((named(declaration, "calloc") || named(declaration, "xcalloc")) &&
          arguments == 2) ||
         ((named(declaration, "strdup") || named(declaration, "xstrdup")) &&
          arguments == 1) ||
         (named(declaration, "strndup") && arguments == 2) ||
         (named(declaration, "aligned_alloc") && arguments == 2);
}

tree strip_deallocator_decl(tree value) {
  if (value != NULL_TREE && TREE_CODE(value) == ADDR_EXPR)
    value = TREE_OPERAND(value, 0);
  return value != NULL_TREE && TREE_CODE(value) == FUNCTION_DECL
             ? value
             : NULL_TREE;
}

bool free_deallocator_decl_p(tree declaration) {
  // Here the FUNCTION_DECL is named by an explicit GCC
  // malloc(DEALLOCATOR, INDEX) contract.  In proof-only mode, however, the
  // attribute still does not prove that a user declaration merely spelled
  // "free" has the semantics of GCC's normal free builtin.  Require the
  // builtin identity there; exploratory mode retains the name heuristic for
  // compatibility with projects that compile with builtins disabled.
  return declaration != NULL_TREE &&
         (normal_builtin(declaration, BUILT_IN_FREE) ||
          (!proof_only &&
           (named(declaration, "free") || named(declaration, "xfree"))));
}

bool malloc_attribute_is_free_compatible(tree declaration) {
  // A plain malloc attribute describes newly obtained no-alias storage but
  // does not name a release function.  Model it as heap ownership.  If the
  // declaration supplies malloc(DEALLOCATOR), only accept pairs this engine
  // can actually close with free/xfree; e.g. fopen/fclose is a different
  // resource family and must not become an SA001 heap false positive.
  bool names_compatible_deallocator = false;
  for (tree attributes = DECL_ATTRIBUTES(declaration);
       (attributes = lookup_attribute("malloc", attributes)) != NULL_TREE;
       attributes = TREE_CHAIN(attributes)) {
    tree arguments = TREE_VALUE(attributes);
    if (arguments == NULL_TREE || TREE_VALUE(arguments) == NULL_TREE)
      continue;
    if (!free_deallocator_decl_p(
            strip_deallocator_decl(TREE_VALUE(arguments))))
      return false;
    names_compatible_deallocator = true;
  }
  // GCC's plain malloc attribute proves fresh/no-alias storage, not an
  // obligation to release it with free.  Arena and garbage-collected APIs
  // commonly use that attribute.  Exploratory mode retains the historical
  // heuristic; proof-only requires malloc(DEALLOCATOR) evidence.
  return names_compatible_deallocator || !proof_only;
}

bool has_malloc_attribute(tree declaration) {
  return declaration != NULL_TREE &&
         lookup_attribute("malloc", DECL_ATTRIBUTES(declaration)) !=
             NULL_TREE;
}

bool pointer_result_p(const gimple *statement, tree declaration) {
  tree lhs = gimple_call_lhs(statement);
  tree function_type = TREE_TYPE(declaration);
  tree return_type =
      function_type == NULL_TREE ? NULL_TREE : TREE_TYPE(function_type);
  return lhs != NULL_TREE && TREE_TYPE(lhs) != NULL_TREE &&
         POINTER_TYPE_P(TREE_TYPE(lhs)) && return_type != NULL_TREE &&
         POINTER_TYPE_P(return_type);
}

bool call_came_from_new_or_delete(const gimple *statement) {
#if BUILDING_GCC_MAJOR >= 11
  return gimple_call_from_new_or_delete(
      as_a<const gcall *>(statement));
#else
  // GCC 10 has the operator predicates but predates the GIMPLE source flag.
  // Without that provenance bit proof-only cannot distinguish a new-expression
  // from an ordinary direct call to a class-specific allocation function.
  return !proof_only;
#endif
}

bool cpp_allocation_call_p(const gimple *statement, tree declaration) {
  // GCC's operator bit is authoritative.  Some older IPA pipelines clear it
  // when cloning declarations, so retain the exact C++ operator identifier as
  // a narrow fallback; C/C++ user identifiers cannot otherwise contain this
  // spelling.
  if (!DECL_IS_OPERATOR_NEW_P(declaration) &&
      !named(declaration, "operator new") &&
      !named(declaration, "operator new []"))
    return false;

  // GCC marks the replaceable global new/new[] declarations as malloc-like.
  // The malloc bit is also the explicit opt-in for custom allocation
  // operators.  Both are ownership-producing even when operator new is
  // invoked directly instead of through a new-expression.
  if (DECL_IS_MALLOC(declaration) ||
      DECL_IS_REPLACEABLE_OPERATOR_NEW_P(declaration))
    return true;

  // A class-specific ordinary operator new(size_t) is also the allocation
  // step of a new-expression.  Extra arguments mean placement/custom-new
  // semantics; without DECL_IS_MALLOC GCC does not prove that such a call
  // creates storage, so leave it unmodelled instead of inventing ownership.
  return gimple_call_num_args(statement) == 1 &&
         call_came_from_new_or_delete(statement);
}

bool second_formal_is_size_type(tree declaration) {
  tree arguments = TYPE_ARG_TYPES(TREE_TYPE(declaration));
  if (arguments == NULL_TREE || arguments == void_list_node)
    return false;
  arguments = TREE_CHAIN(arguments);
  if (arguments == NULL_TREE || arguments == void_list_node)
    return false;
  tree type = TREE_VALUE(arguments);
  return type != NULL_TREE && size_type_node != NULL_TREE &&
         TYPE_MAIN_VARIANT(type) == TYPE_MAIN_VARIANT(size_type_node);
}

bool cpp_deallocation_call_p(const gimple *statement, tree declaration) {
  // This is the same FUNCTION_DECL property used by GCC's
  // gimple_call_operator_delete_p helper.  Keeping the direct predicate also
  // makes the plugin source-compatible with GCC 10, where that helper was not
  // yet exported to plugins.
  if (!DECL_IS_OPERATOR_DELETE_P(declaration) &&
      !named(declaration, "operator delete") &&
      !named(declaration, "operator delete []"))
    return false;

  if (DECL_IS_REPLACEABLE_OPERATOR(declaration))
    return true;

  const unsigned int argument_count = gimple_call_num_args(statement);
  if (argument_count == 1)
    return true;

  // A non-replaceable two-argument delete can be a class-specific sized
  // delete or a placement-delete cleanup.  Only size_t is ownership evidence;
  // void*, references and arbitrary placement tokens are deliberately not.
  return argument_count == 2 && second_formal_is_size_type(declaration) &&
         call_came_from_new_or_delete(statement);
}

}  // namespace

void set_ownership_proof_only(bool enabled) {
  proof_only = enabled;
  // The early provider and the later analysis pass may see the same GIMPLE
  // pointers after GCC has transformed their bodies.  Start a new immutable
  // memory-value epoch here; nested summary/confinement cfun traversals may
  // reuse distinct statement keys within that epoch without caching mode-
  // or caller-specific realloc conclusions.
  memory_value::reset_function_cache();
}

bool ownership_allocation_call_p(const gimple *statement) {
  tree declaration = direct_callee(statement);
  if (declaration == NULL_TREE || gimple_call_lhs(statement) == NULL_TREE)
    return false;
  if (stack_allocation_builtin(declaration))
    return false;
  if (cpp_allocation_call_p(statement, declaration))
    return true;
  if (c_allocation_builtin(declaration) ||
      c_allocation_name(statement, declaration) ||
      ownership_null_reallocation_call_p(statement))
    return true;
  if (!pointer_result_p(statement, declaration))
    return false;
  if (proof_only)
    return malloc_attribute_is_free_compatible(declaration);
  return DECL_IS_MALLOC(declaration) &&
         malloc_attribute_is_free_compatible(declaration);
}

bool ownership_unproved_allocation_call_p(const gimple *statement) {
  if (!proof_only)
    return false;
  tree declaration = direct_callee(statement);
  if (declaration == NULL_TREE || gimple_call_lhs(statement) == NULL_TREE ||
      stack_allocation_builtin(declaration) ||
      !pointer_result_p(statement, declaration) ||
      ownership_allocation_call_p(statement))
    return false;
  const char *name = callee_name(declaration);
  const bool historical_x_allocator =
      name != nullptr &&
      (std::strcmp(name, "xmalloc") == 0 ||
       std::strcmp(name, "xcalloc") == 0 ||
       std::strcmp(name, "xstrdup") == 0 ||
       std::strcmp(name, "xrealloc") == 0);
  const bool unproved_cpp_allocation =
      DECL_IS_OPERATOR_NEW_P(declaration) &&
      gimple_call_num_args(statement) == 1;
  return DECL_IS_MALLOC(declaration) || has_malloc_attribute(declaration) ||
         historical_x_allocator ||
         unproved_cpp_allocation;
}

bool ownership_deallocation_call_p(const gimple *statement) {
  tree declaration = direct_callee(statement);
  if (declaration == NULL_TREE)
    return false;
  return (normal_builtin(declaration, BUILT_IN_FREE) &&
          gimple_call_num_args(statement) == 1) ||
         ((!proof_only &&
           (named(declaration, "free") || named(declaration, "cfree") ||
            named(declaration, "xfree"))) &&
          gimple_call_num_args(statement) == 1) ||
         cpp_deallocation_call_p(statement, declaration);
}

bool ownership_null_reallocation_call_p(const gimple *statement) {
  tree declaration = direct_callee(statement);
  if (declaration == NULL_TREE ||
      !normal_builtin(declaration, BUILT_IN_REALLOC) ||
      gimple_call_num_args(statement) != 2)
    return false;
  tree size = gimple_call_arg(statement, 1);
  // Zero-sized realloc has version/implementation-specific lifetime behavior.
  // Unknown sizes likewise cannot justify the positive-size malloc theorem.
  if (size == NULL_TREE || TREE_TYPE(size) == NULL_TREE ||
      !INTEGRAL_TYPE_P(TREE_TYPE(size)) || TREE_CODE(size) != INTEGER_CST ||
      !tree_fits_uhwi_p(size) || tree_to_uhwi(size) == 0)
    return false;
  return proved_null_pointer(gimple_call_arg(statement, 0));
}

bool ownership_reallocation_call_p(const gimple *statement) {
  tree declaration = direct_callee(statement);
  return declaration != NULL_TREE &&
         ((normal_builtin(declaration, BUILT_IN_REALLOC) &&
           gimple_call_num_args(statement) == 2) ||
          ((!proof_only &&
            (named(declaration, "realloc") || named(declaration, "xrealloc"))) &&
           gimple_call_num_args(statement) == 2)) &&
         !ownership_null_reallocation_call_p(statement);
}

}  // namespace static_analyzer
