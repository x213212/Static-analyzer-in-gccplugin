#include "gcc-plugin.h"
#include "tree.h"
#include "tree-pass.h"
#include "tree-core.h"
#include "print-tree.h"
#include "tree-ssa-alias.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-inline.h"
#include "cgraph.h"
#include "gimple-walk.h"
#include "gimple-pretty-print.h"
#include "gimple-ssa.h"
#include "context.h"
#include "tree-dfa.h"
#include "insn-constants.h"
#include "options.h"
#include "backend.h"
#include "rtl.h"
#include "alloc-pool.h"
#include "ssa.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "stmt.h"
#include "ssa-iterators.h"
#include "attribs.h"
#include "hash-map.h"
#include "gimple-predict.h"
#include "cfgloop.h"
#include "tree-cfg.h"
#include "pretty-print.h"
#include "dumpfile.h"

#include <fstream>
#include <iostream>
#include <list>
#include <math.h>
#include <set>
#include <stack>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "static_analyzer/alias_oracle.h"
#include "static_analyzer/cfg_evidence.h"
#include "static_analyzer/gimple_flow.h"
#include "static_analyzer/legacy_adapter.h"
#include "static_analyzer/report.h"

using namespace std;

// These historical headers contain implementations and shared process-global
// state.  They deliberately live in this translation unit only; none of their
// symbols are part of the modern analyzer API.
#include "../dfs.h"
#include "../define.h"
#include "../unit.h"
#include "../debug.h"
#include "../gimplearray.h"
#include "../getinfo.h"
#include "../trace.h"
#include "../collect.h"
#include "../mapping.h"
#include "../performance.h"
#include "../new_imm_use.h"
#include "../check.h"
#include "../analyze.h"
#include "../Algorithm.h"

// Algorithm.h calls this historical global symbol.  Delegate the actual SSA
// traversal to the bounded modern immediate-use walker while preserving that
// exact linkage and signature.
void collect_builtin_imm_uses(gimple_array *table, tree root) {
  const static_analyzer::UseExpansion expansion =
      static_analyzer::expand_immediate_uses(root);
  static_analyzer::dump_use_expansion(root, expansion);
  for (const static_analyzer::UseSite &site : expansion.sites) {
    tree reached_value = site.incoming_value;
    if (reached_value == NULL_TREE)
      reached_value = root;
    set_gimple_array(table, site.statement, reached_value, reached_value,
                     nullptr);
  }
}

namespace static_analyzer {
namespace legacy_adapter {
namespace {

void apply_options(const PluginOptions &options) {
  relatemod = options.relate;
  retnotheapobjskipcheck = options.skip_non_heap_returns;
  traceallfucntion = options.trace_all_functions;
  prebranchexit = options.stop_at_branch_exit;
  debugoutput = options.debug_output;
  debugmod = options.debug;
  vscode_extensionmod = options.vscode_extension;
  contextsentive = options.context_sensitive;
  memoryleakcheck = options.memory_leak;
  useafterfree = options.use_after_free;
  threadmod = options.thread;
}

}  // namespace

void run(const PluginOptions &options) {
  apply_options(options);
  detect();
}

}  // namespace legacy_adapter
}  // namespace static_analyzer
