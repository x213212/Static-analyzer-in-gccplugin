#ifndef STATIC_ANALYZER_RESOURCE_PROTOCOL_H
#define STATIC_ANALYZER_RESOURCE_PROTOCOL_H
#include "coretypes.h"
namespace static_analyzer {
namespace resource_protocol {
// Analyze local pointer cells under the explicit library profile. Ordinary
// body-proven heap diagnostics remain the lifetime engine's responsibility.
void analyze_current_function();
// Inspect registered framework init/destructor pairs, with loaded bodies.
void analyze_registered_lifecycles();
}  // namespace resource_protocol
}  // namespace static_analyzer
#endif
