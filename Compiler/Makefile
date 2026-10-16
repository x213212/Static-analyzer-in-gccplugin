# The current modular engine; no historical implementation is linked.
.DEFAULT_GOAL := all
.PHONY: clean

# Cleaning needs no installed compiler or plugin development headers.
clean:
	$(RM) -r build newplugin.so

ifneq ($(MAKECMDGOALS),clean)
include toolchain.mk

SOURCES := \
	src/plugin.cc \
	src/analyzer.cc \
	src/analysis_budget.cc \
	src/library_model.cc \
	src/resource_protocol.cc \
	src/framework_lifecycle.cc \
	src/options.cc \
	src/gimple_value.cc \
	src/gimple_flow.cc \
	src/gcc_call_effect.cc \
	src/alias_oracle.cc \
	src/edge_alias.cc \
	src/ownership_call.cc \
	src/ownership_summary.cc \
	src/provider_transport.cc \
	src/trusted_contract.cc \
	src/memory_value.cc \
	src/feasibility.cc \
	src/lifetime_engine.cc \
	src/local_confinement.cc \
	src/cfg_evidence.cc \
	src/report.cc
OBJECTS := $(SOURCES:src/%.cc=$(BUILD_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all clean print-cc print-cxx print-plugin FORCE

all: newplugin.so

$(VERSIONED_PLUGIN): $(OBJECTS) plugin.map
	$(PLUGIN_CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: src/%.cc
	@mkdir -p $(@D)
	$(PLUGIN_CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Publish the selected build while isolating objects and binaries by GCC
# toolchain.  FORCE makes switching PLUGIN_CXX select the correct ABI even if
# another version was built more recently.
newplugin.so: $(VERSIONED_PLUGIN) FORCE
	cp $(VERSIONED_PLUGIN) $@

FORCE:

print-cc:
	@echo $(PLUGIN_CC)

print-cxx:
	@echo $(PLUGIN_CXX)

print-plugin:
	@echo $(abspath $(VERSIONED_PLUGIN))

-include $(DEPS)

endif
