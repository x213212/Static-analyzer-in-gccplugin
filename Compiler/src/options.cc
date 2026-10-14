#include "gcc-plugin.h"
#include "plugin.h"

#include "static_analyzer/options.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <set>

namespace static_analyzer {
namespace {

bool parse_bool(const plugin_argument &argument, bool *value,
                std::string *error_message) {
  if (argument.value == nullptr) {
    *error_message = std::string("missing value for plugin argument '") +
                     argument.key + "' (expected 0 or 1)";
    return false;
  }

  char *end = nullptr;
  errno = 0;
  const long parsed = std::strtol(argument.value, &end, 10);
  if (errno != 0 || end == argument.value || *end != '\0' ||
      (parsed != 0 && parsed != 1)) {
    *error_message = std::string("invalid value '") + argument.value +
                     "' for plugin argument '" + argument.key +
                     "' (expected 0 or 1)";
    return false;
  }

  *value = parsed != 0;
  return true;
}

}  // namespace

bool parse_plugin_options(const plugin_name_args &plugin,
                          PluginOptions *options,
                          std::string *error_message) {
  std::set<std::string> seen_arguments;
  for (int index = 0; index < plugin.argc; ++index) {
    const plugin_argument &argument = plugin.argv[index];
    bool *destination = nullptr;

    if (argument.key == nullptr || argument.key[0] == '\0') {
      *error_message = "empty plugin argument name";
      return false;
    }
    if (!seen_arguments.insert(argument.key).second) {
      *error_message = std::string("duplicate plugin argument '") +
                       argument.key + "'";
      return false;
    }

    if (std::strcmp(argument.key, "reportpath") == 0) {
      if (argument.value == nullptr || argument.value[0] == '\0') {
        *error_message = "missing value for plugin argument 'reportpath'";
        return false;
      }
      options->report_path = argument.value;
      continue;
    }

    if (std::strcmp(argument.key, "librarymodel") == 0) {
      if (argument.value != nullptr && std::strcmp(argument.value, "ffmpeg") == 0)
        options->ffmpeg_library_model = true;
      else if (argument.value != nullptr && std::strcmp(argument.value, "none") == 0)
        options->ffmpeg_library_model = false;
      else {
        *error_message = "librarymodel expects none or ffmpeg";
        return false;
      }
      continue;
    }
    if (std::strcmp(argument.key, "analysispolicy") == 0) {
      if (argument.value == nullptr || argument.value[0] == '\0') {
        *error_message =
            "missing value for plugin argument 'analysispolicy' "
            "(expected proof-only or exploratory)";
        return false;
      }
      if (std::strcmp(argument.value, "proof-only") == 0)
        options->analysis_policy = AnalysisPolicy::proof_only;
      else if (std::strcmp(argument.value, "exploratory") == 0)
        options->analysis_policy = AnalysisPolicy::exploratory;
      else {
        *error_message = std::string("invalid value '") + argument.value +
                         "' for plugin argument 'analysispolicy' "
                         "(expected proof-only or exploratory)";
        return false;
      }
      continue;
    }

    if (std::strcmp(argument.key, "calleemodel") == 0) {
      if (argument.value != nullptr && std::strcmp(argument.value, "unknown") == 0)
        options->callee_model = CalleeModel::unknown;
      else if (argument.value != nullptr && std::strcmp(argument.value, "borrow") == 0)
        options->callee_model = CalleeModel::borrow;
      else if (argument.value != nullptr && std::strcmp(argument.value, "release") == 0)
        options->callee_model = CalleeModel::release;
      else {
        *error_message = "calleemodel expects unknown, borrow, or release";
        return false;
      }
      continue;
    }
    if (std::strcmp(argument.key, "calleesymbol") == 0) {
      if (argument.value == nullptr || argument.value[0] == '\0') {
        *error_message = "calleesymbol requires an exact assembler symbol";
        return false;
      }
      options->callee_symbol = argument.value;
      continue;
    }
    if (std::strcmp(argument.key, "calleearg") == 0) {
      char *end = nullptr;
      errno = 0;
      const long parsed = argument.value == nullptr ? -1 : std::strtol(argument.value, &end, 10);
      if (errno != 0 || end == nullptr || end == argument.value || *end != '\0' || parsed < 0 || parsed > 63) {
        *error_message = "calleearg expects a zero-based argument index from 0 to 63";
        return false;
      }
      options->callee_argument = static_cast<unsigned int>(parsed);
      continue;
    }

    if (std::strcmp(argument.key, "relatemod") == 0)
      destination = &options->relate;
    else if (std::strcmp(argument.key, "retnotheapobjskipcheck") == 0)
      destination = &options->skip_non_heap_returns;
    else if (std::strcmp(argument.key, "traceallfucntion") == 0)
      destination = &options->trace_all_functions;
    else if (std::strcmp(argument.key, "prebranchexit") == 0)
      destination = &options->stop_at_branch_exit;
    else if (std::strcmp(argument.key, "debugoutput") == 0)
      destination = &options->debug_output;
    else if (std::strcmp(argument.key, "debugmod") == 0)
      destination = &options->debug;
    else if (std::strcmp(argument.key, "vscode_extensionmod") == 0)
      destination = &options->vscode_extension;
    else if (std::strcmp(argument.key, "contextsentive") == 0)
      destination = &options->context_sensitive;
    else if (std::strcmp(argument.key, "memoryleakcheck") == 0)
      destination = &options->memory_leak;
    else if (std::strcmp(argument.key, "doublefreecheck") == 0)
      destination = &options->double_free;
    else if (std::strcmp(argument.key, "useafterfree") == 0)
      destination = &options->use_after_free;
    else if (std::strcmp(argument.key, "threadmod") == 0)
      destination = &options->thread;
    else if (std::strcmp(argument.key, "smt") == 0)
      destination = &options->smt;
    else if (std::strcmp(argument.key, "legacyengine") == 0)
      destination = &options->legacy_engine;
    else if (std::strcmp(argument.key, "report") == 0)
      destination = &options->report;
    else if (std::strcmp(argument.key, "reporthtml") == 0)
      destination = &options->report_html;
    else if (std::strcmp(argument.key, "reportonly") == 0)
      destination = &options->report_only;
    else if (std::strcmp(argument.key, "reportcallgraph") == 0)
      destination = &options->report_call_graph;
    else {
      *error_message = std::string("unknown plugin argument '") +
                       argument.key + "'";
      return false;
    }

    if (!parse_bool(argument, destination, error_message))
      return false;
  }

  return true;
}

const char *callee_model_name(CalleeModel model) {
  switch (model) {
    case CalleeModel::unknown: return "unknown";
    case CalleeModel::borrow: return "borrow";
    case CalleeModel::release: return "release";
  }
  return "unknown";
}

const char *analysis_policy_name(AnalysisPolicy policy) {
  switch (policy) {
    case AnalysisPolicy::proof_only:
      return "proof-only";
    case AnalysisPolicy::exploratory:
      return "exploratory";
  }
  return "proof-only";
}

}  // namespace static_analyzer
