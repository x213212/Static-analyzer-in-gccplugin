.PHONY: analyze project-report

# Compile one translation unit with all four rules enabled and emit
# analysis.html + analysis.json alongside the requested object file.
# Usage: make analyze SOURCE=/absolute/or/relative/file.c
analyze: plugin
	@test -n "$(SOURCE)" || { echo "error: set SOURCE, for example: make analyze SOURCE=example.c" >&2; exit 2; }
	@test -f "$(SOURCE)" || { echo "error: source file not found: $(SOURCE)" >&2; exit 2; }
	mkdir -p $(ANALYZE_DIR)
	$(PLUGIN_CC) \
		$(PLUGIN_LOAD_FLAG) \
		-fplugin-arg-newplugin-memoryleakcheck=1 \
		-fplugin-arg-newplugin-useafterfree=1 \
		-fplugin-arg-newplugin-threadmod=1 \
		-fplugin-arg-newplugin-reportpath=$(abspath $(ANALYZE_DIR))/analysis \
		$(ANALYSIS_CFLAGS) -pthread \
		-c $(abspath $(SOURCE)) -o $(ANALYZE_DIR)/analysis.o
	@echo "HTML: $(abspath $(ANALYZE_DIR))/analysis.html"
	@echo "JSON: $(abspath $(ANALYZE_DIR))/analysis.json"

# Merge the collision-resistant per-translation-unit reports written by
# tools/static_analyzer_cc.py into one deterministic project dashboard.
# PROJECT_TU_REPORT_DIR and PROJECT_REPORT_DIR may be overridden by callers.
project-report:
	mkdir -p "$(PROJECT_TU_REPORT_DIR)" "$(PROJECT_REPORT_DIR)"
	$(PYTHON) tools/merge_static_analyzer_reports.py \
		"$(PROJECT_TU_REPORT_DIR)" \
		--json "$(PROJECT_REPORT_DIR)/report.json" \
		--html "$(PROJECT_REPORT_DIR)/report.html" --strict
	@echo "Project HTML: $(abspath $(PROJECT_REPORT_DIR))/report.html"
	@echo "Project JSON: $(abspath $(PROJECT_REPORT_DIR))/report.json"
