PYTHON ?= python3
PLUGIN_CXX ?= $(shell $(MAKE) --no-print-directory -s -C Compiler print-cxx)
PLUGIN_CC ?= $(subst g++,gcc,$(PLUGIN_CXX))
# Prefer the side-by-side GCC 16.2 toolchain used by the reproducible
# comparison.  Callers can still override GCC16_PREFIX, GCC16_CC, or
# GCC16_CXX; the comparison runners independently reject versions other than
# 16.2.x.
GCC16_PREFIX ?= /root/toolchains/gcc-16.2
GCC16_CC ?= $(if $(wildcard $(GCC16_PREFIX)/bin/gcc),$(GCC16_PREFIX)/bin/gcc,gcc-16)
GCC16_CXX ?= $(if $(wildcard $(GCC16_PREFIX)/bin/g++),$(GCC16_PREFIX)/bin/g++,g++-16)
ARTIFACT_DIR ?= artifacts
SMOKE_DIR ?= $(ARTIFACT_DIR)/smoke
MEMORY_CACHE_SMOKE_DIR ?= $(ARTIFACT_DIR)/memory-cache-smoke
ALIAS_CACHE_SMOKE_DIR ?= $(ARTIFACT_DIR)/alias-cache-smoke
PATH_SCALING_SMOKE_DIR ?= $(ARTIFACT_DIR)/path-scaling-smoke
FEASIBILITY_BUDGET_SMOKE_DIR ?= $(ARTIFACT_DIR)/feasibility-budget-smoke
GIMPLE_VALUE_SMOKE_DIR ?= $(ARTIFACT_DIR)/gimple-value-smoke
ANALYSIS_BUDGET_SMOKE_DIR ?= $(ARTIFACT_DIR)/analysis-budget-smoke
ANALYSIS_BUDGET_CPU_SMOKE_DIR ?= $(ARTIFACT_DIR)/analysis-budget-cpu-smoke
TEST_DIR ?= $(ARTIFACT_DIR)/tests
BENCHMARK_DIR ?= $(ARTIFACT_DIR)/benchmarks
GCC16_COMPARE_DIR ?= $(ARTIFACT_DIR)/gcc16-comparison
ANALYZE_DIR ?= $(ARTIFACT_DIR)/analyze
PROJECT_TU_REPORT_DIR ?= $(ARTIFACT_DIR)/project-tu-reports
PROJECT_REPORT_DIR ?= $(ARTIFACT_DIR)/project-report
JULIET_ACCURACY_JOBS ?= 8
JULIET_TIMING_RUN ?= 1

# Shared compiler flags. Preserve intended lifetime events in proof fixtures.
ANALYSIS_CFLAGS := -fdiagnostics-color=never -O1 -fipa-pta -fno-inline
PRESERVE_LIFETIMES_CFLAGS := -fno-tree-dce -fno-tree-dse -fno-tree-fre
# Analysis tasks load immutable per-toolchain paths, so another build cannot
# replace their plugin through the convenient Compiler/newplugin.so copy.
PLUGIN_BINARY = $(shell $(MAKE) --no-print-directory -s -C Compiler PLUGIN_CXX="$(PLUGIN_CXX)" print-plugin)
GCC16_PLUGIN_BINARY = $(shell $(MAKE) --no-print-directory -s -C Compiler PLUGIN_CXX="$(GCC16_CXX)" print-plugin)
PLUGIN_LOAD_FLAG = -fplugin=$(PLUGIN_BINARY)
