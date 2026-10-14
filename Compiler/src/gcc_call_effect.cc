#include "gcc-plugin.h"
#include "bversion.h"
#include "tree.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-cfg.h"
#include "cgraph.h"
#include "stringpool.h"
#include "tm.h"

#if BUILDING_GCC_MAJOR >= 16
#include "attr-fnspec.h"
#endif

#include "static_analyzer/gcc_call_effect.h"
#include "static_analyzer/analysis_budget.h"

#include <cstring>
#include <vector>

namespace static_analyzer {
namespace gcc_call_effect {
namespace {

bool stdio_model_active = false;
bool consumer_models_active = false;
const char *stdio_model_reason = "not-initialized";

bool custom_stdio_symbol(tree declaration) {
  if (declaration == NULL_TREE || DECL_NAME(declaration) == NULL_TREE)
    return false;
  const char *name = IDENTIFIER_POINTER(DECL_NAME(declaration));
  return std::strcmp(name, "fopencookie") == 0 ||
         std::strcmp(name, "funopen") == 0 ||
         std::strcmp(name, "funopen2") == 0 ||
         std::strcmp(name, "register_printf_function") == 0 ||
         std::strcmp(name, "register_printf_specifier") == 0 ||
         std::strcmp(name, "register_printf_type") == 0;
}

bool stdout_reference(tree value) {
  tree base = value == NULL_TREE ? NULL_TREE : get_base_address(value);
  return base != NULL_TREE && TREE_CODE(base) == VAR_DECL &&
         DECL_NAME(base) != NULL_TREE &&
         std::strcmp(IDENTIFIER_POINTER(DECL_NAME(base)), "stdout") == 0;
}

enum class WideLibrary { none, copy, set, output };

bool declaration_has_implementation(tree declaration) {
  if (declaration == NULL_TREE)
    return false;
  cgraph_node *node = cgraph_node::get(declaration);
  return gimple_has_body_p(declaration) || (node != nullptr && node->definition);
}

// The LTO consumer has no C-family c_global_trees. Use target ABI properties
// and middle-end type nodes instead. -fshort-wchar does not change the system
// library ABI and therefore cannot use this model.
bool wide_scalar_type(tree type) {
  if (type == NULL_TREE || TREE_CODE(type) != INTEGER_TYPE || flag_short_wchar)
    return false;
  tree abi_type = NULL_TREE;
  if (std::strcmp(WCHAR_TYPE, "int") == 0)
    abi_type = integer_type_node;
  else if (std::strcmp(WCHAR_TYPE, "unsigned int") == 0)
    abi_type = unsigned_type_node;
  else if (std::strcmp(WCHAR_TYPE, "long int") == 0)
    abi_type = long_integer_type_node;
  else if (std::strcmp(WCHAR_TYPE, "long unsigned int") == 0 ||
           std::strcmp(WCHAR_TYPE, "unsigned long int") == 0)
    abi_type = long_unsigned_type_node;
  else if (std::strcmp(WCHAR_TYPE, "short unsigned int") == 0 ||
           std::strcmp(WCHAR_TYPE, "unsigned short") == 0)
    abi_type = short_unsigned_type_node;
  return abi_type != NULL_TREE && TYPE_PRECISION(type) == TYPE_PRECISION(abi_type) &&
         TYPE_UNSIGNED(type) == TYPE_UNSIGNED(abi_type);
}

bool wide_pointer_type(tree type) {
  return type != NULL_TREE && POINTER_TYPE_P(type) &&
         wide_scalar_type(TREE_TYPE(type));
}

const char *library_assembler_name(tree declaration) {
  const char *name = IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(declaration));
  // GCC prefixes explicitly unencoded assembler names with '*'. This is
  // internal encoding, not part of the ELF symbol.
  return name[0] == '*' ? name + 1 : name;
}

bool size_scalar_type(tree type) {
  return type != NULL_TREE && TYPE_MAIN_VARIANT(type) == TYPE_MAIN_VARIANT(size_type_node);
}

WideLibrary wide_library(const gcall *call) {
  tree declaration = gimple_call_fndecl(call);
  if (!consumer_models_active || declaration == NULL_TREE ||
      DECL_WEAK(declaration) || declaration_has_implementation(declaration) ||
      !DECL_ASSEMBLER_NAME_SET_P(declaration))
    return WideLibrary::none;
  tree type = TREE_TYPE(declaration);
  if (type == NULL_TREE || TREE_CODE(type) != FUNCTION_TYPE ||
      gimple_call_fntype(call) != type)
    return WideLibrary::none;
  const char *name = library_assembler_name(declaration);
  tree arguments = TYPE_ARG_TYPES(type);
  const bool checked_output = std::strcmp(name, "__wprintf_chk") == 0;
  const bool checked_copy = std::strcmp(name, "__wcscpy_chk") == 0;
  const bool checked_set = std::strcmp(name, "__wmemset_chk") == 0;
  if (checked_output) {
    if (arguments == NULL_TREE ||
        TYPE_MAIN_VARIANT(TREE_VALUE(arguments)) != TYPE_MAIN_VARIANT(integer_type_node))
      return WideLibrary::none;
    arguments = TREE_CHAIN(arguments);
  }
  if (arguments == NULL_TREE || !wide_pointer_type(TREE_VALUE(arguments)))
    return WideLibrary::none;
  arguments = TREE_CHAIN(arguments);
  if (std::strcmp(name, "wprintf") == 0 || checked_output) {
    return stdio_model_active && arguments == NULL_TREE &&
           TYPE_MAIN_VARIANT(TREE_TYPE(type)) == TYPE_MAIN_VARIANT(integer_type_node) &&
           gimple_call_num_args(call) >= (checked_output ? 2u : 1u)
               ? WideLibrary::output : WideLibrary::none;
  }
  if (!wide_pointer_type(TREE_TYPE(type)) || arguments == NULL_TREE)
    return WideLibrary::none;
  if (std::strcmp(name, "wcscpy") == 0 || checked_copy) {
    if (!wide_pointer_type(TREE_VALUE(arguments)))
      return WideLibrary::none;
    arguments = TREE_CHAIN(arguments);
    if (checked_copy) {
      if (arguments == NULL_TREE || !size_scalar_type(TREE_VALUE(arguments)))
        return WideLibrary::none;
      arguments = TREE_CHAIN(arguments);
    }
    return arguments != NULL_TREE && TREE_VALUE(arguments) == void_type_node &&
           TREE_CHAIN(arguments) == NULL_TREE && gimple_call_num_args(call) == (checked_copy ? 3u : 2u)
               ? WideLibrary::copy : WideLibrary::none;
  }
  if (std::strcmp(name, "wmemset") == 0 || checked_set) {
    if (!wide_scalar_type(TREE_VALUE(arguments)))
      return WideLibrary::none;
    arguments = TREE_CHAIN(arguments);
    if (arguments == NULL_TREE || !size_scalar_type(TREE_VALUE(arguments)))
      return WideLibrary::none;
    arguments = TREE_CHAIN(arguments);
    if (checked_set) {
      if (arguments == NULL_TREE || !size_scalar_type(TREE_VALUE(arguments)))
        return WideLibrary::none;
      arguments = TREE_CHAIN(arguments);
    }
    return arguments != NULL_TREE && TREE_VALUE(arguments) == void_type_node &&
           TREE_CHAIN(arguments) == NULL_TREE && gimple_call_num_args(call) == (checked_set ? 4u : 3u)
               ? WideLibrary::set : WideLibrary::none;
  }
  return WideLibrary::none;
}

bool wide_format_requires_live(const gcall *call, unsigned int format_index,
                               unsigned int requested_argument) {
  if (requested_argument <= format_index)
    return false;
  tree literal = gimple_call_arg(call, format_index);
  if (literal == NULL_TREE || TREE_CODE(literal) != ADDR_EXPR)
    return false;
  literal = TREE_OPERAND(literal, 0);
  if (TREE_CODE(literal) != STRING_CST || TREE_TYPE(literal) == NULL_TREE ||
      TREE_CODE(TREE_TYPE(literal)) != ARRAY_TYPE ||
      !wide_scalar_type(TREE_TYPE(TREE_TYPE(literal))))
    return false;
  const unsigned int bits = TYPE_PRECISION(TREE_TYPE(TREE_TYPE(literal)));
  if (BITS_PER_UNIT != 8 || (bits != 16 && bits != 32))
    return false;
  const unsigned int bytes = bits / BITS_PER_UNIT;
  const unsigned int length = TREE_STRING_LENGTH(literal);
  if (length % bytes != 0 || length / bytes > 256)
    return false;
  const unsigned char *data = reinterpret_cast<const unsigned char *>(TREE_STRING_POINTER(literal));
  std::vector<unsigned int> format;
  for (unsigned int offset = 0; offset < length; offset += bytes) {
    unsigned int value = 0;
    for (unsigned int index = 0; index < bytes; ++index)
      value |= static_cast<unsigned int>(data[offset + index]) <<
               (8 * (BYTES_BIG_ENDIAN ? bytes - index - 1 : index));
    format.push_back(value);
    if (value == 0)
      break;
  }
  if (format.empty() || format.back() != 0)
    return false;
  unsigned int argument = format_index + 1;
  bool required = false;
  for (unsigned int index = 0; index + 1 < format.size(); ++index) {
    if (format[index] != '%')
      continue;
    if (format[++index] == '%')
      continue;
    while (format[index] != 0 &&
           (format[index] == '-' || format[index] == '+' || format[index] == ' ' ||
            format[index] == '#' || format[index] == '0'))
      ++index;
    while (format[index] >= '0' && format[index] <= '9')
      ++index;
    bool zero_precision = false;
    if (format[index] == '.') {
      zero_precision = true;
      ++index;
      while (format[index] >= '0' && format[index] <= '9') {
        zero_precision = zero_precision && format[index] == '0';
        ++index;
      }
    }
    // Positional arguments and dynamic widths/precisions need additional
    // bindings. Reject them rather than assigning the wrong variadic index.
    if (format[index] == '*' || format[index] == '$')
      return false;
    unsigned int length_modifier = 0;
    bool repeated_modifier = false;
    if (format[index] == 'l' || format[index] == 'h' || format[index] == 'L' ||
        format[index] == 'z' || format[index] == 'j' || format[index] == 't') {
      length_modifier = format[index++];
      if ((length_modifier == 'l' || length_modifier == 'h') &&
          format[index] == length_modifier) {
        repeated_modifier = true;
        ++index;
      }
    }
    const unsigned int conversion = format[index];
    if (conversion == 0 || conversion > 127 ||
        std::strchr("diuoxXfFeEgGaAcspn", conversion) == nullptr ||
        ((conversion == 's' || conversion == 'c') &&
         (repeated_modifier || (length_modifier != 0 && length_modifier != 'l'))))
      return false;
    if (argument >= gimple_call_num_args(call))
      return false;
    if (argument == requested_argument &&
        ((conversion == 's' && !zero_precision) || conversion == 'n'))
      required = true;
    ++argument;
  }
  return required;
}

void prove(bool *property, ProofSource *source, ProofSource evidence) {
  *property = true;
  *source = evidence;
}

void clear_proof(bool *property, ProofSource *source) {
  *property = false;
  *source = ProofSource::none;
}

bool normal_builtin_code(const gcall *call, enum built_in_function *code) {
  tree declaration = gimple_call_fndecl(call);
  if (declaration == NULL_TREE || TREE_CODE(declaration) != FUNCTION_DECL ||
      !fndecl_built_in_p(declaration, BUILT_IN_NORMAL))
    return false;
  *code = static_cast<enum built_in_function>(
      DECL_FUNCTION_CODE(declaration));
  return true;
}

void set_builtin_pointer_effect(ArgumentEffect *effect, bool may_read,
                                bool must_read) {
  prove(&effect->no_free_no_capture,
        &effect->no_free_no_capture_source,
        ProofSource::builtin_identity);
  if (may_read)
    prove(&effect->may_read, &effect->may_read_source,
          ProofSource::builtin_identity);
  else
    clear_proof(&effect->may_read, &effect->may_read_source);
  if (must_read)
    prove(&effect->must_read, &effect->must_read_source,
          ProofSource::builtin_identity);
  else
    clear_proof(&effect->must_read, &effect->must_read_source);
}

bool apply_builtin_effect(const gcall *call, unsigned int argument_index,
                          ArgumentEffect *effect) {
  const WideLibrary wide = wide_library(call);
  if (wide == WideLibrary::output || wide == WideLibrary::copy ||
      (wide == WideLibrary::set && argument_index == 0)) {
    prove(&effect->no_free_no_capture, &effect->no_free_no_capture_source,
          wide == WideLibrary::output ? ProofSource::standard_stdio_model : ProofSource::standard_library_model);
    effect->scalar_return_no_pointer = wide == WideLibrary::output;
    const unsigned int format_index = wide == WideLibrary::output &&
        std::strcmp(library_assembler_name(gimple_call_fndecl(call)), "__wprintf_chk") == 0 ? 1u : 0u;
    effect->requires_live = wide == WideLibrary::copy ||
        (wide == WideLibrary::output &&
         (argument_index == format_index ||
          wide_format_requires_live(call, format_index, argument_index)));
    return true;
  }
  enum built_in_function code = BUILT_IN_NONE;
  if (!normal_builtin_code(call, &code))
    return false;
  if (declaration_has_implementation(gimple_call_fndecl(call)))
    return false;

  if ((code == BUILT_IN_STRCPY || code == BUILT_IN_STRCPY_CHK) && argument_index < 2 &&
      !gimple_has_body_p(gimple_call_fndecl(call))) {
    set_builtin_pointer_effect(effect, argument_index == 1, argument_index == 1);
    effect->requires_live = true;
    return true;
  }

  // C string arguments, not a generic nonnull/access attribute. In N1570,
  // 7.1.1/7.1.4 and the stdio function contracts require valid string objects
  // at entry. A custom FILE callback can subsequently retain or free input:
  // no ownership preservation follows from this precondition.
  const bool string_precondition =
      ((code == BUILT_IN_PUTS || code == BUILT_IN_FPUTS ||
        code == BUILT_IN_PRINTF) && argument_index == 0) ||
      ((code == BUILT_IN_FPRINTF || code == BUILT_IN_PRINTF_CHK) &&
       argument_index == 1) ||
      (code == BUILT_IN_FPRINTF_CHK && argument_index == 2);
  if (string_precondition) {
    effect->requires_live = true;
    prove(&effect->may_read, &effect->may_read_source,
          ProofSource::builtin_identity);
    // Like GCC's known stdio output functions, ordinary stdout output keeps
    // the input storage live. This is an explicit library model, independent
    // of the valid-string entry theorem. A visible custom stream/formatter or
    // stdout mutation disables it for the complete consuming unit.
    tree declaration = gimple_call_fndecl(call);
    if (stdio_model_active &&
        (code == BUILT_IN_PUTS || code == BUILT_IN_PRINTF || code == BUILT_IN_PRINTF_CHK) &&
        !gimple_has_body_p(declaration)) {
      prove(&effect->no_free_no_capture, &effect->no_free_no_capture_source,
            ProofSource::standard_stdio_model);
      effect->scalar_return_no_pointer = true;
    }
    return true;
  }

  // Variadic pointer operands are borrowed even when a format does not read
  // them (e.g. %p). This does not add a string/must-read access guarantee.
  tree declaration = gimple_call_fndecl(call);
  if (stdio_model_active && (code == BUILT_IN_PRINTF || code == BUILT_IN_PRINTF_CHK) &&
      !gimple_has_body_p(declaration)) {
    prove(&effect->no_free_no_capture, &effect->no_free_no_capture_source,
          ProofSource::standard_stdio_model);
    effect->scalar_return_no_pointer = true;
    return true;
  }

  if (code == BUILT_IN_OBJECT_SIZE || code == BUILT_IN_STRLEN ||
      code == BUILT_IN_STRNLEN || code == BUILT_IN_MEMCMP ||
      code == BUILT_IN_BCMP)
    effect->scalar_return_no_pointer = true;

  if (code == BUILT_IN_OBJECT_SIZE && argument_index == 0) {
    set_builtin_pointer_effect(effect, false, false);
    return true;
  }

  // Deallocation identities override every generic noescape inference.  In
  // particular, GCC's fnspec for free says that its argument does not escape;
  // that fact does not mean that the allocation survives the call.
  if (argument_index == 0 &&
      (code == BUILT_IN_FREE || code == BUILT_IN_TM_FREE ||
       code == BUILT_IN_REALLOC)) {
    clear_proof(&effect->no_free_no_capture,
                &effect->no_free_no_capture_source);
    clear_proof(&effect->must_read, &effect->must_read_source);
    if (code == BUILT_IN_REALLOC)
      prove(&effect->may_read, &effect->may_read_source,
            ProofSource::builtin_identity);
    else
      clear_proof(&effect->may_read, &effect->may_read_source);
    return true;
  }

#if BUILDING_GCC_MAJOR >= 16
  if (argument_index == 0 &&
      (code == BUILT_IN_GOMP_FREE || code == BUILT_IN_GOMP_REALLOC)) {
    clear_proof(&effect->no_free_no_capture,
                &effect->no_free_no_capture_source);
    clear_proof(&effect->must_read, &effect->must_read_source);
    if (code == BUILT_IN_GOMP_REALLOC)
      prove(&effect->may_read, &effect->may_read_source,
            ProofSource::builtin_identity);
    else
      clear_proof(&effect->may_read, &effect->may_read_source);
    return true;
  }
#endif

  // These byte-copy builtins do not retain or release either pointer.  A zero
  // length is valid, so even the source is only a may-read.
  if (code == BUILT_IN_MEMCPY || code == BUILT_IN_MEMMOVE ||
      code == BUILT_IN_MEMPCPY || code == BUILT_IN_MEMCPY_CHK ||
      code == BUILT_IN_MEMMOVE_CHK || code == BUILT_IN_MEMPCPY_CHK) {
    if (argument_index == 0 || argument_index == 1) {
      set_builtin_pointer_effect(effect, argument_index == 1, false);
      return true;
    }
    return false;
  }

  if (code == BUILT_IN_MEMSET || code == BUILT_IN_MEMSET_CHK) {
    if (argument_index == 0) {
      set_builtin_pointer_effect(effect, false, false);
      return true;
    }
    return false;
  }

  if (code == BUILT_IN_MEMCMP || code == BUILT_IN_BCMP ||
      code == BUILT_IN_MEMCHR) {
    if ((code == BUILT_IN_MEMCHR && argument_index == 0) ||
        (code != BUILT_IN_MEMCHR && argument_index < 2)) {
      set_builtin_pointer_effect(effect, true, false);
      return true;
    }
    return false;
  }

  if (code == BUILT_IN_STRLEN && argument_index == 0) {
    // Every defined strlen execution observes at least the terminating byte.
    set_builtin_pointer_effect(effect, true, true);
    return true;
  }
  if (code == BUILT_IN_STRNLEN && argument_index == 0) {
    // strnlen(s, 0) performs no access.
    set_builtin_pointer_effect(effect, true, false);
    return true;
  }
  return false;
}

}  // namespace

void configure_standard_stdio_model() {
  consumer_models_active = true;
  stdio_model_active = false;
  stdio_model_reason = "incomplete-program-scan";
  cgraph_node *node;
  unsigned int nodes = 0, statements = 0;
  FOR_EACH_FUNCTION(node) {
    if (++nodes > 4096 || !analysis_budget::consume(analysis_budget::WorkKind::summary_node))
      return;
    if (custom_stdio_symbol(node->decl) && (node->callers != nullptr || node->address_taken)) {
      stdio_model_reason = "custom-stream-or-formatter";
      return;
    }
    if (!node->definition || !gimple_has_body_p(node->decl))
      continue;
    function *body = node->get_fun();
    if (body == nullptr || body->cfg == nullptr)
      return;
    basic_block block;
    FOR_EACH_BB_FN(block, body) {
      for (gimple_stmt_iterator iterator = gsi_start_bb(block);
           !gsi_end_p(iterator); gsi_next(&iterator)) {
        if (++statements > 65536 || !analysis_budget::consume(analysis_budget::WorkKind::summary_dependency))
          return;
        const gimple *statement = gsi_stmt(iterator);
        if (gimple_code(statement) == GIMPLE_ASM) {
          stdio_model_reason = "unmodelled-assembly";
          return;
        }
        if (is_gimple_assign(statement)) {
          tree rhs = gimple_assign_rhs1(statement);
          if (stdout_reference(gimple_assign_lhs(statement)) ||
              (rhs != NULL_TREE && TREE_CODE(rhs) == ADDR_EXPR && stdout_reference(TREE_OPERAND(rhs, 0)))) {
            stdio_model_reason = "stdout-mutation-or-address-exposure";
            return;
          }
        }
      }
    }
  }
  stdio_model_active = true;
  stdio_model_reason = "ordinary-stdout; unmodelled-runtime-interposition-excluded";
}

bool standard_stdio_model_active() { return stdio_model_active; }
const char *standard_stdio_model_reason() { return stdio_model_reason; }

bool known_library_returns_argument(const gcall *call, unsigned int argument_index) {
  if (call == nullptr || argument_index != 0 || gimple_call_num_args(call) == 0)
    return false;
  const WideLibrary wide = wide_library(call);
  if (wide == WideLibrary::copy || wide == WideLibrary::set)
    return true;
  enum built_in_function code = BUILT_IN_NONE;
  return normal_builtin_code(call, &code) && !declaration_has_implementation(gimple_call_fndecl(call)) &&
         (code == BUILT_IN_STRCPY || code == BUILT_IN_STRCPY_CHK ||
          code == BUILT_IN_MEMCPY || code == BUILT_IN_MEMCPY_CHK ||
          code == BUILT_IN_MEMMOVE || code == BUILT_IN_MEMMOVE_CHK ||
          code == BUILT_IN_MEMSET || code == BUILT_IN_MEMSET_CHK);
}

bool standard_call_has_normal_return(const gcall *call) {
  if (call == nullptr || gimple_call_noreturn_p(call) ||
      (gimple_call_flags(call) & ECF_RETURNS_TWICE) != 0)
    return false;
  const WideLibrary wide = wide_library(call);
  if (wide != WideLibrary::none)
    return std::strstr(library_assembler_name(gimple_call_fndecl(call)), "_chk") == nullptr;
  enum built_in_function code = BUILT_IN_NONE;
  if (!normal_builtin_code(call, &code) || declaration_has_implementation(gimple_call_fndecl(call)))
    return false;
  switch (code) {
    case BUILT_IN_MALLOC:
    case BUILT_IN_CALLOC:
    case BUILT_IN_MEMCPY:
    case BUILT_IN_MEMMOVE:
    case BUILT_IN_MEMSET:
    case BUILT_IN_STRCPY:
    case BUILT_IN_STRLEN:
      return true;
    case BUILT_IN_PUTS:
    case BUILT_IN_PRINTF:
      return stdio_model_active;
    default:
      return false;
  }
}

const char *proof_source_name(ProofSource source) {
  switch (source) {
    case ProofSource::none:
      return "none";
    case ProofSource::gimple_call_flags:
      return "gimple-call-flags";
    case ProofSource::builtin_identity:
      return "builtin-identity";
    case ProofSource::attr_fnspec:
      return "attr-fnspec";
    case ProofSource::standard_stdio_model:
      return "standard-stdio-model";
    case ProofSource::standard_library_model:
      return "standard-library-model";
  }
  return "unknown";
}

ArgumentEffect argument_effect(const gcall *call,
                               unsigned int argument_index) {
  ArgumentEffect effect;
  if (call == nullptr || argument_index >= gimple_call_num_args(call))
    return effect;

  tree argument = gimple_call_arg(call, argument_index);
  if (argument == NULL_TREE || TREE_TYPE(argument) == NULL_TREE ||
      !POINTER_TYPE_P(TREE_TYPE(argument)))
    return effect;
  effect.valid = true;

  // Visible bodies, aliases and unloaded source definitions take priority.
  // Do not overlay builtin/fnspec library axioms onto their actual effects.
  if (declaration_has_implementation(gimple_call_fndecl(call)))
    return effect;

  const int flags = gimple_call_flags(call);
  if ((flags & (ECF_CONST | ECF_PURE | ECF_NOVOPS)) != 0)
    prove(&effect.no_free_no_capture,
          &effect.no_free_no_capture_source,
          ProofSource::gimple_call_flags);
  if ((flags & ECF_PURE) != 0 &&
      (flags & (ECF_CONST | ECF_NOVOPS)) == 0)
    prove(&effect.may_read, &effect.may_read_source,
          ProofSource::gimple_call_flags);

#if BUILDING_GCC_MAJOR >= 16
  const attr_fnspec spec = gimple_call_fnspec(call);
  if (spec.known_p() && spec.arg_specified_p(argument_index)) {
    // noescape alone is deliberately insufficient: free itself has noescape
    // metadata.  Unused arguments and readonly+noescape arguments are the two
    // fnspec forms that additionally rule out deallocation through this arg.
    if (!spec.arg_used_p(argument_index) ||
        (spec.arg_readonly_p(argument_index) &&
         spec.arg_noescape_p(argument_index)))
      prove(&effect.no_free_no_capture,
            &effect.no_free_no_capture_source,
            ProofSource::attr_fnspec);

    if (spec.arg_maybe_read_p(argument_index))
      prove(&effect.may_read, &effect.may_read_source,
            ProofSource::attr_fnspec);
    else
      clear_proof(&effect.may_read, &effect.may_read_source);
    // attr_fnspec describes allowed accesses, not a mandatory access.
  }
#endif

  // Exact builtin semantics are both portable across GCC 10/16 and strong
  // enough to override a broader call flag or fnspec classification.
  apply_builtin_effect(call, argument_index, &effect);
  return effect;
}

}  // namespace gcc_call_effect
}  // namespace static_analyzer
