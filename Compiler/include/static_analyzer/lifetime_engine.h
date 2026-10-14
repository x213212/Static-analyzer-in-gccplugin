#ifndef STATIC_ANALYZER_LIFETIME_ENGINE_H
#define STATIC_ANALYZER_LIFETIME_ENGINE_H

#include "static_analyzer/options.h"

namespace static_analyzer {

// Intraprocedural ownership analysis over a finite CFG lattice.  It follows
// SSA values with GCC immediate-use iterators, but never enumerates complete
// paths: branch states are unioned and loops converge to a fixed point.
void run_lifetime_engine(const PluginOptions &options);

}  // namespace static_analyzer

#endif
