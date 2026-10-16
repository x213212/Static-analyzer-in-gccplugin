.PHONY: all plugin gcc16-plugin clean help

all: plugin

plugin:
	$(MAKE) -C Compiler PLUGIN_CXX="$(PLUGIN_CXX)"

gcc16-plugin:
	$(MAKE) -C Compiler PLUGIN_CXX="$(GCC16_CXX)"

# Keep recorded measurements and reports; clean only compiler build outputs.
clean:
	$(MAKE) -C Compiler clean

help:
	@echo "make [plugin]          Build the current analyzer .so"
	@echo "make analyze SOURCE=x.c Analyze one source file"
	@echo "make project-report    Merge per-source JSON reports"
	@echo "make test              Run Python and compiler regression tests"
	@echo "make test-gcc16        Run compiler regression tests with GCC16"
	@echo "make check-options     Check current options and removed-option rejection"
	@echo "make clean             Remove compiler build outputs; retain reports"
