#include <chrono>
#include <cstring>
#include <thread>

#include "gcc-plugin.h"

#include "static_analyzer/analysis_budget.h"

namespace budget = static_analyzer::analysis_budget;

int main() {
  budget::Limits limits;
  limits.tu_cpu_ms = 1000;
  limits.function_cpu_ms = 50;
  limits.tu_work = 100000000;
  limits.function_work = 100000000;

  budget::reset(limits);
  if (!budget::begin_function(nullptr))
    return 1;

  // This wait is four times the function CPU allowance.  A wall-clock
  // deadline would reject the function; a process-CPU budget must retain it.
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  if (!budget::function_available() || !budget::tu_available())
    return 2;

  // Actual CPU work must still exhaust the function time budget.
  volatile unsigned long value = 1;
  while (budget::available())
    for (unsigned int index = 0; index < 10000; ++index)
      value = value * 1664525UL + 1013904223UL;
  if (budget::reason() != budget::ExhaustionReason::function_cpu_time)
    return 3;
  if (std::strcmp(
          budget::reason_name(budget::ExhaustionReason::function_cpu_time),
          "function-cpu-time") != 0)
    return 4;
  budget::end_function();

  // Deterministic caps remain independent of the CPU clock.
  limits.tu_cpu_ms = 1000;
  limits.function_cpu_ms = 1000;
  limits.tu_work = 4;
  limits.function_work = 3;
  budget::reset(limits);
  if (!budget::begin_function(nullptr) ||
      !budget::consume(budget::WorkKind::lifetime_cfg, 3) ||
      budget::consume(budget::WorkKind::lifetime_cfg, 1) ||
      budget::reason() != budget::ExhaustionReason::function_work)
    return 5;
  budget::end_function();

  (void)value;
  return 0;
}
