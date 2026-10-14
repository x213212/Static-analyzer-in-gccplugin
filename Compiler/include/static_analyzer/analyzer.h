#ifndef STATIC_ANALYZER_ANALYZER_H
#define STATIC_ANALYZER_ANALYZER_H

#include "static_analyzer/options.h"

namespace static_analyzer {

// Runs once as a SIMPLE_IPA_PASS after GCC's pta pass.
unsigned int run(const PluginOptions &options);

}  // namespace static_analyzer

#endif
