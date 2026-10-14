#ifndef STATIC_ANALYZER_GCC_CALL_EFFECT_H
#define STATIC_ANALYZER_GCC_CALL_EFFECT_H

#include "coretypes.h"

namespace static_analyzer {
namespace gcc_call_effect {

// The GCC fact or explicit standard-library model establishing a property.
// `none` means that
// the property is not proven; callers must not interpret that as proof of its
// negation.
enum class ProofSource {
  none,
  gimple_call_flags,
  builtin_identity,
  attr_fnspec,
  standard_stdio_model,
  standard_library_model
};

const char *proof_source_name(ProofSource source);

struct ArgumentEffect {
  // False for a null call, an out-of-range argument number, or a non-pointer
  // argument.  ARGUMENT_INDEX is zero-based, matching gimple_call_arg().
  bool valid = false;

  // A positive result rules out both deallocation of storage reachable from
  // the argument and retention of the argument by the callee.  It does not say
  // that the call result cannot alias the argument.
  bool no_free_no_capture = false;
  ProofSource no_free_no_capture_source = ProofSource::none;

  // Exact builtin returns a size/comparison value, not an encoded pointer.
  // Generic ECF_CONST/PURE do not establish this fact.
  bool scalar_return_no_pointer = false;

  // Entry precondition of an exact library builtin: the argument must denote
  // a live string. This neither promises physical reads on an I/O failure nor
  // rules out capture/release by callbacks during the call.
  bool requires_live = false;

  // `may_read` says that authoritative metadata permits a read through the
  // argument; it is not proof that every execution reads.  `must_read` is the
  // stronger property and is deliberately rare.  GCC fnspec/access metadata
  // alone never establishes a mandatory access.
  bool may_read = false;
  ProofSource may_read_source = ProofSource::none;
  bool must_read = false;
  ProofSource must_read_source = ProofSource::none;
};

// Return effects supported by GCC flags, builtin identity, attr_fnspec, or a
// typed standard-library model. Wide models match exact assembler symbols and
// complete ABI prototypes; available bodies and weak declarations take priority.
ArgumentEffect argument_effect(const gcall *call,
                               unsigned int argument_index);

// Default known-function model for ordinary stdout output. Build it once at
// the consuming IPA boundary. Visible custom stream/formatter installation,
// stdout mutation/address exposure or an incomplete scan disables it.
// Producer summaries never initialize or export this model.
void configure_standard_stdio_model();
bool standard_stdio_model_active();
const char *standard_stdio_model_reason();
bool known_library_returns_argument(const gcall *call, unsigned int argument_index);
// Normal return under the modeled API's valid-input and runtime profile.
// Checked variants are excluded: their failure arm can terminate the process.
bool standard_call_has_normal_return(const gcall *call);

}  // namespace gcc_call_effect
}  // namespace static_analyzer

#endif
