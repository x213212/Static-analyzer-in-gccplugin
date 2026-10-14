#ifndef STATIC_ANALYZER_LEGACY_ADAPTER_H
#define STATIC_ANALYZER_LEGACY_ADAPTER_H

#include "static_analyzer/options.h"

namespace static_analyzer {
namespace legacy_adapter {

// Enter the historical header-only analyzer.  Keeping this behind one API
// prevents its GCC-heavy includes and process-global configuration from
// leaking into the modern orchestration translation unit.
void run(const PluginOptions &options);

}  // namespace legacy_adapter
}  // namespace static_analyzer

#endif
