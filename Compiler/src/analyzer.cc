#include "gcc-plugin.h"

#include "static_analyzer/alias_oracle.h"
#include "static_analyzer/analysis_budget.h"
#include "static_analyzer/analyzer.h"
#include "static_analyzer/feasibility.h"
#include "static_analyzer/lifetime_engine.h"
#include "static_analyzer/local_confinement.h"
#include "static_analyzer/ownership_call.h"
#include "static_analyzer/ownership_summary.h"
#include "static_analyzer/provider_transport.h"
#include "static_analyzer/report.h"
#include "static_analyzer/gcc_call_effect.h"
#include "static_analyzer/library_model.h"
#include "static_analyzer/resource_protocol.h"

namespace static_analyzer {

unsigned int run(const PluginOptions &options) {
  analysis_budget::reset();
  library_model::configure(options.ffmpeg_library_model);
  ownership_summary::configure_callee_model(options.callee_model,
                                            options.callee_symbol,
                                            options.callee_argument);
  alias::reset_statistics();
  feasibility::reset_statistics();
  local_confinement::reset();
  feasibility::Limits feasibility_limits = feasibility::default_limits();
  feasibility_limits.use_solver = options.smt;
  feasibility::set_default_limits(feasibility_limits);
  set_ownership_proof_only(
      options.analysis_policy == AnalysisPolicy::proof_only);
  const bool memory_rules_enabled =
      options.memory_leak || options.double_free || options.use_after_free;
  // Ownership summaries are irrelevant to a thread-only run and are one of
  // the dominant fixed costs on function-heavy translation units.  Preserve
  // an explicitly empty summary state for dumps/queries when every memory
  // rule is disabled, and skip the whole modern engine when no modern rule
  // is enabled at all.
  if (memory_rules_enabled) {
    gcc_call_effect::configure_standard_stdio_model();
    ownership_summary::build();
  } else
    ownership_summary::reset();
  if (memory_rules_enabled || options.thread)
    run_lifetime_engine(options);
  if (options.memory_leak && options.ffmpeg_library_model)
    resource_protocol::analyze_registered_lifecycles();
  // Report-only bounded structural graph. It does not change the lifetime
  // engine, its solver constraints, budgets, or any finding classification.
  if (options.report && options.report_call_graph)
    report::collect_call_graph();
  alias::dump_statistics();
  feasibility::dump_statistics();
  ownership_summary::dump_statistics();
  provider_transport::dump_statistics();
  analysis_budget::dump_statistics();
  report::dump_summary();
  return 0;
}

}  // namespace static_analyzer
