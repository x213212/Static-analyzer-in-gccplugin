PLUGIN_CXX ?= $(shell \
	for candidate in $(CXX) g++-16 g++ g++-15 g++-14 g++-13 g++-12 g++-11 g++-10 g++-9; do \
		plugin_dir=`$$candidate -print-file-name=plugin 2>/dev/null`; \
		if test -f "$$plugin_dir/include/gcc-plugin.h"; then echo $$candidate; break; fi; \
	done)

ifeq ($(strip $(PLUGIN_CXX)),)
$(error No G++ installation with gcc-plugin.h was found; install gcc-<version>-plugin-dev or set PLUGIN_CXX)
endif

PLUGIN_CC ?= $(subst g++,gcc,$(PLUGIN_CXX))
GCC_PLUGIN_DIR := $(shell $(PLUGIN_CXX) -print-file-name=plugin)
PLUGIN_VERSION := $(shell $(PLUGIN_CXX) -dumpfullversion -dumpversion)
PLUGIN_TARGET := $(shell $(PLUGIN_CXX) -dumpmachine)
# Z3 is an optional, bounded path-feasibility refinement.  Include the feature
# bit in the object directory so toggling solver availability cannot reuse
# objects compiled with a different preprocessor configuration.
Z3_AVAILABLE := $(shell pkg-config --exists z3 2>/dev/null && echo 1 || echo 0)
PLUGIN_OPT_LEVEL ?= 2
TOOLCHAIN_ID := $(notdir $(PLUGIN_CXX))-$(PLUGIN_VERSION)-$(PLUGIN_TARGET)-z3$(Z3_AVAILABLE)-O$(PLUGIN_OPT_LEVEL)-vis1
BUILD_DIR := build/$(TOOLCHAIN_ID)
VERSIONED_PLUGIN := $(BUILD_DIR)/newplugin.so
PLUGIN_STDCXX := $(realpath $(shell $(PLUGIN_CXX) -print-file-name=libstdc++.so))
PLUGIN_RUNTIME_DIR := $(dir $(PLUGIN_STDCXX))

CPPFLAGS += -I$(GCC_PLUGIN_DIR)/include -Iinclude -I.
# The plugin executes in every compiler process, so build the production
# shared object with optimization.  Keep symbols for crash diagnosis; callers
# can select another level explicitly with PLUGIN_OPT_LEVEL (for example 0 when
# comparing cache implementations under a debugger).
CXXFLAGS += -std=gnu++14 -fPIC -O$(PLUGIN_OPT_LEVEL) -g -fno-rtti -MMD -MP \
	-fvisibility=hidden -fvisibility-inlines-hidden \
	-Wall -Wextra -Wpedantic -Werror=return-type -Wno-unused-parameter \
	-Wno-deprecated-declarations
# A plugin is dlopen'ed by cc1/cc1plus.  In a side-by-side toolchain (for
# example the local GCC 16.2 benchmark compiler), the executable's RUNPATH is
# not reliably used to resolve dependencies of the loaded plugin.  Record the
# C++ runtime selected by the compiler that built this ABI-specific plugin.
LDFLAGS += -shared -Wl,-rpath,$(PLUGIN_RUNTIME_DIR) \
	-Wl,--version-script,$(CURDIR)/plugin.map

# Without the Z3 development package, compilation remains supported and the
# CFG layer still rejects impossible candidates.  Proof-only path-dependent
# candidates that need SMT stay INCONCLUSIVE; may-reachability is never
# promoted into a finding.
ifeq ($(Z3_AVAILABLE),1)
CPPFLAGS += -DSTATIC_ANALYZER_HAVE_Z3=1 $(shell pkg-config --cflags z3)
LDLIBS += $(shell pkg-config --libs z3)
endif
