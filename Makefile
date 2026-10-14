
MAKE := make
PLUGIN_CXX := $(shell $(MAKE) --no-print-directory -s -C Compiler print-cxx)
PLUGIN_CC := $(subst g++,gcc,$(PLUGIN_CXX))
# Prefer the side-by-side GCC 16.2 toolchain used by the reproducible
# comparison.  Callers can still override GCC16_PREFIX, GCC16_CC, or
# GCC16_CXX; the comparison runners independently reject versions other than
# 16.2.x.
GCC16_PREFIX ?= /root/toolchains/gcc-16.2
GCC16_CC ?= $(if $(wildcard $(GCC16_PREFIX)/bin/gcc),$(GCC16_PREFIX)/bin/gcc,gcc-16)
GCC16_CXX ?= $(if $(wildcard $(GCC16_PREFIX)/bin/g++),$(GCC16_PREFIX)/bin/g++,g++-16)
ARTIFACT_DIR := artifacts
SMOKE_DIR := $(ARTIFACT_DIR)/smoke
MEMORY_CACHE_SMOKE_DIR := $(ARTIFACT_DIR)/memory-cache-smoke
ALIAS_CACHE_SMOKE_DIR := $(ARTIFACT_DIR)/alias-cache-smoke
PATH_SCALING_SMOKE_DIR := $(ARTIFACT_DIR)/path-scaling-smoke
FEASIBILITY_BUDGET_SMOKE_DIR := $(ARTIFACT_DIR)/feasibility-budget-smoke
GIMPLE_VALUE_SMOKE_DIR := $(ARTIFACT_DIR)/gimple-value-smoke
ANALYSIS_BUDGET_SMOKE_DIR := $(ARTIFACT_DIR)/analysis-budget-smoke
ANALYSIS_BUDGET_CPU_SMOKE_DIR := $(ARTIFACT_DIR)/analysis-budget-cpu-smoke
TEST_DIR := $(ARTIFACT_DIR)/tests
BENCHMARK_DIR := $(ARTIFACT_DIR)/benchmarks
GCC16_COMPARE_DIR := $(ARTIFACT_DIR)/gcc16-comparison
ANALYZE_DIR := $(ARTIFACT_DIR)/analyze
PROJECT_TU_REPORT_DIR ?= $(ARTIFACT_DIR)/project-tu-reports
PROJECT_REPORT_DIR ?= $(ARTIFACT_DIR)/project-report
JULIET_ACCURACY_JOBS ?= 8
JULIET_TIMING_RUN ?= 1

.PHONY: all plugin gcc16-plugin test test-python test-gcc16 legacy-test smoke \
	memory-cache-smoke alias-cache-smoke analyze \
	path-scaling-smoke feasibility-budget-smoke gimple-value-smoke \
	analysis-budget-smoke analysis-budget-cpu-smoke project-report \
	benchmarks-extracted benchmarks-all compare-gcc16 \
	compare-gcc16-extracted compare-gcc16-all compare-gcc16-juliet \
	compare-gcc16-juliet-timing compare-gcc16-juliet-cxx clean

all: test

plugin:
	$(MAKE) -C Compiler PLUGIN_CXX=$(PLUGIN_CXX)

gcc16-plugin:
	$(MAKE) -C Compiler PLUGIN_CXX=$(GCC16_CXX)

# The original mixed corpus is intentionally retained as a separate target.
legacy-test: plugin
	$(MAKE) -C Test -f Makefile.legacy CC=$(PLUGIN_CC) test.o

