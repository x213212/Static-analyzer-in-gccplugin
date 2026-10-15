#include "gcc-plugin.h"
#include "diagnostic-core.h"
#include "dumpfile.h"
#include "input.h"
#include "options.h"
#include "tree.h"
#include "gimple.h"
#include "stringpool.h"
#include "function.h"
#include "cgraph.h"
#include "gimple-iterator.h"
#include "gimple-pretty-print.h"
#include "tree-cfg.h"

#include "static_analyzer/feasibility.h"
#include "static_analyzer/options.h"
#include "static_analyzer/report.h"
#include "static_analyzer/version.h"
#include "static_analyzer/gcc_call_effect.h"
#include "static_analyzer/ownership_call.h"
#include "static_analyzer/library_model.h"

#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <unistd.h>

namespace static_analyzer {
namespace report {
namespace {

std::set<std::pair<int, location_t>> emitted_findings;
unsigned long finding_counts[4] = {0, 0, 0, 0};
std::set<std::tuple<int, location_t, std::string>> inconclusive_candidates;
unsigned long inconclusive_counts[4] = {0, 0, 0, 0};
std::map<std::string, unsigned long> inconclusive_reason_counts;
unsigned long inconclusive_samples_omitted = 0;
unsigned long inconclusive_events_after_dedupe_cap = 0;
constexpr std::size_t max_inconclusive_samples = 64;
constexpr std::size_t max_inconclusive_dedupe_keys = 4096;
constexpr std::size_t max_inconclusive_related_locations = 8;
bool report_enabled = true;
bool report_html_enabled = true;
bool compiler_diagnostics_enabled = true;
AnalysisPolicy configured_policy = AnalysisPolicy::proof_only;
std::string configured_path;
CalleeModel configured_callee_model = CalleeModel::unknown;
std::string configured_callee_symbol;
unsigned int configured_callee_argument = 0;
bool call_graph_enabled = false;
bool call_graph_truncated = false;
struct GraphStatement {
  std::string kind, text, callee, file;
  int line, column, block;
};
struct GraphFunction {
  std::string symbol, name, file;
  int line;
  std::vector<GraphStatement> statements;
  struct Arc { int from, to; std::string branch; };
  std::vector<Arc> arcs;
};
std::vector<GraphFunction> call_graph_functions;

constexpr std::size_t max_callee_effects = 128;
struct CalleeEffect {
  std::string caller, symbol, fates, file;
  unsigned int argument_index, argument_count;
  int line, column;
  bool body_available, needs_review;
};
std::map<std::pair<const gimple *, unsigned int>, CalleeEffect> callee_effects;
unsigned long callee_effects_omitted_events = 0;

struct Finding {
  FindingKind kind;
  std::string file;
  int line;
  int column;
  struct ExpandedTracePoint {
    std::string role;
    std::string file;
    int line;
    int column;
  };
  std::vector<ExpandedTracePoint> trace;
  std::string model_scope;
  std::string model_path_verdict;
};

std::vector<Finding> findings;

struct InconclusiveSample {
  FindingKind kind;
  std::string reason;
  std::string detail;
  std::string file;
  int line;
  int column;
  std::vector<Finding::ExpandedTracePoint> related;
};

std::vector<InconclusiveSample> inconclusive_samples;

int index(FindingKind kind) { return static_cast<int>(kind); }

std::string json_escape(const std::string &input) {
  std::string result;
  for (const unsigned char character : input) {
    switch (character) {
      case '\\': result += "\\\\"; break;
      case '"': result += "\\\""; break;
      case '\b': result += "\\b"; break;
      case '\f': result += "\\f"; break;
      case '\n': result += "\\n"; break;
      case '\r': result += "\\r"; break;
      case '\t': result += "\\t"; break;
      default:
        if (character < 0x20) {
          char encoded[7];
          std::snprintf(encoded, sizeof(encoded), "\\u%04x", character);
          result += encoded;
        } else {
          result += static_cast<char>(character);
        }
    }
  }
  return result;
}

std::string html_escape(const std::string &input) {
  std::string result;
  for (const char character : input) {
    switch (character) {
      case '&': result += "&amp;"; break;
      case '<': result += "&lt;"; break;
      case '>': result += "&gt;"; break;
      case '"': result += "&quot;"; break;
      case '\'': result += "&#39;"; break;
      default: result += character;
    }
  }
  return result;
}

std::string default_report_stem() {
  // GCC has already resolved -o, -dumpdir and -dumpbase into dump_base_name.
  // Reusing that official output stem prevents two inputs named e.g. util.c
  // in different directories from overwriting one basename-only report when
  // a build system gives them distinct object paths.
  if (dump_base_name != nullptr && dump_base_name[0] != '\0')
    return std::string(dump_base_name) + ".static-analyzer";
  const char *input = main_input_filename;
  if (input == nullptr || input[0] == '\0')
    return "static-analyzer-report";
  const char *slash = std::strrchr(input, '/');
  const char *backslash = std::strrchr(input, '\\');
  const char *base = input;
  if (slash != nullptr)
    base = slash + 1;
  if (backslash != nullptr && backslash + 1 > base)
    base = backslash + 1;
  return std::string(base) + ".static-analyzer";
}

bool ends_with(const std::string &value, const char *suffix) {
  const std::size_t suffix_size = std::strlen(suffix);
  return value.size() >= suffix_size &&
         value.compare(value.size() - suffix_size, suffix_size, suffix) == 0;
}

std::string report_stem() {
  std::string stem = configured_path.empty() ? default_report_stem()
                                             : configured_path;
  if (ends_with(stem, ".json"))
    stem.resize(stem.size() - 5);
  else if (ends_with(stem, ".html"))
    stem.resize(stem.size() - 5);
  // Partitioned LTO launches several lto1 consumers with identical plugin
  // arguments. Atomic rename alone cannot prevent one partition's report
  // from replacing another's. Preserve each process's findings separately.
  if (flag_ltrans)
    stem += ".ltrans-" +
            std::to_string(static_cast<unsigned long>(getpid()));
  return stem;
}

}  // namespace

static bool emit_with_scope(FindingKind kind, location_t location,
          const std::vector<TracePoint> &trace,
          const std::string &scope, const std::string &path_verdict) {
  const std::pair<int, location_t> key(index(kind), location);
  if (emitted_findings.find(key) != emitted_findings.end())
    return false;

  // Warning mode follows GCC's diagnostic filtering. Explicit report-only
  // mode retains the same proved candidates without invoking warning_at:
  // build systems using -Werror can consume findings from JSON/HTML.
  const std::string diagnostic =
      std::string("[") + code(kind) + "] " +
      (scope.empty() ? "" : "library model review: ") + message(kind);
  if (compiler_diagnostics_enabled &&
      !warning_at(location, 0, "%s", diagnostic.c_str()))
    return false;
  if (!compiler_diagnostics_enabled && !report_enabled)
    return false;

  emitted_findings.insert(key);
  ++finding_counts[index(kind)];
  const expanded_location expanded = expand_location(location);
  Finding finding{kind, expanded.file == nullptr ? "" : expanded.file,
                  expanded.line, expanded.column, {}, scope, path_verdict};
  for (const TracePoint &point : trace) {
    const expanded_location related = expand_location(point.location);
    finding.trace.push_back(
        {point.role, related.file == nullptr ? "" : related.file,
         related.line, related.column});
  }
  findings.push_back(finding);
  if (compiler_diagnostics_enabled)
    for (const TracePoint &point : trace)
      inform(point.location, "static analyzer trace: %s", point.role.c_str());
  return true;
}

bool emit(FindingKind kind, location_t location,
          const std::vector<TracePoint> &trace) {
  return emit_with_scope(kind, location, trace, {}, {});
}

bool emit_model_review(FindingKind kind, location_t location,
                       const std::string &scope, const std::string &path_verdict,
                       const std::vector<TracePoint> &trace) {
  return emit_with_scope(kind, location, trace, scope, path_verdict);
}

void record_inconclusive(FindingKind kind, location_t location,
                         const std::string &reason,
                         const std::string &detail,
                         const std::vector<TracePoint> &related) {
  const std::string stable_reason =
      reason.empty() ? "analysis-unknown" : reason;
  const std::tuple<int, location_t, std::string> key(
      index(kind), location, stable_reason);
  if (inconclusive_candidates.find(key) != inconclusive_candidates.end())
    return;
  if (inconclusive_candidates.size() >= max_inconclusive_dedupe_keys) {
    ++inconclusive_events_after_dedupe_cap;
    return;
  }
  inconclusive_candidates.insert(key);

  ++inconclusive_counts[index(kind)];
  ++inconclusive_reason_counts[stable_reason];
  if (inconclusive_samples.size() >= max_inconclusive_samples) {
    ++inconclusive_samples_omitted;
    return;
  }
  const expanded_location expanded = expand_location(location);
  inconclusive_samples.push_back(
      {kind, stable_reason, detail,
       expanded.file == nullptr ? "" : expanded.file,
       expanded.line, expanded.column, {}});
  for (const TracePoint &point : related) {
    if (inconclusive_samples.back().related.size() >=
        max_inconclusive_related_locations)
      break;
    const expanded_location related_location = expand_location(point.location);
    if (related_location.file == nullptr || related_location.line <= 0)
      continue;
    inconclusive_samples.back().related.push_back(
        {point.role, related_location.file, related_location.line,
         related_location.column});
  }
}

void configure(bool enabled, const std::string &path,
               AnalysisPolicy policy, bool html_enabled,
               bool compiler_diagnostics) {
  report_enabled = enabled;
  report_html_enabled = html_enabled;
  compiler_diagnostics_enabled = compiler_diagnostics;
  configured_path = path;
  configured_policy = policy;
}

void record_callee_effect(const gimple *call, unsigned int argument_index,
                          const char *fates, bool body_available,
                          bool needs_review) {
  if (!report_enabled || call == nullptr || !is_gimple_call(call) ||
      cfun == nullptr || cfun->decl == NULL_TREE ||
      argument_index >= gimple_call_num_args(call))
    return;
  const auto key = std::make_pair(call, argument_index);
  // A stable borrow needs no secondary tracing; discard earlier unknowns.
  if (!needs_review && std::strcmp(fates, "freed") != 0) {
    callee_effects.erase(key);
    return;
  }
  auto found = callee_effects.find(key);
  if (found != callee_effects.end() && found->second.fates == fates &&
      found->second.needs_review == needs_review &&
      found->second.body_available == body_available)
    return;
  if (found == callee_effects.end() && callee_effects.size() >= max_callee_effects) {
    ++callee_effects_omitted_events;
    return;
  }
  tree declaration = gimple_call_fndecl(call);
  location_t location = gimple_location(call);
  if (location == UNKNOWN_LOCATION)
    location = DECL_SOURCE_LOCATION(cfun->decl);
  const expanded_location expanded = expand_location(location);
  CalleeEffect entry;
  entry.caller = IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(cfun->decl));
  entry.symbol = declaration == NULL_TREE ? "" :
      IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(declaration));
  entry.fates = fates;
  entry.file = expanded.file == nullptr ? "" : expanded.file;
  entry.line = expanded.line;
  entry.column = expanded.column;
  entry.argument_index = argument_index;
  entry.argument_count = gimple_call_num_args(call);
  entry.body_available = body_available;
  entry.needs_review = needs_review;
  callee_effects[key] = std::move(entry);
}

