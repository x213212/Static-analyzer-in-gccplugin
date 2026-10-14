#ifndef STATIC_ANALYZER_TRUSTED_CONTRACT_H
#define STATIC_ANALYZER_TRUSTED_CONTRACT_H

#include "coretypes.h"

namespace static_analyzer {
namespace trusted_contract {

// These effects are explicit source-level promises, not facts inferred from a
// function name, a standard GCC access attribute, or the absence of an IPA
// mod/ref flag.  In particular, GCC documents that access(read_only, ...)
// describes how an access may occur but does not promise that one will occur.
enum class AccessGuarantee {
  none,
  must_access,
  access_if_nonnull
};

struct PointerArgumentEffect {
  // True when at least one accepted contract attribute applies to this exact
  // declaration and parameter.  A false result is unknown, never a proof that
  // the function has no effect.
  bool resolved = false;

  // The function promises not to release the referenced ownership and not to
  // retain or publish the pointer beyond the dynamic extent of the call,
  // except via its return value.  Return aliasing requires independent
  // evidence; this promise alone says nothing about the returned value.
  bool no_free_no_capture = false;

  // If both access attributes are present, the unconditional promise is the
  // stronger contract and is returned here.
  AccessGuarantee access = AccessGuarantee::none;
};

// PLUGIN_ATTRIBUTES callback.  Integration should register this function with
// register_callback(plugin_name, PLUGIN_ATTRIBUTES,
//                   register_attributes, nullptr).
void register_attributes(void *event_data, void *user_data);

// Query only DECL_ATTRIBUTES on FUNCTION_DECL itself.  This deliberately does
// not follow aliases, inspect TYPE_ATTRIBUTES, recognize symbol spellings, or
// manufacture effects from GCC's ordinary access/nonnull attributes.
//
// PARAMETER_INDEX is 1-based in the SOURCE declaration, excluding artificial
// `this`.  Unlike GCC's ordinary positional attribute convention, method
// attribute (1) means its first explicitly written parameter.  Constructors,
// destructors and other unsupported artificial parameter layouts are rejected.
// This function must not receive a lowered GIMPLE operand index.
PointerArgumentEffect pointer_argument_effect(
    tree function_declaration, unsigned int parameter_index);

// ARGUMENT_INDEX is the zero-based GIMPLE actual operand.  Maps an ordinary
// direct function/method call back to the source contract; `this`, variadic
// arguments, rewritten IPA declarations, and unsupported layouts are unknown.
PointerArgumentEffect pointer_argument_effect_for_call(
    const gimple *statement, unsigned int argument_index);

}  // namespace trusted_contract
}  // namespace static_analyzer

#endif
