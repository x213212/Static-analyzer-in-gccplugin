.PHONY: smoke memory-cache-smoke alias-cache-smoke path-scaling-smoke feasibility-budget-smoke gimple-value-smoke analysis-budget-smoke analysis-budget-cpu-smoke

smoke: plugin
	$(RM) -r $(SMOKE_DIR)
	mkdir -p $(SMOKE_DIR)
	cd $(SMOKE_DIR) && $(PLUGIN_CC) \
		$(PLUGIN_LOAD_FLAG) \
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
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		$(ANALYSIS_CFLAGS) \
		$(PRESERVE_LIFETIMES_CFLAGS) \
		-fdump-ipa-all-details \
		-c $(abspath tests/cases/memory_ssa/load_then_use_after_free.c) \
		-o case.o 2> compiler.log
	@dump_file=`find $(MEMORY_CACHE_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		test "`grep -c '^memory-value:' "$$dump_file"`" -eq 2; \
		grep -q '^memory-value-cache summary: requests=2 evaluations=1 hits=1 entries=1$$' "$$dump_file"; \
		echo "memory cache smoke dump: $$dump_file"

# Two allocations exercise same-value and symmetric no-alias queries.
# Check actual query records against cache counters, without fixing the
# number of requests made by the surrounding lifetime checks.
alias-cache-smoke: plugin
	$(RM) -r $(ALIAS_CACHE_SMOKE_DIR)
	mkdir -p $(ALIAS_CACHE_SMOKE_DIR)
	cd $(ALIAS_CACHE_SMOKE_DIR) && $(PLUGIN_CC) \
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		$(ANALYSIS_CFLAGS) \
		$(PRESERVE_LIFETIMES_CFLAGS) \
		-fdump-ipa-all-details \
		-c $(abspath tests/cases/use_after_free/different_object.c) \
		-o case.o 2> compiler.log
	@dump_file=`find $(ALIAS_CACHE_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		$(PYTHON) tests/check_alias_cache.py "$$dump_file"; \
		echo "alias cache smoke dump: $$dump_file"

# The safe fixture has 2^30 syntactic combinations between a guarded release
# and an incompatible guarded use.  Exact neutral-diamond merging must prove
# the candidate UNSAT with a near-linear state count; the feasible twin keeps
# the same diamonds and must still report the real UAF.
path-scaling-smoke: plugin
	$(RM) -r $(PATH_SCALING_SMOKE_DIR)
	mkdir -p $(PATH_SCALING_SMOKE_DIR)/unsat $(PATH_SCALING_SMOKE_DIR)/feasible
	cd $(PATH_SCALING_SMOKE_DIR)/unsat && $(PLUGIN_CC) \
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-smt=1 \
		$(ANALYSIS_CFLAGS) \
		$(PRESERVE_LIFETIMES_CFLAGS) \
		-fdump-ipa-all-details \
		-c $(abspath tests/cases/path_scaling/thirty_diamonds.c) \
		-o case.o 2> compiler.log
	cd $(PATH_SCALING_SMOKE_DIR)/feasible && $(PLUGIN_CC) \
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-smt=1 \
		$(ANALYSIS_CFLAGS) \
		$(PRESERVE_LIFETIMES_CFLAGS) \
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
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-reportpath=$(abspath $(FEASIBILITY_BUDGET_SMOKE_DIR))/analysis \
		-fplugin-arg-newplugin-memoryleakcheck=0 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-smt=1 \
		$(ANALYSIS_CFLAGS) \
		$(PRESERVE_LIFETIMES_CFLAGS) \
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
		$(PYTHON) -c 'import json,sys; d=json.load(open(sys.argv[1])); assert d["summary"]["SA003"] == 0; assert d["summary"]["inconclusive_total"] > 0; assert d["inconclusive_summary"]["by_rule"]["SA003"] > 0; assert d["inconclusive_summary"]["by_reason"]["path-feasibility-budget"] > 0; assert not d["findings"]' $(FEASIBILITY_BUDGET_SMOKE_DIR)/analysis.json; \
		echo "feasibility budget smoke: $$(grep '^path-feasibility-summary:' "$$dump_file")"

# Exercise a shared 30-level PHI DAG and a separate 131-origin PHI.  The
# former must use both caches; the latter must become incomplete at the
# origin bound instead of being promoted to must-alias.
gimple-value-smoke: plugin
	$(RM) -r $(GIMPLE_VALUE_SMOKE_DIR)
	mkdir -p $(GIMPLE_VALUE_SMOKE_DIR)
	cd $(GIMPLE_VALUE_SMOKE_DIR) && timeout 15s $(PLUGIN_CC) \
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-report=0 \
		-fplugin-arg-newplugin-memoryleakcheck=1 \
		-fplugin-arg-newplugin-useafterfree=1 \
		$(ANALYSIS_CFLAGS) \
		$(PRESERVE_LIFETIMES_CFLAGS) \
		-fdump-ipa-all-details \
		-c $(abspath tests/fixtures/gimple_value_dag_stress.c) \
		-o case.o 2> compiler.log
	@dump_file=`find $(GIMPLE_VALUE_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		grep -Eq '^gimple-value-cache summary: requests=[1-9][0-9]* evaluations=[1-9][0-9]* hits=[1-9][0-9]* .*memo-hits=[1-9][0-9]* .*budget-exhaustions=0 ' "$$dump_file"; \
		grep -Eq '^gimple-value-cache summary: .*budget-exhaustions=[1-9][0-9]* .*origin-cap-exhaustions=[1-9][0-9]* ' "$$dump_file"; \
		grep -q 'complete=no' "$$dump_file"; \
		grep -Eq '^[[:space:]]+unknown:' "$$dump_file"; \
		echo "gimple value smoke: bounded memo and origin cap verified"

# Force an analyzer-wide function budget on a generated TU.  Depending on
# host speed, either the process-CPU cap or the deterministic work cap may win;
# both must fail closed with a machine-readable coverage-gap record.
analysis-budget-smoke: plugin
	$(RM) -r $(ANALYSIS_BUDGET_SMOKE_DIR)
	mkdir -p $(ANALYSIS_BUDGET_SMOKE_DIR)
	cd $(ANALYSIS_BUDGET_SMOKE_DIR) && timeout 15s $(PLUGIN_CC) \
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-analysispolicy=proof-only \
		-fplugin-arg-newplugin-memoryleakcheck=1 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-reportpath=$(abspath $(ANALYSIS_BUDGET_SMOKE_DIR))/analysis \
		$(ANALYSIS_CFLAGS) \
		$(PRESERVE_LIFETIMES_CFLAGS) \
		-fdump-ipa-all-details \
		-c $(abspath tests/fixtures/analysis_budget_stress.c) \
		-o case.o 2> compiler.log
	@set -e; \
		dump_file=`find $(ANALYSIS_BUDGET_SMOKE_DIR) -name '*.static_analyzer*' -print -quit`; \
		test -n "$$dump_file"; \
		grep -Eq '^analysis-budget summary: .*reason=function-(cpu-time|work) .*cpu-clock=process .*limits-tu-cpu-ms=2000 function-cpu-ms=100 function-work=250000$$' "$$dump_file"; \
		grep -Eq '^analysis-budget work: .*(function-cpu-time|function-work)=[1-9][0-9]* ' "$$dump_file"; \
		test "`grep -Ec '\[SA00[1-4]\]' $(ANALYSIS_BUDGET_SMOKE_DIR)/compiler.log`" -eq 0; \
		$(PYTHON) -c 'import json,sys; d=json.load(open(sys.argv[1])); assert d["summary"]["total"] == 0; assert d["summary"]["inconclusive_total"] > 0; assert any(k.startswith("analysis-budget-") for k in d["inconclusive_summary"]["by_reason"])' $(ANALYSIS_BUDGET_SMOKE_DIR)/analysis.json; \
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
