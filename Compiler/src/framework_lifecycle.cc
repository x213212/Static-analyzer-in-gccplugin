#include "gcc-plugin.h"
#include "bversion.h"
#include "tree.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-cfg.h"
#include "cgraph.h"
#include "stringpool.h"
#include "static_analyzer/analysis_budget.h"
#include "static_analyzer/feasibility.h"
#include "static_analyzer/library_model.h"
#include "static_analyzer/report.h"
#include "static_analyzer/resource_protocol.h"

#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace static_analyzer {
namespace resource_protocol {
namespace {
bool charge() { return analysis_budget::consume(analysis_budget::WorkKind::lifetime_cfg); }
bool name_is(tree decl, const char *name) {
  return decl != NULL_TREE && DECL_P(decl) && DECL_NAME(decl) != NULL_TREE &&
         !std::strcmp(IDENTIFIER_POINTER(DECL_NAME(decl)), name);
}
bool record_is(tree type, const char *name) {
  if (type == NULL_TREE || TREE_CODE(type) != RECORD_TYPE) return false;
  tree n = TYPE_NAME(type);
  if (n != NULL_TREE && TREE_CODE(n) == TYPE_DECL) n = DECL_NAME(n);
  return n != NULL_TREE && TREE_CODE(n) == IDENTIFIER_NODE &&
         !std::strcmp(IDENTIFIER_POINTER(n), name);
}
tree object(tree t) {
  if (t != NULL_TREE && TREE_CODE(t) == ADDR_EXPR) t = TREE_OPERAND(t, 0);
  if (t != NULL_TREE && TREE_CODE(t) == ARRAY_REF) t = TREE_OPERAND(t, 0);
  return t;
}
tree member(tree constructor, const char *name) {
  if (constructor == NULL_TREE || TREE_CODE(constructor) != CONSTRUCTOR) return NULL_TREE;
  unsigned HOST_WIDE_INT i; tree field, value;
  FOR_EACH_CONSTRUCTOR_ELT(CONSTRUCTOR_ELTS(constructor), i, field, value)
    if (name_is(field, name)) return value;
  return NULL_TREE;
}
tree function_address(tree t) {
  t = object(t);
  return t != NULL_TREE && TREE_CODE(t) == FUNCTION_DECL ? t : NULL_TREE;
}
cgraph_node *local_body(tree decl) {
  if (decl == NULL_TREE || TREE_PUBLIC(decl) || DECL_WEAK(decl)) return nullptr;
  cgraph_node *n = cgraph_node::get(decl);
  if (n == nullptr || n->alias || n->clone_of != nullptr ||
      !n->has_gimple_body_p() || n->get_fun() == nullptr ||
      n->get_fun()->cfg == nullptr) return nullptr;
#if BUILDING_GCC_MAJOR >= 11
  if (n->thunk) return nullptr;
#else
  if (n->thunk.thunk_p) return nullptr;
#endif
  return n;
}
tree load_reference(tree value, unsigned depth = 0) {
  if (value == NULL_TREE || depth > 16 || TREE_CODE(value) != SSA_NAME) return NULL_TREE;
  gimple *d = SSA_NAME_DEF_STMT(value);
  if (d == nullptr || !is_gimple_assign(d) || gimple_has_volatile_ops(d)) return NULL_TREE;
  if (gimple_assign_load_p(d)) return gimple_assign_rhs1(d);
  tree_code code = gimple_assign_rhs_code(d);
  if (gimple_assign_single_p(d) || code == NOP_EXPR || code == CONVERT_EXPR)
    return load_reference(gimple_assign_rhs1(d), depth + 1);
  return NULL_TREE;
}
tree memory_base(tree reference) {
  if (reference != NULL_TREE && TREE_CODE(reference) == COMPONENT_REF)
    reference = TREE_OPERAND(reference, 0);
  if (reference == NULL_TREE || TREE_CODE(reference) != MEM_REF ||
      TREE_CODE(TREE_OPERAND(reference, 1)) != INTEGER_CST ||
      !integer_zerop(TREE_OPERAND(reference, 1))) return NULL_TREE;
  return TREE_OPERAND(reference, 0);
}
bool parameter(tree value) {
  return value != NULL_TREE && TREE_CODE(value) == SSA_NAME &&
         SSA_NAME_IS_DEFAULT_DEF(value) &&
         SSA_NAME_VAR(value) == DECL_ARGUMENTS(cfun->decl);
}
bool context_pointer(tree value, bool config) {
  if (!config) return parameter(value);
  tree r = load_reference(value);
  return r != NULL_TREE && TREE_CODE(r) == COMPONENT_REF &&
         name_is(TREE_OPERAND(r, 1), "dst") &&
         record_is(DECL_CONTEXT(TREE_OPERAND(r, 1)), "AVFilterLink") &&
         parameter(memory_base(r));
}
bool private_pointer(tree value, bool config, unsigned depth = 0) {
  if (value == NULL_TREE || depth > 16 || TREE_CODE(value) != SSA_NAME) return false;
  tree r = load_reference(value);
  if (r != NULL_TREE && TREE_CODE(r) == COMPONENT_REF &&
      name_is(TREE_OPERAND(r, 1), "priv") &&
      record_is(DECL_CONTEXT(TREE_OPERAND(r, 1)), "AVFilterContext") &&
      context_pointer(memory_base(r), config)) return true;
  gimple *d = SSA_NAME_DEF_STMT(value);
  if (d != nullptr && is_gimple_assign(d) &&
      (gimple_assign_single_p(d) || gimple_assign_rhs_code(d) == NOP_EXPR ||
       gimple_assign_rhs_code(d) == CONVERT_EXPR))
    return private_pointer(gimple_assign_rhs1(d), config, depth + 1);
  return false;
}
tree owned_field(tree reference, bool config, unsigned HOST_WIDE_INT size) {
  for (unsigned depth = 0; reference != NULL_TREE &&
                           TREE_CODE(reference) == SSA_NAME && depth < 16; ++depth) {
    gimple *d = SSA_NAME_DEF_STMT(reference);
    if (d == nullptr || !is_gimple_assign(d) || gimple_has_volatile_ops(d) ||
        !(gimple_assign_single_p(d) || gimple_assign_rhs_code(d) == NOP_EXPR ||
          gimple_assign_rhs_code(d) == CONVERT_EXPR)) break;
    reference = gimple_assign_rhs1(d);
  }
  if (reference != NULL_TREE && TREE_CODE(reference) == ADDR_EXPR)
    reference = TREE_OPERAND(reference, 0);
  if (reference == NULL_TREE || TREE_CODE(reference) != COMPONENT_REF ||
      !POINTER_TYPE_P(TREE_TYPE(reference))) return NULL_TREE;
  tree field = TREE_OPERAND(reference, 1);
  tree bytes = TYPE_SIZE_UNIT(DECL_CONTEXT(field));
  if (bytes == NULL_TREE || !tree_fits_uhwi_p(bytes) || tree_to_uhwi(bytes) != size ||
      !private_pointer(memory_base(reference), config)) return NULL_TREE;
  return field;
}
gimple *allocation_source(tree value, unsigned depth = 0) {
  if (value == NULL_TREE || TREE_CODE(value) != SSA_NAME || depth > 16) return nullptr;
  gimple *d = SSA_NAME_DEF_STMT(value);
  if (library_model::effect(d).kind == library_model::Effect::heap_allocate) return d;
  if (d != nullptr && is_gimple_assign(d) &&
      (gimple_assign_single_p(d) || gimple_assign_rhs_code(d) == NOP_EXPR ||
       gimple_assign_rhs_code(d) == CONVERT_EXPR))
    return allocation_source(gimple_assign_rhs1(d), depth + 1);
  return nullptr;
}
bool exposes_private(tree value, bool config, unsigned HOST_WIDE_INT size) {
  if (value == NULL_TREE || !POINTER_TYPE_P(TREE_TYPE(value))) return false;
  if (private_pointer(value, config) || context_pointer(value, config) ||
      owned_field(value, config, size) != NULL_TREE ||
      owned_field(load_reference(value), config, size) != NULL_TREE) return true;
  return false;
}
struct Success {
  gimple *statement = nullptr;
  std::vector<edge> edges;
};
void zero_origins(tree value, std::vector<edge> suffix,
                  std::vector<std::vector<edge>> *routes, unsigned depth = 0) {
  if (value == NULL_TREE || depth > 16 || routes->size() >= 8 || !charge()) return;
  if (TREE_CODE(value) == INTEGER_CST) {
    if (integer_zerop(value)) routes->push_back(suffix);
    return;
  }
  if (TREE_CODE(value) != SSA_NAME) return;
  gimple *d = SSA_NAME_DEF_STMT(value);
  if (d == nullptr) return;
  if (gimple_code(d) == GIMPLE_PHI) {
    for (unsigned i = 0; i < gimple_phi_num_args(d); ++i) {
      std::vector<edge> next = suffix;
      next.insert(next.begin(), gimple_phi_arg_edge(as_a<gphi *>(d), i));
      zero_origins(gimple_phi_arg_def(d, i), next, routes, depth + 1);
    }
  } else if (is_gimple_assign(d) && gimple_assign_single_p(d))
    zero_origins(gimple_assign_rhs1(d), suffix, routes, depth + 1);
}
struct InitializerInfo {
  std::map<tree, gimple *> allocations;
  std::set<tree> released;
  std::vector<Success> successes;
  bool unknown_consumer = false;
};
InitializerInfo inspect(bool config, unsigned HOST_WIDE_INT size) {
  InitializerInfo result;
  basic_block bb;
  unsigned count = 0;
  FOR_EACH_BB_FN(bb, cfun)
    for (gimple_stmt_iterator i = gsi_start_bb(bb); !gsi_end_p(i); gsi_next(&i)) {
      if (!charge() || ++count > 2048) { result.unknown_consumer = true; return result; }
      gimple *s = gsi_stmt(i);
      if (gimple_code(s) == GIMPLE_ASM) result.unknown_consumer = true;
      if (is_gimple_assign(s) && gimple_assign_single_p(s)) {
        tree field = owned_field(gimple_assign_lhs(s), config, size);
        gimple *allocation = allocation_source(gimple_assign_rhs1(s));
        if (field != NULL_TREE && allocation != nullptr) result.allocations[field] = allocation;
        if (field == NULL_TREE && TREE_CODE(gimple_assign_lhs(s)) != SSA_NAME &&
            exposes_private(gimple_assign_rhs1(s), config, size))
          result.unknown_consumer = true;
      }
      if (gimple_code(s) == GIMPLE_RETURN && config) {
        std::vector<std::vector<edge>> routes;
        zero_origins(gimple_return_retval(as_a<greturn *>(s)), {}, &routes);
        for (const auto &r : routes) result.successes.push_back({s, r});
      }
      if (!is_gimple_call(s)) continue;
      const auto model = library_model::effect(s);
      if (model.kind == library_model::Effect::release_slot ||
          model.kind == library_model::Effect::heap_release) {
        tree arg = gimple_call_arg(s, 0);
        tree field = owned_field(model.kind == library_model::Effect::release_slot ?
                                  arg : load_reference(arg), config, size);
        if (field != NULL_TREE) result.released.insert(field);
        continue;
      }
      for (unsigned a = 0; a < gimple_call_num_args(s); ++a)
        if (exposes_private(gimple_call_arg(s, a), config, size))
          result.unknown_consumer = true;
    }
  return result;
}
}  // namespace

void analyze_registered_lifecycles() {
  if (!library_model::ffmpeg_enabled()) return;
  varpool_node *global;
  unsigned models = 0;
  FOR_EACH_VARIABLE(global) {
    if (!analysis_budget::tu_available() || ++models > 4096) return;
    tree registration = global->decl;
    if (!record_is(TREE_TYPE(registration), "AVFilter") || DECL_WEAK(registration) ||
        !TREE_READONLY(registration)) continue;
    tree init = DECL_INITIAL(registration);
    tree size = member(init, "priv_size");
    tree cleanup = function_address(member(init, "uninit"));
    if (size == NULL_TREE || !tree_fits_uhwi_p(size) || tree_to_uhwi(size) == 0 ||
        tree_to_uhwi(size) > 4096 || local_body(cleanup) == nullptr) continue;
    // A separate initialization callback could establish or replace ownership
    // before config_props. Its composition is outside this initial protocol.
    bool other_init = false;
    for (const char *n : {"preinit", "init", "init_dict", "init_state"}) {
      tree v = member(init, n);
      if (v != NULL_TREE && !(TREE_CODE(v) == INTEGER_CST && integer_zerop(v))) other_init = true;
    }
    if (other_init) continue;
    tree pads = object(member(init, "inputs"));
    if (pads == NULL_TREE || TREE_CODE(pads) != VAR_DECL || !TREE_READONLY(pads)) continue;
    tree array = DECL_INITIAL(pads);
    if (array == NULL_TREE || TREE_CODE(array) != CONSTRUCTOR ||
        CONSTRUCTOR_NELTS(array) != 1) continue;
    tree callback = function_address(member(CONSTRUCTOR_ELT(array, 0)->value, "config_props"));
    cgraph_node *config_node = local_body(callback);
    cgraph_node *cleanup_node = local_body(cleanup);
    if (config_node == nullptr || cleanup_node == nullptr) continue;
    if (!analysis_budget::begin_function(cleanup)) return;
    push_cfun(cleanup_node->get_fun());
    const InitializerInfo destructor = inspect(false, tree_to_uhwi(size));
    pop_cfun(); analysis_budget::end_function();
    if (destructor.unknown_consumer || !analysis_budget::begin_function(callback)) continue;
    push_cfun(config_node->get_fun());
    const InitializerInfo creator = inspect(true, tree_to_uhwi(size));
    if (!creator.unknown_consumer) {
      for (const auto &allocation : creator.allocations) {
        if (destructor.released.count(allocation.first) || creator.released.count(allocation.first)) continue;
        for (const Success &success : creator.successes) {
          const auto path = feasibility::sequence_feasible_through_edges(
              {allocation.second, success.statement}, success.edges);
          feasibility::dump_result(path, "framework-owned-field");
          if (path.verdict == feasibility::Verdict::infeasible) continue;
          if (!analysis_budget::available()) break;
          const std::string field = DECL_NAME(allocation.first) == NULL_TREE ?
              "private pointer field" : IDENTIFIER_POINTER(DECL_NAME(allocation.first));
          report::emit_model_review(report::FindingKind::memory_leak,
              gimple_location(allocation.second),
              "ffmpeg-sdk-v1: registered single-input AVFilter config_props/uninit lifecycle; owned private field " + field,
              feasibility::verdict_name(path.verdict),
              {{gimple_location(allocation.second), "SDK allocation stored in private field " + field},
               {gimple_location(success.statement), "config_props has a normal success return"},
               {DECL_SOURCE_LOCATION(cleanup), "registered uninit has no release for private field " + field}});
          break;
        }
      }
    }
    pop_cfun(); analysis_budget::end_function();
  }
}
}  // namespace resource_protocol
}  // namespace static_analyzer
