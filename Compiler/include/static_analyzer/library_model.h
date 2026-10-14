#ifndef STATIC_ANALYZER_LIBRARY_MODEL_H
#define STATIC_ANALYZER_LIBRARY_MODEL_H

#include "coretypes.h"

namespace static_analyzer {
namespace library_model {

// Explicit SDK profile. These are external API contracts, not body proofs.
// No profile is enabled by default; a loaded implementation takes priority.
enum class Effect { none, output_handles, dictionary_insert, release_slot,
                    heap_allocate, heap_release };
struct CallEffect {
  Effect kind = Effect::none;
  unsigned int slot_argument = 0;
  bool dictionary = false;
};
void configure(bool ffmpeg);
bool ffmpeg_enabled();
const char *profile_name();
CallEffect effect(const gimple *call);

}  // namespace library_model
}  // namespace static_analyzer
#endif
