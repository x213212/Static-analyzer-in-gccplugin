.PHONY: benchmarks-extracted benchmarks-all compare-gcc16 compare-gcc16-extracted compare-gcc16-all compare-gcc16-juliet compare-gcc16-juliet-timing compare-gcc16-juliet-cxx

benchmarks-extracted: plugin
	$(PYTHON) tests/run_benchmarks.py --suite extracted \
		--cc $(PLUGIN_CC) --cxx $(PLUGIN_CXX) \
		--plugin "$(PLUGIN_BINARY)" --output $(BENCHMARK_DIR)/extracted

benchmarks-all: plugin
	$(PYTHON) tests/run_benchmarks.py --suite all \
		--cc $(PLUGIN_CC) --cxx $(PLUGIN_CXX) \
		--plugin "$(PLUGIN_BINARY)" --output $(BENCHMARK_DIR)/all

compare-gcc16: gcc16-plugin
	$(PYTHON) tests/compare_gcc_analyzer.py \
		--plugin-compiler $(GCC16_CC) \
		--plugin-cxx $(GCC16_CXX) \
		--plugin "$(GCC16_PLUGIN_BINARY)" \
		--gcc16 $(GCC16_CC) --gcc16-cxx $(GCC16_CXX) --require-gcc16 \
		--manifest tests/manifest.json \
		--output $(GCC16_COMPARE_DIR)

compare-gcc16-extracted: gcc16-plugin
	$(PYTHON) tests/compare_benchmark_analyzers.py \
		--suite extracted --cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin "$(GCC16_PLUGIN_BINARY)" \
		--output $(GCC16_COMPARE_DIR)-corpus-extracted

compare-gcc16-all: gcc16-plugin
	$(PYTHON) tests/compare_benchmark_analyzers.py \
		--suite all --cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin "$(GCC16_PLUGIN_BINARY)" \
		--output $(GCC16_COMPARE_DIR)-corpus-all

# Ground-truth C accuracy comparison.  Juliet testcase groups are compiled as
# bad (-DOMITGOOD) and good (-DOMITBAD) variants; multi-TU cases stay grouped.
# Parallel execution is appropriate for accuracy/coverage, but its per-process
# timings must not be used as publication-quality speed evidence.
compare-gcc16-juliet: gcc16-plugin
	$(PYTHON) tests/compare_juliet_oracle.py \
		--cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin "$(GCC16_PLUGIN_BINARY)" \
		--language c --jobs $(JULIET_ACCURACY_JOBS) \
		--output $(GCC16_COMPARE_DIR)-juliet-oracle

# Serial timing run over the same C oracle cohort.  Repeat with
# JULIET_TIMING_RUN=1,2,3 on an otherwise idle host and compare the three
# generated reports; one run alone is not a release-quality speed claim.
compare-gcc16-juliet-timing: gcc16-plugin
	$(PYTHON) tests/compare_juliet_oracle.py \
		--cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin "$(GCC16_PLUGIN_BINARY)" \
		--language c --jobs 1 \
		--output $(GCC16_COMPARE_DIR)-juliet-timing-run-$(JULIET_TIMING_RUN)

# GCC's native analyzer documents C as its primary comparison surface.  Keep
# the C++ run visibly experimental instead of folding it into the C headline.
compare-gcc16-juliet-cxx: gcc16-plugin
	$(PYTHON) tests/compare_juliet_oracle.py \
		--cc $(GCC16_CC) --cxx $(GCC16_CXX) \
		--plugin "$(GCC16_PLUGIN_BINARY)" \
		--language c++ --jobs $(JULIET_ACCURACY_JOBS) \
		--output $(GCC16_COMPARE_DIR)-juliet-cxx-experimental
