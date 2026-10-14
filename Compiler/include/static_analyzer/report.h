#ifndef STATIC_ANALYZER_REPORT_H
#define STATIC_ANALYZER_REPORT_H

#include "coretypes.h"

#include <string>
#include <vector>

namespace static_analyzer {

enum class AnalysisPolicy;
enum class CalleeModel;

namespace report {

enum class FindingKind {
  memory_leak,
  double_free,
  use_after_free,
  joinable_thread_leak
};

struct TracePoint {
  location_t location = UNKNOWN_LOCATION;
  std::string role;
};

// Emit one stable GCC diagnostic.  Findings are deduplicated by kind and
// location; debug traces and dump evidence use separate output channels.
bool emit(FindingKind kind, location_t location,
          const std::vector<TracePoint> &trace = std::vector<TracePoint>());
// Explicit library-profile review, distinct from a PROVEN body/path finding.
// An UNKNOWN path is retained as such; consumers must not count it as proof.
bool emit_model_review(FindingKind kind, location_t location,
                       const std::string &scope, const std::string &path_verdict,
                       const std::vector<TracePoint> &trace);

// Record a candidate which could not satisfy the active proof contract.
// This never emits a GCC warning and never changes SA001--SA004 counts.
// Aggregate counts are complete; serialized samples are globally bounded.
void record_inconclusive(FindingKind kind, location_t location,
                         const std::string &reason,
                         const std::string &detail = std::string(),
                         const std::vector<TracePoint> &related =
                             std::vector<TracePoint>());

// Bounded report-only ledger for optional post-link binary investigation.
// A queried effect may change while source summaries reach their fixed point;
// repeated queries replace the same call/argument entry. Known borrows remove
// provisional entries. This API never changes a finding or ownership state.
void record_callee_effect(const gimple *call, unsigned int argument_index,
                          const char *fates, bool body_available,
                          bool needs_review);

void configure(bool enabled, const std::string &path,
               AnalysisPolicy policy, bool html_enabled = true,
               bool compiler_diagnostics = true);
void configure_callee_model(CalleeModel model, const std::string &symbol,
                            unsigned int argument_index);
void configure_call_graph(bool enabled);
void collect_call_graph();
void reset();
void dump_summary();
void write_json();
const char *code(FindingKind kind);
const char *message(FindingKind kind);
// A conservative source-level recommendation.  This is intentionally not a
// generated patch: automatic edits require a separate must-quality repair
// certificate and recompile/verify workflow.
const char *remediation(FindingKind kind);

}  // namespace report
}  // namespace static_analyzer

#endif