void configure_callee_model(CalleeModel model, const std::string &symbol,
                            unsigned int argument_index) {
  configured_callee_model = model;
  configured_callee_symbol = symbol;
  configured_callee_argument = argument_index;
}

void configure_call_graph(bool enabled) { call_graph_enabled = enabled; }

void collect_call_graph() {
  if (!call_graph_enabled || !report_enabled)
    return;
  call_graph_functions.clear();
  call_graph_truncated = false;
  constexpr unsigned int max_functions = 128;
  constexpr unsigned int max_statements = 4096;
  constexpr unsigned int max_blocks = 2048;
  constexpr unsigned int max_arcs = 4096;
  unsigned int statements = 0, blocks = 0, arcs = 0;
  cgraph_node *node;
  FOR_EACH_DEFINED_FUNCTION(node) {
    function *body = node->get_fun();
    if (body == nullptr || body->cfg == nullptr || node->decl == NULL_TREE)
      continue;
    if (call_graph_functions.size() >= max_functions) {
      call_graph_truncated = true;
      break;
    }
    GraphFunction entry;
    entry.symbol = IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(node->decl));
    entry.name = DECL_NAME(node->decl) == NULL_TREE ? entry.symbol :
                 IDENTIFIER_POINTER(DECL_NAME(node->decl));
    const expanded_location source = expand_location(DECL_SOURCE_LOCATION(node->decl));
    entry.file = source.file == nullptr ? "" : source.file;
    entry.line = source.line;
    edge first;
    edge_iterator first_it;
    FOR_EACH_EDGE(first, first_it, ENTRY_BLOCK_PTR_FOR_FN(body)->succs) {
      if (++arcs > max_arcs) { call_graph_truncated = true; break; }
      entry.arcs.push_back({ENTRY_BLOCK_PTR_FOR_FN(body)->index, first->dest->index, "entry"});
    }
    basic_block block;
    FOR_EACH_BB_FN(block, body) {
      if (++blocks > max_blocks) { call_graph_truncated = true; break; }
      for (gimple_stmt_iterator it = gsi_start_bb(block); !gsi_end_p(it); gsi_next(&it)) {
        const gimple *statement = gsi_stmt(it);
        if (++statements > max_statements) { call_graph_truncated = true; break; }
        const bool condition = gimple_code(statement) == GIMPLE_COND ||
                               gimple_code(statement) == GIMPLE_SWITCH;
        const bool memory = is_gimple_assign(statement) &&
                            (gimple_vuse(statement) != NULL_TREE || gimple_vdef(statement) != NULL_TREE);
        if (!is_gimple_call(statement) && !condition && !memory &&
            gimple_code(statement) != GIMPLE_RETURN)
          continue;
        GraphStatement item;
        item.block = block->index;
        item.kind = condition ? "branch" : memory ? "memory" :
                    gimple_code(statement) == GIMPLE_RETURN ? "return" : "call";
        if (is_gimple_call(statement)) {
          tree target = gimple_call_fndecl(statement);
          item.callee = target == NULL_TREE ? "<indirect>" :
                        IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(target));
          if (ownership_allocation_call_p(statement)) item.kind = "allocation";
          else if (ownership_deallocation_call_p(statement)) item.kind = "release";
        }
        const expanded_location at = expand_location(gimple_location(statement));
        item.file = at.file == nullptr ? "" : at.file;
        item.line = at.line;
        item.column = at.column;
        if (item.kind == "branch") {
          char *buffer = nullptr;
          size_t length = 0;
          FILE *stream = open_memstream(&buffer, &length);
          if (stream != nullptr) {
            print_gimple_stmt(stream, const_cast<gimple *>(statement), 0, TDF_SLIM);
            fclose(stream);
            if (buffer != nullptr) {
              item.text.assign(buffer, std::min<size_t>(length, 256));
              free(buffer);
            }
          }
        }
        entry.statements.push_back(std::move(item));
      }
      edge successor;
      edge_iterator ei;
      FOR_EACH_EDGE(successor, ei, block->succs) {
        if (++arcs > max_arcs) { call_graph_truncated = true; break; }
        entry.arcs.push_back({block->index, successor->dest->index,
          (successor->flags & EDGE_TRUE_VALUE) != 0 ? "true" :
          (successor->flags & EDGE_FALSE_VALUE) != 0 ? "false" : "normal"});
      }
      if (call_graph_truncated) break;
    }
    call_graph_functions.push_back(std::move(entry));
    if (call_graph_truncated) break;
  }
}

