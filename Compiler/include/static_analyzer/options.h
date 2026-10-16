#ifndef STATIC_ANALYZER_OPTIONS_H
#define STATIC_ANALYZER_OPTIONS_H

#include <string>

struct plugin_name_args;

namespace static_analyzer {

enum class AnalysisPolicy {
  // Emit a diagnostic only after the bounded model proves a feasible witness.
  // Unknown analysis outcomes are kept out of finding counts and are exposed
  // separately as report-only inconclusive records.
  proof_only,

  // Research mode: retain candidates whose feasibility is
  // unknown.  This is intentionally opt-in because it trades precision for
  // recall.
  exploratory
};

enum class CalleeModel { unknown, borrow, release };

struct PluginOptions {
  bool memory_leak = true;
  bool double_free = true;
  bool use_after_free = false;
  bool thread = true;
  bool smt = true;
  bool report = true;
  bool report_html = true;
  bool report_only = false;
  bool ffmpeg_library_model = false;
  bool report_call_graph = false;
  AnalysisPolicy analysis_policy = AnalysisPolicy::proof_only;
  CalleeModel callee_model = CalleeModel::unknown;
  std::string callee_symbol;
  unsigned int callee_argument = 0;
  std::string report_path;
};

const char *analysis_policy_name(AnalysisPolicy policy);
const char *callee_model_name(CalleeModel model);

// Parse GCC's -fplugin-arg-<plugin>-<key>=<value> arguments once, during
// plugin_init.  GCC owns the raw argv storage, so no pointer to it escapes.
bool parse_plugin_options(const plugin_name_args &plugin,
                          PluginOptions *options,
                          std::string *error_message);

}  // namespace static_analyzer

#endif
