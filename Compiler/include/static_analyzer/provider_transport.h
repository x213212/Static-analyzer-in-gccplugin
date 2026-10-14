#ifndef STATIC_ANALYZER_PROVIDER_TRANSPORT_H
#define STATIC_ANALYZER_PROVIDER_TRANSPORT_H

#include "coretypes.h"

namespace static_analyzer {
namespace provider_transport {

using GenerateFacts = void (*)();

// Register an early IPA transport pass and its GCC lifecycle/GC callbacks.
// GENERATE runs once during initial LTO summary generation, not an ordinary
// non-LTO compile or a WPA/LTRANS import. It must use record() for independently
// proved, bounded payloads; late-IPA points-to facts are not yet available.
void register_passes(const char *plugin_name, GenerateFacts generate);

// Store an opaque payload on the exact current FUNCTION_DECL. At most 4096
// records are retained. A later record for the same declaration replaces its
// previous payload; NULL_TREE invalidates it. Stale nodes, aliases, thunks and
// clones are rejected rather than transferring facts by assembler name.
//
// The producer MUST validate payload shape/depth/size before calling record().
// The payload and declaration are explicitly marked during GCC collection.
// They are serialized by GCC's declaration streamer, not as process pointers.
void record(tree declaration, tree payload);

// Return the opaque payload for the same surviving cgraph declaration, or
// NULL_TREE. This does NOT prove call binding or validate semantics: consumers
// must check the payload schema/version, complete lowered signature, producer
// identity and actual call-edge binding before using any imported fact.
tree lookup(tree declaration);

// Formal diagnostics use the current GCC dump stream only, never stderr.
void dump_statistics();

}  // namespace provider_transport
}  // namespace static_analyzer

#endif
