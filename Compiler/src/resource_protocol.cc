#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-cfg.h"
#include "dominance.h"
#include "function.h"
#include "stringpool.h"
#include "dumpfile.h"
#include "static_analyzer/analysis_budget.h"
#include "static_analyzer/feasibility.h"
#include "static_analyzer/library_model.h"
#include "static_analyzer/report.h"
#include "static_analyzer/resource_protocol.h"

#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace static_analyzer {
namespace resource_protocol {
namespace {
bool charge(unsigned long n = 1) {
  return analysis_budget::consume(analysis_budget::WorkKind::lifetime_cfg, n);
}
tree peel(tree t) {
  for (unsigned n = 0; t != NULL_TREE && n < 16; ++n) {
    if (CONVERT_EXPR_P(t)) { t = TREE_OPERAND(t, 0); continue; }
    if (TREE_CODE(t) != SSA_NAME) break;
    gimple *d = SSA_NAME_DEF_STMT(t);
    if (d == nullptr || !is_gimple_assign(d) || !gimple_assign_single_p(d)) break;
    tree r = gimple_assign_rhs1(d);
    if (TREE_CODE(r) != ADDR_EXPR && TREE_CODE(r) != SSA_NAME) break;
    t = r;
  }
  return t;
}
tree local_cell(tree t) {
  t = peel(t);
  if (t == NULL_TREE || TREE_CODE(t) != ADDR_EXPR) return NULL_TREE;
  t = TREE_OPERAND(t, 0);
  return TREE_CODE(t) == VAR_DECL && !TREE_STATIC(t) && !DECL_EXTERNAL(t) &&
         DECL_CONTEXT(t) == cfun->decl && POINTER_TYPE_P(TREE_TYPE(t)) &&
         !TREE_THIS_VOLATILE(t) ? t : NULL_TREE;
}
bool references(tree t, tree cell, unsigned depth = 0) {
  if (t == NULL_TREE || depth >= 20 || !charge()) return false;
  if (t == cell) return true;
  if (TREE_CODE(t) == SSA_NAME) {
    gimple *d = SSA_NAME_DEF_STMT(t);
    if (d != nullptr && is_gimple_assign(d))
      for (unsigned i = 1; i < gimple_num_ops(d); ++i)
        if (references(gimple_op(d, i), cell, depth + 1)) return true;
    if (d != nullptr && gimple_code(d) == GIMPLE_PHI)
      for (unsigned i = 0; i < gimple_phi_num_args(d); ++i)
        if (references(gimple_phi_arg_def(d, i), cell, depth + 1)) return true;
    return false;
  }
  if (EXPR_P(t))
    for (int i = 0; i < TREE_OPERAND_LENGTH(t); ++i)
      if (references(TREE_OPERAND(t, i), cell, depth + 1)) return true;
  return false;
}
bool null_initialized(tree cell, gimple *start) {
  basic_block bb;
  FOR_EACH_BB_FN(bb, cfun)
    for (gimple_stmt_iterator i = gsi_start_bb(bb); !gsi_end_p(i); gsi_next(&i)) {
      gimple *s = gsi_stmt(i);
      if (!charge()) return false;
      if (s == start) break;
      if (!is_gimple_assign(s) || !gimple_assign_single_p(s) ||
          gimple_assign_lhs(s) != cell ||
          TREE_CODE(gimple_assign_rhs1(s)) != INTEGER_CST ||
          !integer_zerop(gimple_assign_rhs1(s))) continue;
      if (bb == gimple_bb(start) ||
          dominated_by_p(CDI_DOMINATORS, gimple_bb(start), bb)) return true;
    }
  return false;
}
bool successful_creator(gimple *s, tree cell, bool dictionary) {
  const auto e = library_model::effect(s);
  if ((e.kind != library_model::Effect::output_handles &&
       e.kind != library_model::Effect::dictionary_insert) ||
      e.dictionary != dictionary ||
      local_cell(gimple_call_arg(s, e.slot_argument)) != cell) return false;
  if (dictionary) {
    tree flags = gimple_call_arg(s, 3), value = gimple_call_arg(s, 2);
    // Restrict the initial dictionary contract to ordinary insertion. Deletion,
    // ownership-taking flags and custom key/value semantics remain unknown.
    if (TREE_CODE(flags) != INTEGER_CST || !integer_zerop(flags) ||
        TREE_CODE(peel(value)) != ADDR_EXPR ||
        TREE_CODE(peel(gimple_call_arg(s, 1))) != ADDR_EXPR) return false;
  }
  return true;
}
bool zero_test(tree_code code, HOST_WIDE_INT a, HOST_WIDE_INT b, bool *value) {
  switch (code) {
    case EQ_EXPR: *value = a == b; break;
    case NE_EXPR: *value = a != b; break;
    case LT_EXPR: *value = a < b; break;
    case LE_EXPR: *value = a <= b; break;
    case GT_EXPR: *value = a > b; break;
    case GE_EXPR: *value = a >= b; break;
    default: return false;
  }
  return true;
}
bool known_condition(gimple *condition, const std::set<tree> &nonnull,
                     const std::set<tree> &success, bool *value) {
  if (condition == nullptr || gimple_code(condition) != GIMPLE_COND) return false;
  tree left = gimple_cond_lhs(condition), right = gimple_cond_rhs(condition);
  tree_code code = gimple_cond_code(condition);
  if (TREE_CODE(right) == INTEGER_CST && integer_zerop(right)) {
    if (success.count(left)) return zero_test(code, 0, 0, value);
    if (nonnull.count(left) && (code == EQ_EXPR || code == NE_EXPR)) {
      *value = code == NE_EXPR; return true;
    }
  }
  return false;
}
struct WalkState {
  basic_block block = nullptr;
  bool first = false;
  std::set<tree> nonnull, success;
};
struct ExitWitness {
  gimple *exit = nullptr;
  std::vector<edge> excluded;
  bool exhausted = false;
};
ExitWitness find_exit(gimple *start, tree cell, bool dictionary,
                     const std::vector<gimple *> &barriers) {
  ExitWitness result;
  std::deque<WalkState> pending;
  WalkState first; first.block = gimple_bb(start); first.first = true;
  pending.push_back(first);
  std::set<std::pair<int, bool>> seen;
  unsigned states = 0;
  while (!pending.empty()) {
    WalkState current = pending.front(); pending.pop_front();
    if (!charge() || ++states > 512) { result.exhausted = true; return result; }
    if (!seen.insert({current.block->index, current.first}).second) continue;
    bool active = !current.first, blocked = false;
    gimple *last = nullptr;
    for (gimple_stmt_iterator i = gsi_start_bb(current.block); !gsi_end_p(i); gsi_next(&i)) {
      gimple *s = gsi_stmt(i);
      if (!charge()) { result.exhausted = true; return result; }
      if (!active) { if (s != start) continue; active = true; }
      last = s;
      if (std::find(barriers.begin(), barriers.end(), s) != barriers.end()) {
        blocked = true; break;
      }
      if (gimple_code(s) == GIMPLE_RETURN) { result.exit = s; return result; }
      if (successful_creator(s, cell, dictionary)) {
        tree lhs = gimple_call_lhs(s);
        if (lhs != NULL_TREE) current.success.insert(lhs);
      }
      if (is_gimple_assign(s)) {
        tree lhs = gimple_assign_lhs(s), rhs = gimple_assign_rhs1(s);
        if (TREE_CODE(lhs) == SSA_NAME && gimple_assign_single_p(s)) {
          if (rhs == cell || current.nonnull.count(rhs)) current.nonnull.insert(lhs);
          if (current.success.count(rhs)) current.success.insert(lhs);
        }
      }
    }
    if (blocked) continue;
    bool truth = false;
    const bool known = known_condition(last, current.nonnull, current.success, &truth);
    edge e; edge_iterator ei;
    FOR_EACH_EDGE(e, ei, current.block->succs) {
      if ((e->flags & (EDGE_ABNORMAL | EDGE_EH | EDGE_FAKE)) != 0) continue;
      const bool wrong = known && ((truth && (e->flags & EDGE_FALSE_VALUE)) ||
                                  (!truth && (e->flags & EDGE_TRUE_VALUE)));
      if (wrong) { result.excluded.push_back(e); continue; }
      if (e->dest == EXIT_BLOCK_PTR_FOR_FN(cfun)) continue;
      WalkState next = current; next.block = e->dest; next.first = false;
      if (pending.size() >= 512) { result.exhausted = true; return result; }
      pending.push_back(std::move(next));
    }
  }
  return result;
}
}  // namespace

void analyze_current_function() {
  if (!library_model::ffmpeg_enabled() || cfun == nullptr || cfun->cfg == nullptr ||
      !analysis_budget::available()) return;
  std::vector<gimple *> statements;
  std::map<tree, std::vector<gimple *>> creators;
  basic_block bb;
  FOR_EACH_BB_FN(bb, cfun)
    for (gimple_stmt_iterator i = gsi_start_bb(bb); !gsi_end_p(i); gsi_next(&i)) {
      if (!charge() || statements.size() >= 4096) return;
      gimple *s = gsi_stmt(i); statements.push_back(s);
      const auto e = library_model::effect(s);
      if (e.kind == library_model::Effect::output_handles ||
          e.kind == library_model::Effect::dictionary_insert) {
        tree cell = local_cell(gimple_call_arg(s, e.slot_argument));
        if (cell != NULL_TREE && successful_creator(s, cell, e.dictionary)) creators[cell].push_back(s);
      }
    }
  if (creators.empty() || creators.size() > 16) return;
  calculate_dominance_info(CDI_DOMINATORS);
  for (const auto &item : creators) {
    tree cell = item.first;
    const bool dictionary = library_model::effect(item.second.front()).dictionary;
    for (gimple *start : item.second) {
      if (!analysis_budget::available()) return;
      if (!null_initialized(cell, start)) continue;
      std::vector<gimple *> barriers;
      for (gimple *s : statements) {
        if (s == start || (is_gimple_assign(s) && gimple_clobber_p(s))) continue;
        if (gimple_code(s) == GIMPLE_ASM ||
            (gimple_code(s) == GIMPLE_RETURN &&
             references(gimple_return_retval(as_a<greturn *>(s)), cell))) {
          barriers.push_back(s); continue;
        }
        if (is_gimple_assign(s) && gimple_assign_lhs(s) == cell) {
          barriers.push_back(s); continue;
        }
        if (is_gimple_assign(s) && TREE_CODE(gimple_assign_lhs(s)) != SSA_NAME) {
          bool escapes = false;
          for (unsigned i = 1; i < gimple_num_ops(s); ++i)
            escapes = escapes || references(gimple_op(s, i), cell);
          if (escapes) { barriers.push_back(s); continue; }
        }
        if (!is_gimple_call(s)) continue;
        if ((gimple_call_flags(s) & ECF_RETURNS_TWICE) != 0) {
          barriers.push_back(s); continue;
        }
        if (successful_creator(s, cell, dictionary) && dictionary) continue;
        for (unsigned a = 0; a < gimple_call_num_args(s); ++a)
          if (references(gimple_call_arg(s, a), cell)) {
            // Complete SDK releases and unknown consumers both block a leak
            // witness. A missing effect is never evidence of non-release.
            barriers.push_back(s); break;
          }
      }
      const ExitWitness witness = find_exit(start, cell, dictionary, barriers);
      if (witness.exhausted) {
        report::record_inconclusive(report::FindingKind::memory_leak,
            gimple_location(start), "resource-protocol-budget");
        break;
      }
      if (witness.exit == nullptr) continue;
      const auto path = feasibility::path_to_exit_avoiding(start, barriers, witness.excluded);
      feasibility::dump_result(path, "library-output-resource");
      if (path.verdict == feasibility::Verdict::infeasible) continue;
      if (!analysis_budget::available()) return;
      std::vector<report::TracePoint> trace{
        {gimple_location(start), dictionary ?
          "FFmpeg contract: successful nonnull dictionary insertion creates caller-owned storage" :
          "FFmpeg contract: successful output-parameter call transfers caller-owned handles"},
        {gimple_location(witness.exit), "normal exit with no modeled release; path needs review if UNKNOWN"}};
      report::emit_model_review(report::FindingKind::memory_leak, gimple_location(start),
          std::string("ffmpeg-sdk-v1: output-parameter ownership; local cell ") +
          (DECL_NAME(cell) == NULL_TREE ? "(unnamed)" : IDENTIFIER_POINTER(DECL_NAME(cell))) +
          "; valid API inputs; no hidden interposition",
          feasibility::verdict_name(path.verdict), trace);
      break;
    }
  }
}
}  // namespace resource_protocol
}  // namespace static_analyzer