test-python:
	python3 -m unittest discover -s tests -p 'test_*.py' -v
	python3 -m py_compile tests/*.py tools/*.py

test: plugin test-python
	python3 tests/run.py \
		--compiler $(PLUGIN_CC) \
		--plugin Compiler/newplugin.so \
		--manifest tests/manifest.json \
		--output $(TEST_DIR)

test-gcc16: gcc16-plugin
	python3 tests/run.py \
		--compiler $(GCC16_CC) \
		--plugin Compiler/newplugin.so \
		--manifest tests/manifest.json \
		--output $(TEST_DIR)-gcc16

# Compile one translation unit with all four rules enabled and emit
# analysis.html + analysis.json alongside the requested object file.
# Usage: make analyze SOURCE=/absolute/or/relative/file.c
analyze: plugin
	@test -n "$(SOURCE)" || { echo "error: set SOURCE, for example: make analyze SOURCE=example.c" >&2; exit 2; }
	@test -f "$(SOURCE)" || { echo "error: source file not found: $(SOURCE)" >&2; exit 2; }
	mkdir -p $(ANALYZE_DIR)
	$(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-memoryleakcheck=1 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-threadmod=1 \
		-fplugin-arg-newplugin-reportpath=$(abspath $(ANALYZE_DIR))/analysis \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline -pthread \
		-c $(abspath $(SOURCE)) -o $(ANALYZE_DIR)/analysis.o
	@echo "HTML: $(abspath $(ANALYZE_DIR))/analysis.html"
	@echo "JSON: $(abspath $(ANALYZE_DIR))/analysis.json"

# Merge the collision-resistant per-translation-unit reports written by
# tools/static_analyzer_cc.py into one deterministic project dashboard.
# PROJECT_TU_REPORT_DIR and PROJECT_REPORT_DIR may be overridden by callers.
project-report:
	mkdir -p "$(PROJECT_TU_REPORT_DIR)" "$(PROJECT_REPORT_DIR)"
	python3 tools/merge_static_analyzer_reports.py \
		"$(PROJECT_TU_REPORT_DIR)" \
		--json "$(PROJECT_REPORT_DIR)/report.json" \
		--html "$(PROJECT_REPORT_DIR)/report.html" --strict
	@echo "Project HTML: $(abspath $(PROJECT_REPORT_DIR))/report.html"
	@echo "Project JSON: $(abspath $(PROJECT_REPORT_DIR))/report.json"

benchmarks-extracted: plugin
	python3 tests/run_benchmarks.py --suite extracted \
		--cc $(PLUGIN_CC) --cxx $(PLUGIN_CXX) \
		--plugin Compiler/newplugin.so --output $(BENCHMARK_DIR)/extracted

benchmarks-all: plugin
	python3 tests/run_benchmarks.py --suite all \
		--cc $(PLUGIN_CC) --cxx $(PLUGIN_CXX) \
		--plugin Compiler/newplugin.so --output $(BENCHMARK_DIR)/all

compare-gcc16: gcc16-plugin
	python3 tests/compare_gcc_analyzer.py \
		--plugin-compiler $(GCC16_CC) \
		--plugin-cxx $(GCC16_CXX) \
		--plugin Compiler/newplugin.so \
		--gcc16 $(GCC16_CC) --gcc16-cxx $(GCC16_CXX) --require-gcc16 \
		--manifest tests/manifest.json \
		--output $(GCC16_COMPARE_DIR)

compare-gcc16-extracted: gcc16-plugin
	python3 tests/compare_benchmark_analyzers.py \
		--suite extracted --cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin Compiler/newplugin.so \
		--output $(GCC16_COMPARE_DIR)-corpus-extracted

compare-gcc16-all: gcc16-plugin
	python3 tests/compare_benchmark_analyzers.py \
		--suite all --cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin Compiler/newplugin.so \
		--output $(GCC16_COMPARE_DIR)-corpus-all

# Ground-truth C accuracy comparison.  Juliet testcase groups are compiled as
# bad (-DOMITGOOD) and good (-DOMITBAD) variants; multi-TU cases stay grouped.
# Parallel execution is appropriate for accuracy/coverage, but its per-process
# timings must not be used as publication-quality speed evidence.
compare-gcc16-juliet: gcc16-plugin
	python3 tests/compare_juliet_oracle.py \
		--cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin Compiler/newplugin.so \
		--language c --jobs $(JULIET_ACCURACY_JOBS) \
		--output $(GCC16_COMPARE_DIR)-juliet-oracle

# Serial timing run over the same C oracle cohort.  Repeat with
# JULIET_TIMING_RUN=1,2,3 on an otherwise idle host and compare the three
# generated reports; one run alone is not a release-quality speed claim.
compare-gcc16-juliet-timing: gcc16-plugin
	python3 tests/compare_juliet_oracle.py \
		--cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin Compiler/newplugin.so \
		--language c --jobs 1 \
		--output $(GCC16_COMPARE_DIR)-juliet-timing-run-$(JULIET_TIMING_RUN)

# GCC's native analyzer documents C as its primary comparison surface.  Keep
# the C++ run visibly experimental instead of folding it into the C headline.
compare-gcc16-juliet-cxx: gcc16-plugin
	python3 tests/compare_juliet_oracle.py \
		--cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin Compiler/newplugin.so \
		--language c++ --jobs $(JULIET_ACCURACY_JOBS) \
		--output $(GCC16_COMPARE_DIR)-juliet-cxx-experimental

smoke: plugin
	$(RM) -r $(SMOKE_DIR)
	mkdir -p $(SMOKE_DIR)
	cd $(SMOKE_DIR) && $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugmod=0 \
		-fplugin-arg-newplugin-debugoutput=0 \
		-O1 -fipa-pta -fno-inline -fdump-ipa-all-details \
		-c $(abspath tests/fixtures/alias_flow.c) -o alias_flow.o \
		2> compiler.log
	@dump_file=`find $(SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		grep -q 'value-flow:' "$$dump_file"; \
		grep -q 'alias-oracle summary:' "$$dump_file"; \
		echo "smoke dump: $$dump_file"

# Assert that repeated resolution of one pointer load preserves both caller
# dumps while evaluating its Memory-SSA walk only once in the active function.
memory-cache-smoke: plugin
	$(RM) -r $(MEMORY_CACHE_SMOKE_DIR)
	mkdir -p $(MEMORY_CACHE_SMOKE_DIR)
	cd $(MEMORY_CACHE_SMOKE_DIR) && $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline \
		-fno-tree-dce -fno-tree-dse -fno-tree-fre \
		-fdump-ipa-all-details \
		-c $(abspath tests/cases/memory_ssa/load_then_use_after_free.c) \
		-o case.o 2> compiler.log
	@dump_file=`find $(MEMORY_CACHE_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		test "`grep -c '^memory-value:' "$$dump_file"`" -eq 2; \
		grep -q '^memory-value-cache summary: requests=2 evaluations=1 hits=1 entries=1$$' "$$dump_file"; \
		echo "memory cache smoke dump: $$dump_file"

# Two allocations released in source order ask both (first, second) and
# (second, first).  Preserve all four evidence records while evaluating that
# symmetric GCC points-to/provenance pair only once.
alias-cache-smoke: plugin
	$(RM) -r $(ALIAS_CACHE_SMOKE_DIR)
	mkdir -p $(ALIAS_CACHE_SMOKE_DIR)
	cd $(ALIAS_CACHE_SMOKE_DIR) && $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline \
		-fno-tree-dce -fno-tree-dse -fno-tree-fre \
		-fdump-ipa-all-details \
		-c $(abspath tests/cases/use_after_free/different_object.c) \
		-o case.o 2> compiler.log
	@dump_file=`find $(ALIAS_CACHE_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		test "`grep -c '^alias-query:' "$$dump_file"`" -eq 4; \
		grep -q '^alias-query: .* cache=miss$$' "$$dump_file"; \
		grep -q '^alias-query: .* cache=hit$$' "$$dump_file"; \
		grep -q 'alias-oracle summary: .*cache-requests=4 cache-evaluations=3 cache-hits=1 cache-entries=3 cache-cap-skips=0 cache-limit=8192$$' "$$dump_file"; \
		echo "alias cache smoke dump: $$dump_file"

# The safe fixture has 2^30 syntactic combinations between a guarded release
# and an incompatible guarded use.  Exact neutral-diamond merging must prove
# the candidate UNSAT with a near-linear state count; the feasible twin keeps
# the same diamonds and must still report the real UAF.
path-scaling-smoke: plugin
	$(RM) -r $(PATH_SCALING_SMOKE_DIR)
	mkdir -p $(PATH_SCALING_SMOKE_DIR)/unsat $(PATH_SCALING_SMOKE_DIR)/feasible
	cd $(PATH_SCALING_SMOKE_DIR)/unsat && $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-smt=1 \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline \
		-fno-tree-dce -fno-tree-dse -fno-tree-fre \
		-fdump-ipa-all-details \
		-c $(abspath tests/cases/path_scaling/thirty_diamonds.c) \
		-o case.o 2> compiler.log
	cd $(PATH_SCALING_SMOKE_DIR)/feasible && $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-smt=1 \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline \
		-fno-tree-dce -fno-tree-dse -fno-tree-fre \
		-fdump-ipa-all-details \
		-c $(abspath tests/cases/path_scaling/thirty_diamonds_feasible.c) \
		-o case.o 2> compiler.log
	@unsat_dump=`find $(PATH_SCALING_SMOKE_DIR)/unsat -name '*.static_analyzer*' -print -quit`; \
		feasible_dump=`find $(PATH_SCALING_SMOKE_DIR)/feasible -name '*.static_analyzer*' -print -quit`; \
		test -n "$$unsat_dump"; test -n "$$feasible_dump"; \
		unsat_line=`grep '^path-feasibility: purpose=use-after-free verdict=infeasible backend=z3 ' "$$unsat_dump" | head -1`; \
		feasible_line=`grep '^path-feasibility: purpose=use-after-free verdict=feasible backend=z3 ' "$$feasible_dump" | head -1`; \
		test -n "$$unsat_line"; test -n "$$feasible_line"; \
		echo "$$unsat_line" | grep -q 'abstracted-diamonds=30 '; \
		echo "$$unsat_line" | grep -Eq 'symbolic-states=([0-9]|[1-5][0-9]|6[0-4]) '; \
		echo "$$unsat_line" | grep -Eq 'checks=([0-8]) '; \
		echo "$$unsat_line" | grep -q 'truncated=no$$'; \
		echo "$$feasible_line" | grep -q 'abstracted-diamonds=30 '; \
		echo "$$feasible_line" | grep -q 'truncated=no$$'; \
		grep -Eq '^path-feasibility-summary: queries=[1-9]' "$$unsat_dump"; \
		grep -q '\[SA003\]' $(PATH_SCALING_SMOKE_DIR)/feasible/compiler.log; \
		test ! -s $(PATH_SCALING_SMOKE_DIR)/unsat/compiler.log; \
		echo "path scaling smoke: $$unsat_line"; \
		echo "path scaling twin: $$feasible_line"

# Generate more independent proof queries than the TU-wide feasibility cap.
# The compiler must finish promptly and exhausted queries must be UNKNOWN.
# Under the default proof-only policy they are recorded as INCONCLUSIVE,
# never emitted as findings and never misrepresented as INFEASIBLE proofs.
feasibility-budget-smoke: plugin
	$(RM) -r $(FEASIBILITY_BUDGET_SMOKE_DIR)
	mkdir -p $(FEASIBILITY_BUDGET_SMOKE_DIR)
	cd $(FEASIBILITY_BUDGET_SMOKE_DIR) && timeout 15s $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-reportpath=$(abspath $(FEASIBILITY_BUDGET_SMOKE_DIR))/analysis \
		-fplugin-arg-newplugin-memoryleakcheck=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-smt=1 \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline \
		-fno-tree-dce -fno-tree-dse -fno-tree-fre \
		-fdump-ipa-all-details \
		-c $(abspath tests/fixtures/feasibility_tu_budget.c) \
		-o case.o 2> compiler.log
	@dump_file=`find $(FEASIBILITY_BUDGET_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		grep -q '^path-feasibility: .* verdict=unknown .*reason=TU feasibility query cap exhausted .*tu-budget-exhausted=yes truncated=yes$$' "$$dump_file"; \
		test "`grep -c '^path-feasibility: .* verdict=infeasible .*tu-budget-exhausted=yes ' "$$dump_file"`" -eq 0; \
		grep -q '^path-feasibility-summary: .*tu-admitted=256 .*tu-budget-exhaustions=[1-9][0-9]* .*tu-query-cap=[1-9][0-9]* .*tu-query-limit=256 ' "$$dump_file"; \
		test -f $(FEASIBILITY_BUDGET_SMOKE_DIR)/analysis.json; \
		test "`grep -c '\[SA003\]' $(FEASIBILITY_BUDGET_SMOKE_DIR)/compiler.log`" -eq 0; \
		python3 -c 'import json,sys; d=json.load(open(sys.argv[1])); assert d["summary"]["SA003"] == 0; assert d["summary"]["inconclusive_total"] > 0; assert d["inconclusive_summary"]["by_rule"]["SA003"] > 0; assert d["inconclusive_summary"]["by_reason"]["path-feasibility-budget"] > 0; assert not d["findings"]' $(FEASIBILITY_BUDGET_SMOKE_DIR)/analysis.json; \
		echo "feasibility budget smoke: $$(grep '^path-feasibility-summary:' "$$dump_file")"

# Exercise a shared 30-level PHI DAG and a separate 131-origin PHI.  The
# former must use both caches; the latter must become incomplete at the
# origin bound instead of being promoted to must-alias.
gimple-value-smoke: plugin
	$(RM) -r $(GIMPLE_VALUE_SMOKE_DIR)
	mkdir -p $(GIMPLE_VALUE_SMOKE_DIR)
	cd $(GIMPLE_VALUE_SMOKE_DIR) && timeout 15s $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-memoryleakcheck=1 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline \
		-fno-tree-dce -fno-tree-dse -fno-tree-fre \
		-fdump-ipa-all-details \
		-c $(abspath tests/fixtures/gimple_value_dag_stress.c) \
		-o case.o 2> compiler.log
	@dump_file=`find $(GIMPLE_VALUE_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		grep -Eq '^gimple-value-cache summary: requests=[1-9][0-9]* evaluations=[1-9][0-9]* hits=[1-9][0-9]* .*memo-hits=[1-9][0-9]* .*budget-exhaustions=0 ' "$$dump_file"; \
		grep -Eq '^gimple-value-cache summary: .*budget-exhaustions=[1-9][0-9]* .*origin-cap-exhaustions=[1-9][0-9]* ' "$$dump_file"; \
		grep -q 'complete=no' "$$dump_file"; \
		grep -q 'kind=unknown' "$$dump_file"; \
		echo "gimple value smoke: bounded memo and origin cap verified"

# Force an analyzer-wide function budget on a generated TU.  Depending on
# host speed, either the process-CPU cap or the deterministic work cap may win;
# both must fail closed with a machine-readable coverage-gap record.
analysis-budget-smoke: plugin
	$(RM) -r $(ANALYSIS_BUDGET_SMOKE_DIR)
	mkdir -p $(ANALYSIS_BUDGET_SMOKE_DIR)
	cd $(ANALYSIS_BUDGET_SMOKE_DIR) && timeout 15s $(PLUGIN_CC) \
		-fplugin=$(abspath Compiler/newplugin.so) \
		-fplugin-arg-newplugin-debugoutput=0 \
		-fplugin-arg-newplugin-analysispolicy=proof-only \
		-fplugin-arg-newplugin-memoryleakcheck=1 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-reportpath=$(abspath $(ANALYSIS_BUDGET_SMOKE_DIR))/analysis \
		-fdiagnostics-color=never -O1 -fipa-pta -fno-inline \
		-fno-tree-dce -fno-tree-dse -fno-tree-fre \
		-fdump-ipa-all-details \
		-c $(abspath tests/fixtures/analysis_budget_stress.c) \
		-o case.o 2> compiler.log
	@set -e; \
		dump_file=`find $(ANALYSIS_BUDGET_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		grep -Eq '^analysis-budget summary: .*reason=function-(cpu-time|work) .*cpu-clock=process .*limits-tu-cpu-ms=2000 function-cpu-ms=100 function-work=250000$$' "$$dump_file"; \
		grep -Eq '^analysis-budget work: .*(function-cpu-time|function-work)=[1-9][0-9]* ' "$$dump_file"; \
		test "`grep -Ec '\[SA00[1-4]\]' $(ANALYSIS_BUDGET_SMOKE_DIR)/compiler.log`" -eq 0; \
		python3 -c 'import json,sys; d=json.load(open(sys.argv[1])); assert d["summary"]["total"] == 0; assert d["summary"]["inconclusive_total"] > 0; assert any(k.startswith("analysis-budget-") for k in d["inconclusive_summary"]["by_reason"])' $(ANALYSIS_BUDGET_SMOKE_DIR)/analysis.json; \
		echo "analysis budget smoke: fail-closed function CPU/work cap verified"

# Exercise the budget module outside GCC so a deliberate 200ms deschedule is
# deterministic: it must not consume a 50ms process-CPU allowance, while real
# CPU work and the independent deterministic work cap must still exhaust.
# Function/data sections let the standalone link discard the dump-only GCC
# entry point; the production plugin continues to build the same source.
analysis-budget-cpu-smoke:
	$(RM) -r $(ANALYSIS_BUDGET_CPU_SMOKE_DIR)
	mkdir -p $(ANALYSIS_BUDGET_CPU_SMOKE_DIR)
	$(PLUGIN_CXX) -std=gnu++14 -O2 -ffunction-sections -fdata-sections \
		-Wl,--gc-sections -pthread \
		-I`$(PLUGIN_CXX) -print-file-name=plugin`/include \
		-ICompiler/include -ICompiler \
		tests/fixtures/analysis_budget_cpu_time_test.cc \
		Compiler/src/analysis_budget.cc \
		-o $(ANALYSIS_BUDGET_CPU_SMOKE_DIR)/cpu_time_test
	timeout 5s $(ANALYSIS_BUDGET_CPU_SMOKE_DIR)/cpu_time_test
	@echo "analysis budget CPU smoke: deschedule ignored; CPU/work caps verified"

clean:
	$(MAKE) -C Compiler clean
	$(RM) -r $(ARTIFACT_DIR)