void reset() {
  call_graph_functions.clear();
  call_graph_truncated = false;
  emitted_findings.clear();
  findings.clear();
  inconclusive_candidates.clear();
  inconclusive_samples.clear();
  inconclusive_reason_counts.clear();
  inconclusive_samples_omitted = 0;
  inconclusive_events_after_dedupe_cap = 0;
  callee_effects.clear();
  callee_effects_omitted_events = 0;
  for (unsigned int i = 0; i < 4; ++i) {
    finding_counts[i] = 0;
    inconclusive_counts[i] = 0;
  }
}

void write_json() {
  if (!report_enabled)
    return;
  const std::string stem = report_stem();
  const std::string json_path = stem + ".json";
  const std::string html_path = stem + ".html";
  const std::string temporary_suffix =
      ".tmp." + std::to_string(static_cast<unsigned long>(getpid()));
  const std::string json_temporary_path = json_path + temporary_suffix;
  const std::string html_temporary_path = html_path + temporary_suffix;
  std::ofstream output(json_temporary_path.c_str(),
                       std::ios::out | std::ios::trunc);
  if (!output.is_open()) {
    inform(UNKNOWN_LOCATION,
           "static analyzer could not write report '%s'", json_path.c_str());
    return;
  }

  unsigned long total = 0;
  unsigned long inconclusive_total = 0;
  for (unsigned int i = 0; i < 4; ++i)
    total += finding_counts[i];
  for (unsigned int i = 0; i < 4; ++i)
    inconclusive_total += inconclusive_counts[i];
  output << "{\n"
         << "  \"schema_version\": 2,\n"
         << "  \"tool\": {\"name\": \"gcc-static-analyzer-plugin\", "
            "\"version\": \""
         << STATIC_ANALYZER_VERSION << "\"},\n"
         << "  \"analysis\": {\"engine\": \"bounded-cfg-typestate\", "
            "\"z3_compiled\": "
         << (feasibility::solver_compiled_in() ? "true" : "false")
         << ", \"smt_enabled\": "
         << (feasibility::default_limits().use_solver ? "true" : "false")
         << ", \"policy\": \""
         << analysis_policy_name(configured_policy) << "\""
         << ", \"diagnostics\": \"" << (compiler_diagnostics_enabled ? "warning" : "report-only") << "\""
         << ", \"library_model\": \"" << library_model::profile_name() << "\""
         << ", \"callee_model\": \"" << callee_model_name(configured_callee_model)
         << "\", \"standard_stdio_model\": " << (gcc_call_effect::standard_stdio_model_active() ? "true" : "false")
         << ", \"standard_stdio_model_scope\": \"" << json_escape(gcc_call_effect::standard_stdio_model_reason())
         << "\", \"callee_symbol\": \"" << json_escape(configured_callee_symbol)
         << "\", \"callee_argument\": " << configured_callee_argument
         << "},\n"
         << "  \"call_graph\": {\"enabled\": " << (call_graph_enabled ? "true" : "false")
         << ", \"truncated\": " << (call_graph_truncated ? "true" : "false")
         << ", \"scope\": \"loaded GCC call sites and CFG; structural evidence, not a feasible-path certificate\", \"functions\": [";
  for (std::size_t i = 0; i < call_graph_functions.size(); ++i) {
    const GraphFunction &function = call_graph_functions[i];
    if (i != 0) output << ',';
    output << "{\"symbol\":\"" << json_escape(function.symbol) << "\",\"name\":\""
           << json_escape(function.name) << "\",\"file\":\"" << json_escape(function.file)
           << "\",\"line\":" << function.line << ",\"statements\":[";
    for (std::size_t j = 0; j < function.statements.size(); ++j) {
      const GraphStatement &item = function.statements[j];
      if (j != 0) output << ',';
      output << "{\"kind\":\"" << item.kind << "\",\"callee\":\"" << json_escape(item.callee)
             << "\",\"text\":\"" << json_escape(item.text) << "\",\"block\":" << item.block
             << ",\"file\":\"" << json_escape(item.file) << "\",\"line\":" << item.line
             << ",\"column\":" << item.column << '}';
    }
    output << "],\"arcs\":[";
    for (std::size_t j = 0; j < function.arcs.size(); ++j) {
      const GraphFunction::Arc &arc = function.arcs[j];
      if (j != 0) output << ',';
      output << "{\"from\":" << arc.from << ",\"to\":" << arc.to
             << ",\"branch\":\"" << arc.branch << "\"}";
    }
    output << "]}";
  }
  output << "]},\n"
         << "  \"source\": \""
         << json_escape(main_input_filename == nullptr ? "" :
                        main_input_filename)
         << "\",\n"
         << "  \"summary\": {\"total\": " << total
         << ", \"SA001\": " << finding_counts[0]
         << ", \"SA002\": " << finding_counts[1]
         << ", \"SA003\": " << finding_counts[2]
         << ", \"SA004\": " << finding_counts[3]
         << ", \"model_review_total\": "
         << std::count_if(findings.begin(), findings.end(),
                          [](const Finding &f) { return !f.model_scope.empty(); })
         << ", \"non_model_total\": "
         << std::count_if(findings.begin(), findings.end(),
                          [](const Finding &f) { return f.model_scope.empty(); })
         << ", \"inconclusive_total\": " << inconclusive_total
         << "},\n"
         << "  \"findings\": [\n";
  const char *finding_status =
      configured_callee_model != CalleeModel::unknown ? "ASSUMPTION_BASED" :
      configured_policy == AnalysisPolicy::proof_only
          ? "PROVEN"
          : "EXPLORATORY";
  const char *finding_confidence =
      configured_callee_model != CalleeModel::unknown ? "conditional" :
      configured_policy == AnalysisPolicy::proof_only
          ? "high"
          : "uncertain";
  for (std::size_t i = 0; i < findings.size(); ++i) {
    const Finding &finding = findings[i];
    output << "    {\"rule_id\": \"" << code(finding.kind)
           << "\", \"status\": \"" << (finding.model_scope.empty() ? finding_status : "MODEL_REVIEW")
           << "\", \"confidence\": \"" << (finding.model_scope.empty() ? finding_confidence : "conditional") << "\", "
              "\"severity\": \"warning\", \"message\": \""
           << json_escape(message(finding.kind))
           << "\", \"remediation\": {\"kind\": \"manual-guidance\", "
              "\"automatic_patch\": false, \"summary\": \""
           << json_escape(remediation(finding.kind)) << "\"}"
           << ", \"model_scope\": \"" << json_escape(finding.model_scope)
           << "\", \"path_verdict\": \"" << json_escape(finding.model_path_verdict)
           << "\", \"location\": {\"file\": \""
           << json_escape(finding.file) << "\", \"line\": "
           << finding.line << ", \"column\": " << finding.column
           << "}, \"trace\": [";
    for (std::size_t trace_index = 0;
         trace_index < finding.trace.size(); ++trace_index) {
      const Finding::ExpandedTracePoint &point = finding.trace[trace_index];
      output << "{\"role\": \"" << json_escape(point.role)
             << "\", \"file\": \"" << json_escape(point.file)
             << "\", \"line\": " << point.line << ", \"column\": "
             << point.column << "}";
      if (trace_index + 1 != finding.trace.size())
        output << ',';
    }
    output << "]}";
    if (i + 1 != findings.size())
      output << ',';
    output << '\n';
  }
  output << "  ],\n"
         << "  \"inconclusive_summary\": {\"total\": "
         << inconclusive_total << ", \"sample_limit\": "
         << max_inconclusive_samples << ", \"samples_recorded\": "
         << inconclusive_samples.size() << ", \"samples_omitted\": "
         << inconclusive_samples_omitted << ", \"dedupe_key_limit\": "
         << max_inconclusive_dedupe_keys
         << ", \"dedupe_saturated\": "
         << (inconclusive_events_after_dedupe_cap == 0 ? "false" : "true")
         << ", \"events_after_dedupe_cap\": "
         << inconclusive_events_after_dedupe_cap << ", \"by_rule\": {"
         << "\"SA001\": " << inconclusive_counts[0]
         << ", \"SA002\": " << inconclusive_counts[1]
         << ", \"SA003\": " << inconclusive_counts[2]
         << ", \"SA004\": " << inconclusive_counts[3]
         << "}, \"by_reason\": {";
  std::size_t reason_index = 0;
  for (const auto &item : inconclusive_reason_counts) {
    if (reason_index++ != 0)
      output << ',';
    output << "\"" << json_escape(item.first) << "\": " << item.second;
  }
  output << "}},\n  \"inconclusive\": [\n";
  for (std::size_t i = 0; i < inconclusive_samples.size(); ++i) {
    const InconclusiveSample &sample = inconclusive_samples[i];
    output << "    {\"rule_id\": \"" << code(sample.kind)
           << "\", \"status\": \"INCONCLUSIVE\", \"reason\": \""
           << json_escape(sample.reason) << "\", \"detail\": \""
           << json_escape(sample.detail)
           << "\", \"location\": {\"file\": \""
           << json_escape(sample.file) << "\", \"line\": "
           << sample.line << ", \"column\": " << sample.column << "}";
    if (!sample.related.empty()) {
      output << ", \"related\": [";
      for (std::size_t point_index = 0; point_index < sample.related.size();
           ++point_index) {
        const Finding::ExpandedTracePoint &point = sample.related[point_index];
        if (point_index != 0)
          output << ',';
        output << "{\"role\": \"" << json_escape(point.role)
               << "\", \"file\": \"" << json_escape(point.file)
               << "\", \"line\": " << point.line
               << ", \"column\": " << point.column << '}';
      }
      output << ']';
    }
    output << '}';
    if (i + 1 != inconclusive_samples.size())
      output << ',';
    output << '\n';
  }
  std::vector<CalleeEffect> effects;
  for (const auto &item : callee_effects)
    effects.push_back(item.second);
  std::sort(effects.begin(), effects.end(),
            [](const CalleeEffect &a, const CalleeEffect &b) {
    return std::tie(a.file, a.line, a.column, a.caller, a.symbol, a.argument_index) <
           std::tie(b.file, b.line, b.column, b.caller, b.symbol, b.argument_index);
  });
  unsigned int review_count = 0;
  for (const CalleeEffect &effect : effects)
    review_count += effect.needs_review ? 1 : 0;
  output << "  ],\n  \"callee_effect_summary\": {\"record_limit\": "
         << max_callee_effects << ", \"recorded\": " << effects.size()
         << ", \"needs_review\": " << review_count
         << ", \"known_release\": " << effects.size() - review_count
         << ", \"omitted_events\": " << callee_effects_omitted_events
         << ", \"binary_tracing\": \"not-run\"},\n"
         << "  \"callee_effects\": [\n";
  for (std::size_t i = 0; i < effects.size(); ++i) {
    const CalleeEffect &effect = effects[i];
    output << "    {\"caller\": \"" << json_escape(effect.caller)
           << "\", \"symbol\": \"" << json_escape(effect.symbol)
           << "\", \"argument_index\": " << effect.argument_index
           << ", \"argument_count\": " << effect.argument_count
           << ", \"fates\": \"" << json_escape(effect.fates)
           << "\", \"body_available\": "
           << (effect.body_available ? "true" : "false")
           << ", \"needs_review\": " << (effect.needs_review ? "true" : "false")
           << ", \"location\": {\"file\": \"" << json_escape(effect.file)
           << "\", \"line\": " << effect.line << ", \"column\": "
           << effect.column << "}}" << (i + 1 == effects.size() ? "" : ",") << '\n';
  }
  output << "  ]\n}\n";
  output.close();
  if (output.fail()) {
    std::remove(json_temporary_path.c_str());
    inform(UNKNOWN_LOCATION,
           "static analyzer could not finish report '%s'", json_path.c_str());
    return;
  }

  // Project builds merge per-TU JSON documents into one HTML dashboard.
  // Avoid generating thousands of redundant HTML files in that mode while
  // keeping the default one-shot compiler experience unchanged.
  if (!report_html_enabled) {
    // Do not leave an older HTML document beside a newer JSON-only result.
    std::remove(html_path.c_str());
    if (std::rename(json_temporary_path.c_str(), json_path.c_str()) != 0) {
      const int saved_errno = errno;
      std::remove(json_temporary_path.c_str());
      inform(UNKNOWN_LOCATION,
             "static analyzer could not publish report '%s' (errno %d)",
             json_path.c_str(), saved_errno);
      return;
    }
    inform(UNKNOWN_LOCATION,
           "static analyzer: %lu finding(s), %lu inconclusive candidate(s); "
           "report: %s", total, inconclusive_total, json_path.c_str());
    return;
  }

  std::ofstream html(html_temporary_path.c_str(),
                     std::ios::out | std::ios::trunc);
  if (!html.is_open()) {
    std::remove(json_temporary_path.c_str());
    inform(UNKNOWN_LOCATION,
           "static analyzer could not write report '%s'", html_path.c_str());
    return;
  }
  const std::string source =
      main_input_filename == nullptr ? "" : main_input_filename;
  html << "<!doctype html>\n<html lang=\"en\"><head>\n"
       << "<meta charset=\"utf-8\"><meta name=\"viewport\" "
          "content=\"width=device-width,initial-scale=1\">\n"
       << "<title>Static Analyzer Report</title>\n"
       << "<style>*{box-sizing:border-box}body{font:15px system-ui,sans-serif;margin:0;background:#f5f7fa;color:#172033}"
          "main{width:100%;max-width:960px;margin:32px auto;padding:0 20px;overflow-wrap:anywhere}h1{margin-bottom:6px}"
          ".source{color:#526079;overflow-wrap:anywhere}.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(120px,1fr));gap:12px;margin:28px 0}"
          ".card,table{background:white;border:1px solid #dce2ea;border-radius:10px;box-shadow:0 2px 8px #18243a0d}"
          ".card{padding:16px}.count{font-size:28px;font-weight:700}.label{color:#65718a}"
          "table{width:100%;table-layout:fixed;border-collapse:separate;border-spacing:0;overflow:hidden}th,td{padding:13px;text-align:left;border-bottom:1px solid #e7ebf0;vertical-align:top;overflow-wrap:anywhere}"
          "th:first-child{width:22%}th:last-child{width:30%}"
          "th{background:#eef2f7}.rule{font-weight:700;color:#a51d2d}.ok{padding:28px;color:#177245;font-weight:650}"
          ".inconclusive{color:#875c00}.note{margin:22px 0 8px;color:#526079}"
          "code{white-space:normal;overflow-wrap:anywhere}.trace{margin:8px 0 0;padding-left:20px;color:#526079;font-size:13px}footer{margin-top:18px;color:#778196;font-size:13px}"
          "@media(max-width:640px){main{margin:20px auto;padding:0 12px}.cards{grid-template-columns:repeat(2,minmax(0,1fr))}"
          "table{font-size:13px}thead{display:none}tbody,tr,td{display:block;width:100%}td{padding:10px 12px}tr+tr{border-top:2px solid #dce2ea}td:last-child{color:#526079}}"
          "</style></head><body><main>\n"
       << "<h1>Static Analyzer Report</h1><div class=\"source\">"
       << html_escape(source) << "</div>\n<div class=\"cards\">\n"
       << "<div class=\"card\"><div class=\"count\">" << total
       << "</div><div class=\"label\">Total findings</div></div>\n";
  if (configured_callee_model != CalleeModel::unknown)
    html << "<p class=\"inconclusive\">Assumption-based results: "
         << html_escape(configured_callee_symbol) << ", argument "
         << configured_callee_argument + 1 << ", assumed effect "
         << callee_model_name(configured_callee_model)
         << ". This experiment is not a source-derived ownership proof.</p>";
  for (unsigned int i = 0; i < 4; ++i) {
    const FindingKind kind = static_cast<FindingKind>(i);
    html << "<div class=\"card\"><div class=\"count\">"
         << finding_counts[i] << "</div><div class=\"label\">"
         << code(kind) << "</div></div>\n";
  }
  html << "<div class=\"card\"><div class=\"count\">"
       << inconclusive_total
       << "</div><div class=\"label\">Inconclusive</div></div>\n"
       << "</div>\n<table><thead><tr><th>Rule</th><th>Problem</th>"
          "<th>Location</th></tr></thead><tbody>\n";
  if (findings.empty()) {
    html << "<tr><td class=\"ok\" colspan=\"3\">No findings.</td></tr>\n";
  } else {
    for (const Finding &finding : findings) {
      html << "<tr><td class=\"rule\">" << code(finding.kind)
           << "</td><td>" << html_escape(message(finding.kind))
           << "<div class=\"note\">Status: <strong>"
           << (finding.model_scope.empty() ? finding_status : "MODEL_REVIEW") << "</strong> · confidence "
           << (finding.model_scope.empty() ? finding_confidence : "conditional") << "</div>"
           << "<div class=\"note\"><strong>Suggested repair "
              "(manual):</strong> "
           << html_escape(remediation(finding.kind)) << "</div>";
      if (!finding.model_scope.empty())
        html << "<p class=\"inconclusive\">Library review: "
             << html_escape(finding.model_scope) << "; path="
             << html_escape(finding.model_path_verdict)
             << ". This is not an unconditional body/path proof.</p>";
      if (!finding.trace.empty()) {
        html << "<ol class=\"trace\">";
        for (const Finding::ExpandedTracePoint &point : finding.trace)
          html << "<li>" << html_escape(point.role) << ": <code>"
               << html_escape(point.file) << ':' << point.line << ':'
               << point.column << "</code></li>";
        html << "</ol>";
      }
      html << "</td><td><code>" << html_escape(finding.file) << ':'
           << finding.line << ':' << finding.column << "</code></td></tr>\n";
    }
  }
  html << "</tbody></table>\n<h2>Inconclusive candidates</h2>"
       << "<p class=\"note\">These records are not findings and did not "
          "emit compiler warnings. Samples are bounded to "
       << max_inconclusive_samples << ".</p>\n"
       << "<table><thead><tr><th>Rule</th><th>Status / reason</th>"
          "<th>Location</th></tr></thead><tbody>\n";
  if (inconclusive_samples.empty()) {
    html << "<tr><td class=\"ok\" colspan=\"3\">No inconclusive "
            "candidates.</td></tr>\n";
  } else {
    for (const InconclusiveSample &sample : inconclusive_samples) {
      html << "<tr><td class=\"rule\">" << code(sample.kind)
           << "</td><td class=\"inconclusive\">INCONCLUSIVE · "
           << html_escape(sample.reason);
      if (!sample.detail.empty())
        html << "<div class=\"note\">" << html_escape(sample.detail)
             << "</div>";
      if (!sample.related.empty()) {
        html << "<ul class=\"trace\">";
        for (const Finding::ExpandedTracePoint &point : sample.related)
          html << "<li>" << html_escape(point.role) << ": <code>"
               << html_escape(point.file) << ':' << point.line << ':'
               << point.column << "</code></li>";
        html << "</ul>";
      }
      if (sample.kind == FindingKind::memory_leak &&
          (sample.reason == "ambiguous-callee-ownership" ||
           sample.reason == "opaque-ownership-transfer")) {
        html << "<details class=\"note\"><summary>Ownership assumptions "
                "to check</summary><p>If ownership is preserved without "
                "release or transfer, the outstanding object may leak. "
                "If it is released or transferred, a local leak is not "
                "established; later accesses require a separate lifetime "
                "check.</p><p>These are model alternatives, not verified "
                "executions. This record remains INCONCLUSIVE.</p></details>";
      }
      html << "</td><td><code>" << html_escape(sample.file) << ':'
           << sample.line << ':' << sample.column << "</code></td></tr>\n";
    }
  }
  html << "</tbody></table><h2>Callee effects to review</h2>"
       << "<p class=\"note\">" << review_count << " call arguments need review; "
       << effects.size() - review_count << " known release arguments recorded. "
       << "Binary tracing runs only when requested after linking. These records "
          "do not count as findings. List limit: " << max_callee_effects
       << "; omitted observations: " << callee_effects_omitted_events << ".</p>"
       << "<table><thead><tr><th>Caller / callee</th><th>Effect</th>"
          "<th>Location</th></tr></thead><tbody>";
  for (const CalleeEffect &effect : effects) {
    html << "<tr><td>" << html_escape(effect.caller) << " &rarr; "
         << html_escape(effect.symbol.empty() ? "indirect call" : effect.symbol)
         << "<div class=\"note\">Argument " << effect.argument_index + 1
         << "</div></td><td>" << html_escape(effect.fates)
         << "<div class=\"note\">"
         << (effect.needs_review
                 ? (effect.body_available ? "Source effect needs refinement"
                                          : "External body unavailable; binary trace pending")
                 : "Known release effect")
         << "</div></td><td><code>" << html_escape(effect.file) << ':'
         << effect.line << ':' << effect.column << "</code></td></tr>";
  }
  html << "</tbody></table><footer>gcc-static-analyzer-plugin "
       << STATIC_ANALYZER_VERSION << " · "
          "bounded CFG fixed-point analysis · policy "
       << analysis_policy_name(configured_policy) << " · SMT "
       << (feasibility::default_limits().use_solver &&
                   feasibility::solver_compiled_in()
               ? "enabled"
               : "disabled")
       << "</footer>\n"
          "</main></body></html>\n";
  html.close();
  if (html.fail()) {
    std::remove(json_temporary_path.c_str());
    std::remove(html_temporary_path.c_str());
    inform(UNKNOWN_LOCATION,
           "static analyzer could not finish report '%s'", html_path.c_str());
    return;
  }

  // Each file becomes visible with one rename, so report consumers never
  // parse a half-written JSON/HTML document while parallel compilation is
  // still flushing it.
  if (std::rename(html_temporary_path.c_str(), html_path.c_str()) != 0) {
    const int saved_errno = errno;
    std::remove(json_temporary_path.c_str());
    std::remove(html_temporary_path.c_str());
    inform(UNKNOWN_LOCATION,
           "static analyzer could not publish report '%s' (errno %d)",
           html_path.c_str(), saved_errno);
    return;
  }
  if (std::rename(json_temporary_path.c_str(), json_path.c_str()) != 0) {
    const int saved_errno = errno;
    std::remove(json_temporary_path.c_str());
    inform(UNKNOWN_LOCATION,
           "static analyzer could not publish report '%s' (errno %d)",
           json_path.c_str(), saved_errno);
    return;
  }
  inform(UNKNOWN_LOCATION,
         "static analyzer: %lu finding(s), %lu inconclusive candidate(s); "
         "reports: %s, %s", total, inconclusive_total, html_path.c_str(),
         json_path.c_str());
}

