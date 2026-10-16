#ifndef STATIC_ANALYZER_ANALYSIS_BUDGET_H
#define STATIC_ANALYZER_ANALYSIS_BUDGET_H

#include "coretypes.h"

namespace static_analyzer {
namespace analysis_budget {

enum class WorkKind {
  summary_node,
  summary_dependency,
  summary_scc,
  summary_order,
  summary_parameter_cfg,
  lifetime_discovery,
  lifetime_allocation_statement,
  lifetime_cfg,
  count
};

enum class ExhaustionReason {
  none,
  tu_cpu_time,
  function_cpu_time,
  tu_work,
  function_work,
  summary_node_limit
};

struct Limits {
  unsigned int tu_cpu_ms = 2000;
  unsigned int function_cpu_ms = 100;
  unsigned long tu_work = 2000000;
  unsigned long function_work = 250000;
  unsigned int summary_nodes = 4096;
};

// Start one translation-unit budget.  The defaults are intentionally much
// larger than ordinary unit-test functions and exist only to prevent an
// adversarial or generated TU from monopolizing the compiler process.
void reset(const Limits &limits = Limits{});

// A function budget may be opened repeatedly during summary construction and
// lifetime analysis.  It is independent each time, while all work also
// contributes to the single TU budget.
bool begin_function(tree declaration);
void end_function();

// Charge deterministic work before doing it.  False is sticky for the
// current function (or the whole TU for a TU limit).  Process-CPU checks are
// sampled at bounded work intervals to keep the hot-loop overhead small and
// do not charge time while this compiler process is descheduled.
bool consume(WorkKind kind, unsigned long amount = 1);
bool available();
bool tu_available();
bool function_available();

// Stop summary graph recursion before it can become a C++ stack hazard.
bool accept_summary_node_count(unsigned long nodes);

ExhaustionReason reason();
const char *reason_name(ExhaustionReason reason);
void dump_statistics();

}  // namespace analysis_budget
}  // namespace static_analyzer

#endif
