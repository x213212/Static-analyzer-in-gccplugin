#include "gcc-plugin.h"
#include "plugin-version.h"
#include "context.h"
#include "tree-pass.h"
#include "diagnostic-core.h"
#include "options.h"

#include "static_analyzer/analyzer.h"
#include "static_analyzer/options.h"
#include "static_analyzer/ownership_summary.h"
#include "static_analyzer/provider_transport.h"
#include "static_analyzer/report.h"
#include "static_analyzer/trusted_contract.h"
#include "static_analyzer/version.h"

#include <string>

__attribute__((visibility("default"))) int plugin_is_GPL_compatible;

namespace {

const pass_data analyzer_pass_data = {
    SIMPLE_IPA_PASS,
    "static_analyzer",
    OPTGROUP_NONE,
    TV_PLUGIN_RUN,
    PROP_ssa,
    0,
    0,
    0,
    0,
};

bool analyzer_executed_for_unit = false;

class AnalyzerPass : public simple_ipa_opt_pass {
 public:
  AnalyzerPass(gcc::context *context,
               const static_analyzer::PluginOptions &options)
      : simple_ipa_opt_pass(analyzer_pass_data, context), options_(options) {}

  unsigned int execute(function *) override {
    // LTO producers publish bounded body facts; diagnostics belong to the
    // link-time consumer with its resolved graph and late points-to facts.
    if (flag_lto && !in_lto_p)
      return 0;
    // This is a whole-translation-unit analysis.  The pass is registered at
    // the unique late-IPA boundary below; retain a guard as a defense against
    // vendor pass-manager changes duplicating that boundary.
    if (analyzer_executed_for_unit)
      return 0;
    analyzer_executed_for_unit = true;
    return static_analyzer::run(options_);
  }

  AnalyzerPass *clone() override { return new AnalyzerPass(m_ctxt, options_); }

 private:
  static_analyzer::PluginOptions options_;
};

register_pass_info registered_pass;

void start_unit(void *, void *) {
  analyzer_executed_for_unit = false;
  static_analyzer::report::reset();
}

void finish_unit(void *, void *) {
  // A producer/WPA-only process has not analyzed the program. Do not leave
  // an empty report that can be mistaken for the consumer's result.
  if (analyzer_executed_for_unit)
    static_analyzer::report::write_json();
}

const plugin_info analyzer_plugin_info = {
    STATIC_ANALYZER_VERSION,
    "GCC points-to based memory lifetime analyzer\n"
    "Arguments are boolean values (0 or 1): debugmod, debugoutput, "
    "vscode_extensionmod, contextsentive, relatemod, "
    "retnotheapobjskipcheck, traceallfucntion, prebranchexit, "
    "memoryleakcheck, doublefreecheck, useafterfree, threadmod, smt, "
    "legacyengine, report, reporthtml, reportonly, reportcallgraph.\n"
    "analysispolicy accepts proof-only (default) or exploratory.\n"
    "calleemodel=borrow|release with calleesymbol and calleearg (zero-based) "
    "runs a labelled external-call assumption experiment.\n"
    "reportpath accepts an output stem; reporthtml=0 writes JSON only.\n"
    "reportonly=1 retains findings in reports without compiler diagnostics.\n"
    "librarymodel=ffmpeg enables labelled SDK resource reviews (default none).\n"
    "Use -fdump-ipa-all-details for alias and value-flow evidence.\n",
};

}  // namespace

__attribute__((visibility("default")))
int plugin_init(plugin_name_args *plugin, plugin_gcc_version *version) {
  if (!plugin_default_version_check(version, &gcc_version)) {
    error("static analyzer plugin was built for GCC %s but loaded by GCC %s",
          gcc_version.basever, version->basever);
    return 1;
  }

  static_analyzer::PluginOptions options;
  std::string option_error;
  if (!static_analyzer::parse_plugin_options(*plugin, &options,
                                              &option_error)) {
    error("static analyzer: %s", option_error.c_str());
    return 1;
  }
  static_analyzer::report::configure(options.report, options.report_path,
                                     options.analysis_policy,
                                     options.report_html, !options.report_only);
  static_analyzer::report::configure_call_graph(options.report_call_graph);
  if (options.callee_model != static_analyzer::CalleeModel::unknown &&
      options.callee_symbol.empty()) {
    error("calleemodel requires calleesymbol");
    return 1;
  }
  static_analyzer::report::configure_callee_model(options.callee_model,
                                                options.callee_symbol,
                                                options.callee_argument);

  register_callback(plugin->base_name, PLUGIN_INFO, nullptr,
                    const_cast<plugin_info *>(&analyzer_plugin_info));
  register_callback(plugin->base_name, PLUGIN_ATTRIBUTES,
                    static_analyzer::trusted_contract::register_attributes,
                    nullptr);

  if (options.memory_leak || options.double_free || options.use_after_free)
    static_analyzer::provider_transport::register_passes(
        plugin->base_name,
        static_analyzer::ownership_summary::generate_provider_facts);

  registered_pass.pass = new AnalyzerPass(g, options);
  // GCC 10 and GCC 16 both place the unique simple-IPA "simdclone" pass
  // immediately after the late IPA PTA pass.  Anchor before that pass instead
  // of registering after every pass named "pta": the other PTA lives inside
  // the OpenACC pass group and can otherwise run first on accelerator input.
  // Instance 1 has a defined plugin-API meaning (TODO_mark_first_instance);
  // arbitrary dump suffixes such as pta2 are not portable instance numbers.
  registered_pass.reference_pass_name = "simdclone";
  registered_pass.ref_pass_instance_number = 1;
  registered_pass.pos_op = PASS_POS_INSERT_BEFORE;
  register_callback(plugin->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr,
                    &registered_pass);
  register_callback(plugin->base_name, PLUGIN_START_UNIT, start_unit, nullptr);
  register_callback(plugin->base_name, PLUGIN_FINISH_UNIT, finish_unit,
                    nullptr);
  return 0;
}