void dump_summary() {
  if (dump_file == nullptr)
    return;
  dump_printf(MSG_NOTE,
              "finding summary: SA001=%lu SA002=%lu SA003=%lu SA004=%lu "
              "inconclusive=%lu\n",
              finding_counts[0], finding_counts[1], finding_counts[2],
              finding_counts[3], inconclusive_counts[0] +
                  inconclusive_counts[1] + inconclusive_counts[2] +
                  inconclusive_counts[3]);
}

const char *code(FindingKind kind) {
  switch (kind) {
    case FindingKind::memory_leak:
      return "SA001";
    case FindingKind::double_free:
      return "SA002";
    case FindingKind::use_after_free:
      return "SA003";
    case FindingKind::joinable_thread_leak:
      return "SA004";
  }
  return "SA000";
}

const char *message(FindingKind kind) {
  switch (kind) {
    case FindingKind::memory_leak:
      return "allocated object may reach function exit without release";
    case FindingKind::double_free:
      return "the same allocated object may be freed twice on one feasible path";
    case FindingKind::use_after_free:
      return "a proven alias of the released object may be used later";
    case FindingKind::joinable_thread_leak:
      return "joinable thread may reach function exit without join or detach";
  }
  return "unknown static analyzer finding";
}

const char *remediation(FindingKind kind) {
  switch (kind) {
    case FindingKind::memory_leak:
      return "release the owned object on every normal exit after its last "
             "use, or make the ownership transfer explicit";
    case FindingKind::double_free:
      return "make the release paths mutually exclusive and keep exactly "
             "one owner responsible for deallocation";
    case FindingKind::use_after_free:
      return "move the release after the last required access; when returning "
             "the pointer, return null or transfer live ownership instead; "
             "otherwise stop using every alias after release";
    case FindingKind::joinable_thread_leak:
      return "join the successfully created thread on every exit, or detach "
             "it when no join is required";
  }
  return "review the ownership protocol before changing the source";
}

}  // namespace report
}  // namespace static_analyzer
