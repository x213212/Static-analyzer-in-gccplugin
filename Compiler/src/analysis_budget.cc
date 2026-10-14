#include "gcc-plugin.h"
#include "tree.h"
#include "dumpfile.h"

#include "static_analyzer/analysis_budget.h"

#include <cstdint>
#include <ctime>

namespace static_analyzer {
namespace analysis_budget {
namespace {

constexpr std::uint64_t nanoseconds_per_millisecond = 1000000ULL;

struct State {
  Limits limits;
  std::uint64_t tu_cpu_start_ns = 0;
  std::uint64_t function_cpu_start_ns = 0;
  std::uint64_t last_cpu_ns = 0;
  unsigned long tu_work = 0;
  unsigned long function_work = 0;
  unsigned long work_by_kind[static_cast<unsigned int>(WorkKind::count)] = {};
  unsigned long next_tu_cpu_check = 0;
  unsigned long next_function_cpu_check = 0;
  unsigned long cpu_clock_reads = 0;
  unsigned long cpu_clock_failures = 0;
  unsigned long functions_started = 0;
  unsigned long functions_completed = 0;
  unsigned long functions_exhausted = 0;
  unsigned long functions_skipped = 0;
  unsigned long exhaustion_counts[6] = {};
  bool initialized = false;
  bool function_active = false;
  bool cpu_clock_available = false;
  ExhaustionReason tu_reason = ExhaustionReason::none;
  ExhaustionReason function_reason = ExhaustionReason::none;
  ExhaustionReason last_reason = ExhaustionReason::none;
};

State state;
constexpr unsigned long cpu_check_work_interval = 256;

bool read_process_cpu_time(std::uint64_t *result) {
#ifdef CLOCK_PROCESS_CPUTIME_ID
  struct timespec value = {};
  if (::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &value) != 0 ||
      value.tv_sec < 0 || value.tv_nsec < 0)
    return false;
  *result = static_cast<std::uint64_t>(value.tv_sec) * 1000000000ULL +
            static_cast<std::uint64_t>(value.tv_nsec);
  return true;
#else
  (void)result;
  return false;
#endif
}

bool cpu_limit_reached(std::uint64_t now, std::uint64_t start,
                       unsigned int limit_ms) {
  if (now < start)
    return false;
  return (now - start) / nanoseconds_per_millisecond >= limit_ms;
}

unsigned int reason_index(ExhaustionReason value) {
  return static_cast<unsigned int>(value) - 1;
}

void exhaust(ExhaustionReason value, bool translation_unit) {
  ExhaustionReason &destination =
      translation_unit ? state.tu_reason : state.function_reason;
  if (destination != ExhaustionReason::none)
    return;
  destination = value;
  state.last_reason = value;
  ++state.exhaustion_counts[reason_index(value)];
}

bool check_cpu_time(bool force) {
  if (state.tu_reason != ExhaustionReason::none ||
      state.function_reason != ExhaustionReason::none)
    return false;
  const bool check_tu =
      force || state.tu_work >= state.next_tu_cpu_check;
  const bool check_function =
      state.function_active &&
      (force || state.function_work >= state.next_function_cpu_check);
  if (!check_tu && !check_function)
    return state.tu_reason == ExhaustionReason::none &&
           state.function_reason == ExhaustionReason::none;

  if (!state.cpu_clock_available)
    return state.tu_reason == ExhaustionReason::none &&
           state.function_reason == ExhaustionReason::none;

  std::uint64_t now = 0;
  ++state.cpu_clock_reads;
  if (!read_process_cpu_time(&now)) {
    ++state.cpu_clock_failures;
    state.cpu_clock_available = false;
    return state.tu_reason == ExhaustionReason::none &&
           state.function_reason == ExhaustionReason::none;
  }
  state.last_cpu_ns = now;
  if (check_tu) {
    state.next_tu_cpu_check =
        state.tu_work + cpu_check_work_interval;
    if (cpu_limit_reached(now, state.tu_cpu_start_ns,
                          state.limits.tu_cpu_ms))
      exhaust(ExhaustionReason::tu_cpu_time, true);
  }
  if (check_function) {
    state.next_function_cpu_check =
        state.function_work + cpu_check_work_interval;
    if (cpu_limit_reached(now, state.function_cpu_start_ns,
                          state.limits.function_cpu_ms))
      exhaust(ExhaustionReason::function_cpu_time, false);
  }
  return state.tu_reason == ExhaustionReason::none &&
         state.function_reason == ExhaustionReason::none;
}

}  // namespace

void reset(const Limits &limits) {
  state = State{};
  state.limits = limits;
  std::uint64_t now = 0;
  state.cpu_clock_available = read_process_cpu_time(&now);
  state.cpu_clock_reads = 1;
  if (!state.cpu_clock_available)
    state.cpu_clock_failures = 1;
  state.tu_cpu_start_ns = now;
  state.last_cpu_ns = now;
  state.next_tu_cpu_check = cpu_check_work_interval;
  state.initialized = true;
}

bool begin_function(tree) {
  if (!state.initialized)
    reset();
  if (!check_cpu_time(true) || state.tu_reason != ExhaustionReason::none) {
    ++state.functions_skipped;
    return false;
  }
  state.function_active = true;
  state.function_reason = ExhaustionReason::none;
  state.function_work = 0;
  state.next_function_cpu_check = cpu_check_work_interval;
  // check_cpu_time(true) immediately above supplied this process-CPU sample.
  // Reusing it avoids a second syscall on every function entry.
  state.function_cpu_start_ns = state.last_cpu_ns;
  ++state.functions_started;
  return true;
}

void end_function() {
  if (!state.function_active)
    return;
  if (state.function_reason == ExhaustionReason::none)
    ++state.functions_completed;
  else
    ++state.functions_exhausted;
  state.function_active = false;
  state.function_reason = ExhaustionReason::none;
  state.function_work = 0;
}

bool consume(WorkKind kind, unsigned long amount) {
  if (!state.initialized)
    reset();
  if (!check_cpu_time(false))
    return false;
  if (amount > state.limits.tu_work - state.tu_work) {
    exhaust(ExhaustionReason::tu_work, true);
    return false;
  }
  if (state.function_active &&
      amount > state.limits.function_work - state.function_work) {
    exhaust(ExhaustionReason::function_work, false);
    return false;
  }
  state.tu_work += amount;
  if (state.function_active)
    state.function_work += amount;
  state.work_by_kind[static_cast<unsigned int>(kind)] += amount;
  return check_cpu_time(false);
}

bool available() {
  return check_cpu_time(true) && state.tu_reason == ExhaustionReason::none &&
         state.function_reason == ExhaustionReason::none;
}

bool tu_available() {
  check_cpu_time(true);
  return state.tu_reason == ExhaustionReason::none;
}

bool function_available() {
  check_cpu_time(true);
  return !state.function_active ||
         state.function_reason == ExhaustionReason::none;
}

bool accept_summary_node_count(unsigned long nodes) {
  if (nodes <= state.limits.summary_nodes)
    return true;
  exhaust(ExhaustionReason::summary_node_limit, true);
  return false;
}

ExhaustionReason reason() {
  if (state.tu_reason != ExhaustionReason::none)
    return state.tu_reason;
  return state.function_reason;
}

const char *reason_name(ExhaustionReason value) {
  switch (value) {
    case ExhaustionReason::none: return "none";
    case ExhaustionReason::tu_cpu_time: return "tu-cpu-time";
    case ExhaustionReason::function_cpu_time: return "function-cpu-time";
    case ExhaustionReason::tu_work: return "tu-work";
    case ExhaustionReason::function_work: return "function-work";
    case ExhaustionReason::summary_node_limit: return "summary-node-limit";
  }
  return "unknown";
}

void dump_statistics() {
  if (dump_file == nullptr)
    return;
  std::uint64_t observed_cpu_ns = state.last_cpu_ns;
  if (state.cpu_clock_available) {
    ++state.cpu_clock_reads;
    if (read_process_cpu_time(&observed_cpu_ns)) {
      state.last_cpu_ns = observed_cpu_ns;
    } else {
      ++state.cpu_clock_failures;
      state.cpu_clock_available = false;
    }
  }
  const unsigned long observed_cpu_ms =
      observed_cpu_ns >= state.tu_cpu_start_ns
          ? static_cast<unsigned long>(
                (observed_cpu_ns - state.tu_cpu_start_ns) /
                nanoseconds_per_millisecond)
          : 0;
  dump_printf(MSG_NOTE,
              "analysis-budget summary: tu-work=%lu/%lu functions-started=%lu "
              "completed=%lu exhausted=%lu skipped=%lu reason=%s "
              "cpu-clock=%s observed-cpu-ms=%lu cpu-reads=%lu "
              "cpu-read-failures=%lu limits-tu-cpu-ms=%u "
              "function-cpu-ms=%u function-work=%lu\n",
              state.tu_work, state.limits.tu_work, state.functions_started,
              state.functions_completed, state.functions_exhausted,
              state.functions_skipped, reason_name(state.last_reason),
              state.cpu_clock_available ? "process" : "unavailable",
              observed_cpu_ms, state.cpu_clock_reads,
              state.cpu_clock_failures, state.limits.tu_cpu_ms,
              state.limits.function_cpu_ms,
              state.limits.function_work);
  dump_printf(MSG_NOTE,
              "analysis-budget work: summary-node=%lu dependency=%lu scc=%lu "
              "order=%lu parameter-cfg=%lu lifetime-discovery=%lu "
              "allocation-statement=%lu lifetime-cfg=%lu "
              "exhaust-tu-cpu-time=%lu function-cpu-time=%lu tu-work=%lu "
              "function-work=%lu node-limit=%lu\n",
              state.work_by_kind[0], state.work_by_kind[1],
              state.work_by_kind[2], state.work_by_kind[3],
              state.work_by_kind[4], state.work_by_kind[5],
              state.work_by_kind[6], state.work_by_kind[7],
              state.exhaustion_counts[0], state.exhaustion_counts[1],
              state.exhaustion_counts[2], state.exhaustion_counts[3],
              state.exhaustion_counts[4]);
}

}  // namespace analysis_budget
}  // namespace static_analyzer
