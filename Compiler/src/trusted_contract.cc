#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "stringpool.h"
#include "plugin.h"
#include "attribs.h"
#include "diagnostic-core.h"
#include "input.h"

#include "static_analyzer/trusted_contract.h"

namespace static_analyzer {
namespace trusted_contract {
namespace {

constexpr const char borrow_attribute_name[] = "static_analyzer_borrow";
constexpr const char must_access_attribute_name[] =
    "static_analyzer_must_access";
constexpr const char access_if_nonnull_attribute_name[] =
    "static_analyzer_access_if_nonnull";

location_t declaration_location(tree declaration) {
  if (declaration != NULL_TREE && DECL_P(declaration) &&
      DECL_SOURCE_LOCATION(declaration) != UNKNOWN_LOCATION)
    return DECL_SOURCE_LOCATION(declaration);
  return input_location;
}

// GCC's type_argument_type/type_num_arguments count every TYPE_ARG_TYPES
// entry, including a METHOD_TYPE's artificial this.  Source contract indices
// instead count only written parameters.  Do not use C++ frontend helpers
// here: this plugin is also loaded into the C frontend.
bool source_parameter_layout(tree declaration, unsigned int *offset,
                             unsigned int *count) {
  if (declaration == NULL_TREE || TREE_CODE(declaration) != FUNCTION_DECL)
    return false;
  tree function_type = TREE_TYPE(declaration);
  if (function_type == NULL_TREE || !FUNC_OR_METHOD_TYPE_P(function_type) ||
      !prototype_p(function_type))
    return false;

  // GCC adds/removes in-charge and VTT parameters when cloning constructors
  // and destructors.  Their source-to-ABI mapping is deliberately unsupported
  // rather than treating the extra pointer operand as a source parameter.
  if (DECL_CXX_CONSTRUCTOR_P(declaration) ||
      DECL_CXX_DESTRUCTOR_P(declaration))
    return false;

  const unsigned int first = TREE_CODE(function_type) == METHOD_TYPE ? 1 : 0;
  const unsigned int total = type_num_arguments(function_type);
  if (total < first)
    return false;
  if (first != 0 && !POINTER_TYPE_P(type_argument_type(function_type, 1)))
    return false;

  // Prototypes need not retain PARM_DECLs.  When present, require a complete
  // ordinary layout: just this may be artificial.  This also fails closed on
  // additional target/frontend generated parameter kinds.
  if (DECL_ARGUMENTS(declaration) != NULL_TREE) {
    unsigned int index = 0;
    for (tree parameter = DECL_ARGUMENTS(declaration); parameter != NULL_TREE;
         parameter = TREE_CHAIN(parameter), ++index) {
      if (TREE_CODE(parameter) != PARM_DECL ||
          index >= total ||
          (DECL_ARTIFICIAL(parameter) != (first != 0 && index == 0)))
        return false;
    }
    if (index != total)
      return false;
  }
  *offset = first;
  *count = total - first;
  return true;
}

tree declared_parameter_type(tree declaration,
                             unsigned int parameter_index) {
  unsigned int offset = 0;
  unsigned int count = 0;
  if (parameter_index == 0 ||
      !source_parameter_layout(declaration, &offset, &count) ||
      parameter_index > count)
    return NULL_TREE;
  tree parameter_type =
      type_argument_type(TREE_TYPE(declaration), parameter_index + offset);
  if (parameter_type == NULL_TREE || parameter_type == error_mark_node ||
      VOID_TYPE_P(parameter_type))
    return NULL_TREE;
  return parameter_type;
}

bool attribute_parameter_index(tree arguments, unsigned int *result) {
  if (arguments == NULL_TREE || TREE_CHAIN(arguments) != NULL_TREE)
    return false;
  tree value = TREE_VALUE(arguments);
  if (value == NULL_TREE || TREE_CODE(value) != INTEGER_CST ||
      !tree_fits_uhwi_p(value))
    return false;
  const unsigned HOST_WIDE_INT converted = tree_to_uhwi(value);
  if (converted == 0 || converted > UINT_MAX)
    return false;
  *result = static_cast<unsigned int>(converted);
  return true;
}

tree handle_pointer_contract_attribute(tree *node, tree name, tree arguments,
                                       int, bool *no_add_attributes) {
  tree declaration = node == nullptr ? NULL_TREE : node[0];
  const location_t location = declaration_location(declaration);
  if (declaration == NULL_TREE || TREE_CODE(declaration) != FUNCTION_DECL) {
    error_at(location, "%qE attribute applies only to a direct function "
                       "declaration", name);
    *no_add_attributes = true;
    return NULL_TREE;
  }

  tree function_type = TREE_TYPE(declaration);
  if (function_type == NULL_TREE || !FUNC_OR_METHOD_TYPE_P(function_type) ||
      !prototype_p(function_type)) {
    error_at(location, "%qE attribute requires a function prototype", name);
    *no_add_attributes = true;
    return NULL_TREE;
  }

  unsigned int parameter_index = 0;
  if (!attribute_parameter_index(arguments, &parameter_index)) {
    error_at(location, "%qE attribute requires one positive 1-based "
                       "parameter index", name);
    *no_add_attributes = true;
    return NULL_TREE;
  }

  unsigned int offset = 0;
  unsigned int parameter_count = 0;
  if (!source_parameter_layout(declaration, &offset, &parameter_count)) {
    error_at(location, "%qE attribute requires an ordinary function or "
                       "member function parameter layout", name);
    *no_add_attributes = true;
    return NULL_TREE;
  }

  if (parameter_index > parameter_count) {
    error_at(location, "%qE attribute parameter index %u exceeds the "
                       "function parameter count", name, parameter_index);
    *no_add_attributes = true;
    return NULL_TREE;
  }

  tree parameter_type =
      declared_parameter_type(declaration, parameter_index);
  if (parameter_type == NULL_TREE || !POINTER_TYPE_P(parameter_type)) {
    error_at(location, "%qE attribute parameter %u is not a pointer "
                       "parameter", name, parameter_index);
    *no_add_attributes = true;
    return NULL_TREE;
  }

  return NULL_TREE;
}

// Attribute syntax:
//
//   __attribute__((static_analyzer_borrow(1)))
//   __attribute__((static_analyzer_must_access(1)))
//   __attribute__((static_analyzer_access_if_nonnull(1)))
//
// A contract is a trusted axiom supplied by the declaration author.  The
// handler proves only that the axiom is well-formed.  It does not inspect or
// validate the function body, and a dishonest declaration can therefore make
// a client analysis unsound.  Reports consuming these effects must retain
// user-contract provenance rather than presenting them as body proofs.
const attribute_spec borrow_attribute = {
    borrow_attribute_name, 1, 1, true, false, false, false,
    handle_pointer_contract_attribute, nullptr};
const attribute_spec must_access_attribute = {
    must_access_attribute_name, 1, 1, true, false, false, false,
    handle_pointer_contract_attribute, nullptr};
const attribute_spec access_if_nonnull_attribute = {
    access_if_nonnull_attribute_name, 1, 1, true, false, false, false,
    handle_pointer_contract_attribute, nullptr};

bool exact_attribute_has_parameter(tree declaration, const char *name,
                                   unsigned int parameter_index) {
  if (declaration == NULL_TREE || TREE_CODE(declaration) != FUNCTION_DECL)
    return false;
  for (tree attribute =
           lookup_attribute(name, DECL_ATTRIBUTES(declaration));
       attribute != NULL_TREE;
       attribute = lookup_attribute(name, TREE_CHAIN(attribute))) {
    unsigned int attributed_parameter = 0;
    if (attribute_parameter_index(TREE_VALUE(attribute),
                                  &attributed_parameter) &&
        attributed_parameter == parameter_index)
      return true;
  }
  return false;
}

}  // namespace

void register_attributes(void *, void *) {
  register_attribute(&borrow_attribute);
  register_attribute(&must_access_attribute);
  register_attribute(&access_if_nonnull_attribute);
}

PointerArgumentEffect pointer_argument_effect(
    tree function_declaration, unsigned int parameter_index) {
  PointerArgumentEffect result;
  tree parameter_type =
      declared_parameter_type(function_declaration, parameter_index);
  if (parameter_type == NULL_TREE || !POINTER_TYPE_P(parameter_type))
    return result;

  result.no_free_no_capture = exact_attribute_has_parameter(
      function_declaration, borrow_attribute_name, parameter_index);
  const bool must_access = exact_attribute_has_parameter(
      function_declaration, must_access_attribute_name, parameter_index);
  const bool access_if_nonnull = exact_attribute_has_parameter(
      function_declaration, access_if_nonnull_attribute_name,
      parameter_index);

  if (must_access)
    result.access = AccessGuarantee::must_access;
  else if (access_if_nonnull)
    result.access = AccessGuarantee::access_if_nonnull;
  result.resolved = result.no_free_no_capture || must_access ||
                    access_if_nonnull;
  return result;
}

PointerArgumentEffect pointer_argument_effect_for_call(
    const gimple *statement, unsigned int argument_index) {
  if (statement == nullptr || !is_gimple_call(statement) ||
      argument_index >= gimple_call_num_args(statement))
    return PointerArgumentEffect{};
  tree declaration = gimple_call_fndecl(statement);
  unsigned int offset = 0;
  unsigned int count = 0;
  if (!source_parameter_layout(declaration, &offset, &count) ||
      argument_index < offset || argument_index - offset >= count)
    return PointerArgumentEffect{};

  // Attributes can survive on IPA clones whose argument list was rewritten.
  // Such a declaration is not an independently stated source contract.
  if (DECL_ABSTRACT_ORIGIN(declaration) != NULL_TREE &&
      DECL_ABSTRACT_ORIGIN(declaration) != declaration)
    return PointerArgumentEffect{};

  const unsigned int actual_count = gimple_call_num_args(statement);
  const unsigned int fixed_count = count + offset;
  if (actual_count < fixed_count ||
      (!stdarg_p(TREE_TYPE(declaration)) && actual_count != fixed_count))
    return PointerArgumentEffect{};

  return pointer_argument_effect(declaration, argument_index - offset + 1);
}

}  // namespace trusted_contract
}  // namespace static_analyzer
