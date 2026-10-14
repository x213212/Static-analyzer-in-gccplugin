#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "gimple-iterator.h"
#include "tree-cfg.h"
#include "cgraph.h"
#include "dumpfile.h"
#include "input.h"
#include "ssa.h"
#include "ssa-iterators.h"

#include "static_analyzer/alias_oracle.h"
#include "static_analyzer/analysis_budget.h"
#include "static_analyzer/edge_alias.h"
#include "static_analyzer/feasibility.h"
#include "static_analyzer/gimple_flow.h"
#include "static_analyzer/gimple_value.h"
#include "static_analyzer/lifetime_engine.h"
#include "static_analyzer/local_confinement.h"
#include "static_analyzer/memory_value.h"
#include "static_analyzer/ownership_call.h"
#include "static_analyzer/ownership_summary.h"
#include "static_analyzer/report.h"
#include "static_analyzer/resource_protocol.h"

#include <cstddef>
#include <cstring>
#include <algorithm>
#include <deque>
#include <initializer_list>
#include <map>
#include <set>
#include <vector>

namespace static_analyzer {
namespace {

enum OwnershipState : unsigned int {
  state_none = 0,
  state_unseen = 1u << 0,
  state_live = 1u << 1,
  state_freed = 1u << 2,
  state_escaped = 1u << 3,
  // A successful realloc invalidates its input, but the same call's failure
  // edge leaves that input live.  Keep this distinct from an unconditional
  // free so a subsequent NULL test can refine only the realloc split.
  state_reallocated = 1u << 4
};

enum ThreadState : unsigned int {
  thread_none = 0,
  thread_unseen = 1u << 0,
  thread_outstanding = 1u << 1,
  thread_released = 1u << 2,
  thread_escaped = 1u << 3
};

struct AllocationSite {
  gimple *statement = nullptr;
  tree value = NULL_TREE;
  // A call can introduce an identity whose lifetime ended inside the callee.
  // It is never a live allocation or a caller-side ownership obligation.
  ownership_summary::ReturnedObjectEffect returned_object;
  bool released_on_return() const {
    return returned_object.resolved && !returned_object.may_be_null &&
           returned_object.lifetime ==
               ownership_summary::ReturnedObjectLifetime::released;
  }
  std::vector<location_t> summarized_origins;
  UseExpansion expansion;
  std::map<gimple *, std::vector<UseSite>> uses;
  std::vector<gimple *> release_events;
  // A mixed PHI can be an exact release operand on only one incoming CFG
  // edge.  Retain that edge instead of promoting the PHI's may-alias result
  // to a must-alias release.  The finite edge-state solver below applies the
  // release only to the abstract state which arrived on this exact edge.
  struct ConditionalRelease {
    gimple *statement = nullptr;
    edge incoming = nullptr;
  };
  std::vector<ConditionalRelease> conditional_release_events;
  std::vector<ConditionalRelease> conditional_return_events;
  std::vector<edge> allocation_failure_edges;
  // Events which provably end this function's ownership obligation on every
  // execution reaching the event: exact release, exact return/nonlocal
  // escape, or a body-bound summary which always consumes the argument.
  // Leak feasibility must avoid all of them, not only free calls, otherwise
  // joined abstract states can synthesize a leak from different branches.
  std::vector<gimple *> definite_terminal_events;
  // An unproved release relation is not evidence that no release occurs.
  // Leak witnesses must avoid these possible releases, but typestate must
  // never treat them as definite consumption or proof that the code is safe.
  std::vector<gimple *> uncertain_release_events;
  bool opaque_ownership_transfer = false;
  gimple *opaque_ownership_call = nullptr;
  // A mixed/unknown body effect blocks only paths that execute this call.
  // A separate caller branch may still prove a leak by avoiding every such
  // call. Never choose a LIVE alternative inside an ambiguous callee.
  std::vector<gimple *> ambiguous_ownership_events;
  struct GuardedBorrow {
    gimple *statement = nullptr;
    ownership_summary::BorrowCondition condition;
  };
  std::vector<GuardedBorrow> guarded_borrows;
  bool local_memory_spill = false;
  bool unresolved_call_present = false;
  bool inline_asm_ownership_unknown = false;
  bool opaque_reallocation_status = false;

  // A body-bound callee summary is indexed by the exact lowered actual/formal
  // position.  Keep the whole finite normal-return fate set: a conditional
  // deallocator is may-free, never silently promoted to a definite free.
  struct SummaryEffect {
    gimple *statement = nullptr;
    unsigned int argument_index = 0;
    ownership_summary::ArgumentEffect effect;
    tree accessed_value = NULL_TREE;
  };
  std::vector<SummaryEffect> summary_effects;
};

struct MemoryLoadBridge {
  gimple *statement = nullptr;
  tree stored_value = NULL_TREE;
  tree loaded_value = NULL_TREE;
  UseExpansion loaded_uses;
};

struct MemoryReleaseBridge {
  gimple *statement = nullptr;
  tree stored_value = NULL_TREE;
  location_t release_location = UNKNOWN_LOCATION;
};

struct MemoryAccessBridge {
  gimple *statement = nullptr;
  tree stored_value = NULL_TREE;
  location_t access_location = UNKNOWN_LOCATION;
  ownership_summary::AccessGuarantee access = ownership_summary::AccessGuarantee::none;
};

bool same_pointer_representation(tree left, tree right) {
  if (left == NULL_TREE || right == NULL_TREE ||
      TREE_TYPE(left) == NULL_TREE || TREE_TYPE(right) == NULL_TREE)
    return false;
  tree left_type = TREE_TYPE(left);
  tree right_type = TREE_TYPE(right);
  return POINTER_TYPE_P(left_type) && POINTER_TYPE_P(right_type) &&
         TYPE_PRECISION(left_type) == TYPE_PRECISION(right_type) &&
         TYPE_MODE(left_type) == TYPE_MODE(right_type) &&
         TYPE_ADDR_SPACE(TREE_TYPE(left_type)) ==
             TYPE_ADDR_SPACE(TREE_TYPE(right_type));
}

std::vector<MemoryReleaseBridge> collect_memory_release_bridges() {
  std::vector<MemoryReleaseBridge> bridges;
  basic_block block;
  FOR_EACH_BB_FN(block, cfun) {
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::lifetime_discovery))
        return bridges;
      gimple *statement = gsi_stmt(gsi);
      if (!is_gimple_call(statement))
        continue;
      for (const ownership_summary::MemoryReleaseEffect &effect :
           ownership_summary::memory_release_effects(statement)) {
        const MemoryValueResult memory = resolve_pointer_reference(
            effect.reference, gimple_vuse(statement));
        if (!analysis_budget::consume(
                analysis_budget::WorkKind::lifetime_discovery,
                memory.visited_states + memory.walked_vdefs + 1))
          return bridges;
        tree stored = memory.unique_stored_value();
        if (stored == NULL_TREE || memory.definitions.size() != 1 ||
            !same_pointer_representation(effect.reference, stored) ||
            !same_pointer_representation(
                effect.reference, memory.definitions.front().memory_reference) ||
            memory.definitions.front().statement == nullptr ||
            gimple_has_volatile_ops(memory.definitions.front().statement))
          continue;
        bridges.push_back({statement, stored, effect.release_location});
        if (dump_file != nullptr) {
          dump_printf(MSG_NOTE,
                      "memory-release: origin=body entry-memory=must-kill "
                      "callee-release-location=%lu\n",
                      static_cast<unsigned long>(effect.release_location));
          if (dump_flags & TDF_DETAILS) {
            dump_gimple_stmt(MSG_NOTE, TDF_SLIM, statement, 0);
            dump_memory_value_result(memory);
          }
        }
      }
    }
  }
  return bridges;
}

std::vector<MemoryAccessBridge> collect_memory_access_bridges() {
  std::vector<MemoryAccessBridge> bridges;
  basic_block block;
  FOR_EACH_BB_FN(block, cfun) {
    for (gimple_stmt_iterator iterator = gsi_start_bb(block); !gsi_end_p(iterator); gsi_next(&iterator)) {
      if (!analysis_budget::consume(analysis_budget::WorkKind::lifetime_discovery))
        return bridges;
      gimple *statement = gsi_stmt(iterator);
      if (!is_gimple_call(statement))
        continue;
      for (const auto &effect : ownership_summary::memory_access_effects(statement)) {
        const MemoryValueResult memory = resolve_pointer_reference(effect.reference, gimple_vuse(statement));
        if (!analysis_budget::consume(analysis_budget::WorkKind::lifetime_discovery,
                                     memory.visited_states + memory.walked_vdefs + 1))
          return bridges;
        tree stored = memory.unique_stored_value();
        if (stored == NULL_TREE || memory.definitions.size() != 1 ||
            !same_pointer_representation(effect.reference, stored) ||
            !same_pointer_representation(effect.reference, memory.definitions.front().memory_reference) ||
            memory.definitions.front().statement == nullptr ||
            gimple_has_volatile_ops(memory.definitions.front().statement))
          continue;
        bridges.push_back({statement, stored, effect.access_location, effect.access});
      }
    }
  }
  return bridges;
}

using FeasibilityCache =
    std::map<std::pair<gimple *, gimple *>, feasibility::Result>;

struct ThreadSite {
  gimple *statement = nullptr;
  tree storage = NULL_TREE;
};

const char *call_name(const gimple *statement) {
  if (statement == nullptr || !is_gimple_call(statement))
    return nullptr;
  tree declaration = gimple_call_fndecl(statement);
  if (declaration == NULL_TREE || DECL_NAME(declaration) == NULL_TREE)
    return nullptr;
  return IDENTIFIER_POINTER(DECL_NAME(declaration));
}

bool named(const gimple *statement, const char *expected) {
  const char *actual = call_name(statement);
  return actual != nullptr && std::strcmp(actual, expected) == 0;
}

bool pointer_value(tree value) {
  return value != NULL_TREE && value != error_mark_node &&
         CODE_CONTAINS_STRUCT(TREE_CODE(value), TS_TYPED) &&
         TREE_TYPE(value) != NULL_TREE &&
         POINTER_TYPE_P(TREE_TYPE(value));
}

bool source_location_p(location_t location) {
  if (LOCATION_LOCUS(location) == UNKNOWN_LOCATION)
    return false;
  const expanded_location expanded = expand_location(location);
  return expanded.file != nullptr && expanded.file[0] != '\0' &&
         expanded.line > 0;
}

location_t best_source_location(
    std::initializer_list<const gimple *> statements) {
  for (const gimple *statement : statements) {
    if (statement == nullptr)
      continue;
    const location_t nonartificial =
        gimple_nonartificial_location(statement);
    if (source_location_p(nonartificial))
      return nonartificial;
    const location_t direct = gimple_location(statement);
    if (source_location_p(direct))
      return direct;
  }
  if (current_function_decl != NULL_TREE) {
    const location_t function_location =
        DECL_SOURCE_LOCATION(current_function_decl);
    if (source_location_p(function_location))
      return function_location;
  }
  return UNKNOWN_LOCATION;
}

void record_budget_inconclusive(const PluginOptions &options,
                                location_t location) {
  const std::string reason =
      std::string("analysis-budget-") +
      analysis_budget::reason_name(analysis_budget::reason());
  const std::string detail =
      "translation-unit or function analysis stopped before a complete "
      "ownership proof";
  if (options.memory_leak)
    report::record_inconclusive(report::FindingKind::memory_leak, location,
                                reason, detail);
  if (options.double_free)
    report::record_inconclusive(report::FindingKind::double_free, location,
                                reason, detail);
  if (options.use_after_free)
    report::record_inconclusive(report::FindingKind::use_after_free, location,
                                reason, detail);
  if (options.thread)
    report::record_inconclusive(report::FindingKind::joinable_thread_leak,
                                location, reason, detail);
}

std::vector<report::TracePoint> allocation_origin_trace(
    const AllocationSite &allocation) {
  std::vector<report::TracePoint> trace;
  for (location_t location : allocation.summarized_origins)
    if (source_location_p(location))
      trace.push_back({location, "callee allocation origin"});
  return trace;
}

bool same_owned_object(tree candidate, tree allocation) {
  return pointer_value(candidate) && pointer_value(allocation) &&
         alias::proven_same_object(candidate, allocation);
}

bool same_returned_object_origin(tree candidate,
                                const AllocationSite &allocation,
                                bool allow_interior) {
  if (!allocation.released_on_return() || !pointer_value(candidate))
    return false;
  const ValueExpansion expansion = expand_pointer_value(candidate);
  if (!expansion.complete || expansion.origins.size() != 1)
    return false;
  const ValueOrigin &origin = expansion.origins.front();
  // This is an identity proof local to this imported lifetime token. Do not
  // classify the call as a fresh/no-alias allocator in the shared oracle.
  return origin.kind == OriginKind::call_result &&
         origin.defining_statement == allocation.statement &&
         (allow_interior || !origin.derived_pointer);
}

bool same_owned_object_after_allocation(
    tree candidate, const AllocationSite &allocation) {
  if (same_returned_object_origin(candidate, allocation, false))
    return true;
  if (same_owned_object(candidate, allocation.value))
    return true;
  // The path-relative proof is only useful when ordinary provenance already
  // finds this allocation on at least one arm.  This cheap guard avoids a CFG
  // walk for every unrelated allocation/release pair in large functions.
  if (!alias::may_share_allocation_origin(candidate, allocation.value))
    return false;
  const edge_alias::Evidence evidence = edge_alias::prove_same_object_after(
      candidate, allocation.value, allocation.statement);
  edge_alias::dump_evidence(candidate, allocation.value, evidence);
  return evidence.proven();
}

tree nonnull_call_argument_value(const gimple *statement,
                                unsigned int argument_index,
                                const AllocationSite &allocation) {
  if (statement == nullptr || !is_gimple_call(statement) ||
      argument_index >= gimple_call_num_args(statement))
    return NULL_TREE;
  tree actual = gimple_call_arg(statement, argument_index);
  if (actual == allocation.value ||
      !same_pointer_representation(actual, allocation.value))
    return actual;
  if (!analysis_budget::consume(
          analysis_budget::WorkKind::lifetime_allocation_statement) ||
      !same_owned_object_after_allocation(actual, allocation) ||
      !analysis_budget::available())
    return actual;

  // This fresh, argument-indexed proof identifies the exact non-derived
  // value on every path after this allocation. A PHI may still contain NULL
  // on an incoming edge which cannot follow the allocation; that unused arm
  // must not make the call's nonnull precondition inexact. Substitute only
  // inside the release/use query, which also requires the allocation event.
  // Do not canonicalize PHIs globally or discard any query edge constraints.
  // Mixed reachable objects, derived pointers and uncertain loop epochs fail
  // the same ownership-grade proof and retain their original predicate.
  if (dump_file != nullptr && (dump_flags & TDF_DETAILS)) {
    dump_printf(MSG_NOTE,
                "call-contract: argument=%u nonnull-value=allocation-root "
                "exact-after-allocation=yes\n", argument_index);
    dump_gimple_stmt(MSG_NOTE, TDF_SLIM,
                     const_cast<gimple *>(statement), 0);
  }
  return allocation.value;
}

bool contains_statement(const std::vector<gimple *> &statements,
                        const gimple *candidate) {
  for (const gimple *statement : statements)
    if (statement == candidate)
      return true;
  return false;
}

bool has_summary_effect(const AllocationSite &allocation,
                        const gimple *statement) {
  for (const AllocationSite::SummaryEffect &effect :
       allocation.summary_effects)
    if (effect.statement == statement)
      return true;
  return false;
}

bool summary_effect_may_free(const AllocationSite &allocation,
                             const gimple *statement) {
  for (const AllocationSite::SummaryEffect &effect :
       allocation.summary_effects)
    if (effect.statement == statement && effect.effect.may_free())
      return true;
  return false;
}

bool summary_effect_definitely_frees(const AllocationSite &allocation,
                                     const gimple *statement) {
  for (const AllocationSite::SummaryEffect &effect :
       allocation.summary_effects)
    if (effect.statement == statement && effect.effect.definitely_freed())
      return true;
  return false;
}

void append_release_origin(std::vector<report::TracePoint> *trace,
                           const AllocationSite &allocation,
                           const gimple *statement) {
  if (statement == nullptr)
    return;
  ownership_summary::ReleaseOrigin origin;
  for (const AllocationSite::SummaryEffect &item : allocation.summary_effects) {
    if (item.statement != statement || !item.effect.definitely_freed())
      continue;
    const ownership_summary::ReleaseOrigin &candidate = item.effect.release_origin;
    // Multiple arguments can name this same allocation.  Do not select an
    // arbitrary free site if their complete release evidence disagrees or
    // one of the definite releases has no source witness.
    if (candidate.location == UNKNOWN_LOCATION ||
        (origin.location != UNKNOWN_LOCATION &&
         (candidate.location != origin.location ||
          candidate.through_memory != origin.through_memory)))
      return;
    origin = candidate;
  }
  if (origin.location != UNKNOWN_LOCATION)
    trace->push_back({origin.location,
                      origin.through_memory
                          ? "callee releases the pointer read from caller memory"
                          : "callee releases this argument"});
}

std::vector<report::TracePoint> lifetime_release_trace(
    const AllocationSite &allocation, const gimple *release,
    bool repeated_release = false) {
  if (!allocation.released_on_return())
    return {{best_source_location({allocation.statement}), "allocation"},
            {best_source_location({release, allocation.statement}),
             repeated_release ? "previous execution of this release"
                              : "previous release"}};

  std::vector<report::TracePoint> trace;
  const auto &returned = allocation.returned_object;
  if (source_location_p(returned.allocation_location))
    trace.push_back({returned.allocation_location, "callee allocation origin"});
  if (source_location_p(returned.release_location))
    trace.push_back({returned.release_location, "callee releases before return"});
  if (source_location_p(returned.return_location))
    trace.push_back({returned.return_location, "callee returns released pointer"});
  trace.push_back({best_source_location({allocation.statement}),
                   "released object returned from callee"});
  if (release != allocation.statement)
    trace.push_back({best_source_location({release}), "previous caller release"});
  return trace;
}

bool summary_effect_may_dereference(const AllocationSite &allocation,
                                    const gimple *statement) {
  for (const AllocationSite::SummaryEffect &effect :
       allocation.summary_effects)
    if (effect.statement == statement && effect.effect.may_dereference)
      return true;
  return false;
}

unsigned int apply_summary_effects(const AllocationSite &allocation,
                                   const gimple *statement,
                                   unsigned int state,
                                   const PluginOptions &options) {
  const unsigned int incoming_state = state;
  bool matched = false;
  bool definitely_consumed = false;
  bool establishes_free = false;
  bool may_escape = false;
  for (const AllocationSite::SummaryEffect &item :
       allocation.summary_effects) {
    if (item.statement != statement || !item.effect.resolved)
      continue;
    matched = true;
    definitely_consumed =
        definitely_consumed || item.effect.definitely_consumed();
    establishes_free =
        establishes_free ||
        (options.analysis_policy == AnalysisPolicy::proof_only
             ? item.effect.definitely_freed()
             : item.effect.may_free());
    may_escape = may_escape || item.effect.may_escape();
  }
  if (!matched || (incoming_state & state_live) == 0)
    return state;
  if (definitely_consumed)
    state &= ~state_live;
  if (establishes_free)
    state |= state_freed;
  if (may_escape)
    state |= state_escaped;
  return state;
}

bool call_use_may_dereference(const gimple *statement, tree incoming) {
  if (statement == nullptr || !is_gimple_call(statement))
    return false;
  bool found_argument = false;
  for (unsigned int index = 0; index < gimple_call_num_args(statement);
       ++index) {
    if (gimple_call_arg(statement, index) != incoming)
      continue;
    found_argument = true;
    const ownership_summary::ArgumentEffect effect =
        ownership_summary::argument_effect(statement, index);
    // An unresolved target remains conservative.  A resolved no-op sink is
    // the useful precision case: merely passing a dangling value without
    // reading through it is not a use-after-free.
    if (!effect.resolved || effect.may_dereference)
      return true;
  }
  return !found_argument;
}

bool call_use_has_resolved_may_dereference(const gimple *statement,
                                           tree incoming) {
  if (statement == nullptr || !is_gimple_call(statement))
    return false;
  for (unsigned int index = 0; index < gimple_call_num_args(statement);
       ++index) {
    if (gimple_call_arg(statement, index) != incoming)
      continue;
    const ownership_summary::ArgumentEffect effect =
        ownership_summary::argument_effect(statement, index);
    if (effect.resolved && effect.may_dereference)
      return true;
  }
  return false;
}

bool contains_conditional_release(const AllocationSite &allocation,
                                  const gimple *statement,
                                  edge incoming) {
  if (incoming == nullptr)
    return false;
  for (const AllocationSite::ConditionalRelease &release :
       allocation.conditional_release_events)
    if (release.statement == statement && release.incoming == incoming)
      return true;
  return false;
}

bool contains_conditional_return(const AllocationSite &allocation,
                                 const gimple *statement, edge incoming) {
  for (const AllocationSite::ConditionalRelease &event :
       allocation.conditional_return_events)
    if (incoming != nullptr && event.statement == statement &&
        event.incoming == incoming)
      return true;
  return false;
}

bool add_conditional_phi_returns(AllocationSite *allocation,
                                 gimple *statement) {
  tree returned = gimple_return_retval(as_a<greturn *>(statement));
  if (returned == NULL_TREE || TREE_CODE(returned) != SSA_NAME)
    return false;
  gimple *definition = SSA_NAME_DEF_STMT(returned);
  if (definition == nullptr || gimple_code(definition) != GIMPLE_PHI ||
      gimple_bb(definition) != gimple_bb(statement) ||
      gimple_bb(allocation->statement) == gimple_bb(statement))
    return false;
  const edge_alias::Evidence proof = edge_alias::prove_same_object_after(
      returned, allocation->value, allocation->statement);
  if (proof.epoch_cycle_detected || proof.cycle_detected ||
      proof.depth_exhausted || proof.value_limit_exhausted ||
      proof.cfg_limit_exhausted)
    return false;
  const gphi *phi = as_a<gphi *>(definition);
  if (gimple_phi_num_args(phi) > 256 ||
      allocation->conditional_return_events.size() +
          gimple_phi_num_args(phi) > 4096)
    return false;
  std::vector<AllocationSite::ConditionalRelease> events;
  for (unsigned int index = 0; index < gimple_phi_num_args(phi); ++index) {
    tree argument = gimple_phi_arg_def(phi, index);
    edge incoming = gimple_phi_arg_edge(phi, index);
    if (incoming == nullptr || incoming->dest != gimple_bb(statement) ||
        (incoming->flags & (EDGE_FAKE | EDGE_ABNORMAL | EDGE_EH)) != 0)
      return false;
    if (same_owned_object(argument, allocation->value))
      events.push_back({statement, incoming});
    else if (TREE_CODE(argument) != INTEGER_CST || !integer_zerop(argument))
      return false;
  }
  if (events.empty())
    return false;
  allocation->conditional_return_events.insert(
      allocation->conditional_return_events.end(), events.begin(), events.end());
  return true;
}

bool same_conditional_release(
    const AllocationSite::ConditionalRelease &left,
    const AllocationSite::ConditionalRelease &right) {
  return left.statement == right.statement &&
         left.incoming == right.incoming;
}

void append_conditional_release(
    AllocationSite *allocation,
    const AllocationSite::ConditionalRelease &release) {
  for (const AllocationSite::ConditionalRelease &existing :
       allocation->conditional_release_events)
    if (same_conditional_release(existing, release))
      return;
  allocation->conditional_release_events.push_back(release);
}

// Recognize the lowered form of an edge-correlated ownership release:
//
//   value = PHI <allocation(edge-a), other(edge-b)>;
//   free (value);
//
// The PHI argument is an edge use in GCC GIMPLE.  Exact object identity on
// one argument is therefore enough to release the allocation on that one
// incoming edge, but never enough to classify the merged SSA name itself as
// a must-alias release.  We deliberately limit this proof to a PHI in the
// release block.  That lets the solver preserve its incoming-edge identity
// until the actual call, without enumerating paths or applying a release
// before intervening statements.
bool add_conditional_phi_releases(AllocationSite *allocation,
                                  gimple *release) {
  if (allocation == nullptr || release == nullptr ||
      !ownership_deallocation_call_p(release) ||
      gimple_call_num_args(release) == 0)
    return false;

  tree candidate = gimple_call_arg(release, 0);
  if (candidate == NULL_TREE || TREE_CODE(candidate) != SSA_NAME)
    return false;
  // Existential provenance is only a traversal guard here.  It never
  // establishes a release; each selected arm still needs exact identity.
  if (!alias::may_share_allocation_origin(candidate, allocation->value))
    return false;
  gimple *definition = SSA_NAME_DEF_STMT(candidate);
  if (definition == nullptr || gimple_code(definition) != GIMPLE_PHI ||
      gimple_bb(definition) == nullptr ||
      gimple_bb(definition) != gimple_bb(release))
    return false;

  // Static SSA identity is not dynamic allocation-epoch identity on a loop
  // backedge.  Reuse the bounded allocation-relative oracle's SCC and limit
  // evidence; any cycle or exhausted proof remains unknown and cannot
  // suppress a leak.
  const edge_alias::Evidence aggregate = edge_alias::prove_same_object_after(
      candidate, allocation->value, allocation->statement);
  edge_alias::dump_evidence(candidate, allocation->value, aggregate);
  if (aggregate.epoch_cycle_detected || aggregate.cycle_detected ||
      aggregate.depth_exhausted || aggregate.value_limit_exhausted ||
      aggregate.cfg_limit_exhausted)
    return false;

  const gphi *phi = as_a<gphi *>(definition);
  constexpr unsigned int max_phi_arguments = 256;
  constexpr unsigned int max_conditional_releases = 4096;
  const unsigned int argument_count = gimple_phi_num_args(phi);
  if (argument_count > max_phi_arguments ||
      allocation->conditional_release_events.size() >=
          max_conditional_releases) {
    if (dump_file != nullptr)
      dump_printf(MSG_NOTE,
                  "conditional-release: skipped bounded PHI proof "
                  "arguments=%u existing-events=%u\n",
                  argument_count,
                  static_cast<unsigned int>(
                      allocation->conditional_release_events.size()));
    return false;
  }
  bool complete = true;
  for (unsigned int index = 0; index < argument_count; ++index) {
    edge incoming = gimple_phi_arg_edge(phi, index);
    if (incoming == nullptr || incoming->dest != gimple_bb(release) ||
        (incoming->flags & (EDGE_FAKE | EDGE_ABNORMAL | EDGE_EH)) != 0)
      return false;
    tree argument = gimple_phi_arg_def(phi, index);
    if (!same_owned_object(argument, allocation->value)) {
      // A NULL PHI arm cannot release a live allocation. PTA may conservatively
      // overlap NULL with the nullable allocation result; that is not a
      // possible deallocation of its successful allocation epoch.
      if (!(argument != NULL_TREE && TREE_CODE(argument) == INTEGER_CST &&
            integer_zerop(argument)) &&
          alias::may_alias(argument, allocation->value))
        complete = false;
      continue;
    }
    append_conditional_release(allocation, {release, incoming});
    if (dump_file != nullptr && (dump_flags & TDF_DETAILS)) {
      dump_printf(MSG_NOTE,
                  "conditional-release: exact PHI arm src-bb=%d "
                  "dest-bb=%d\n",
                  incoming->src->index, incoming->dest->index);
      dump_gimple_stmt(MSG_NOTE, TDF_SLIM, release, 0);
    }
  }
  return complete;
}

bool same_allocation(tree candidate, tree allocation) {
  return pointer_value(candidate) && pointer_value(allocation) &&
         alias::proven_same_allocation(candidate, allocation);
}

bool invalidated_state(unsigned int state) {
  return (state & (state_freed | state_reallocated)) != 0;
}

const gimple *single_reallocation_origin(tree value) {
  const ValueExpansion expansion = expand_pointer_value(value);
  if (!expansion.complete || expansion.origins.size() != 1 ||
      expansion.origins.front().kind != OriginKind::reallocation_call)
    return nullptr;
  return expansion.origins.front().defining_statement;
}

bool phi_has_null_arm(const gimple *statement) {
  if (statement == nullptr || gimple_code(statement) != GIMPLE_PHI)
    return false;
  const gphi *phi = as_a<gphi *>(const_cast<gimple *>(statement));
  for (unsigned int index = 0; index < gimple_phi_num_args(phi); ++index) {
    const ValueExpansion expansion =
        expand_pointer_value(gimple_phi_arg_def(phi, index));
    for (const ValueOrigin &origin : expansion.origins)
      if (origin.kind == OriginKind::null_pointer)
        return true;
  }
  return false;
}

bool reallocation_result_has_direct_null_guard(
    const gimple *statement, const UseExpansion *expansion = nullptr) {
  if (statement == nullptr || !ownership_reallocation_call_p(statement))
    return false;
  tree result = gimple_call_lhs(statement);
  if (result == NULL_TREE || TREE_CODE(result) != SSA_NAME)
    return false;
  imm_use_iterator iterator;
  gimple *use;
  bool has_direct_guard = false;
  FOR_EACH_IMM_USE_STMT(use, iterator, result) {
    if (gimple_code(use) != GIMPLE_COND)
      continue;
    const enum tree_code code = gimple_cond_code(use);
    if (code != EQ_EXPR && code != NE_EXPR)
      continue;
    tree left = gimple_cond_lhs(use);
    tree right = gimple_cond_rhs(use);
    if ((left == result && right != NULL_TREE &&
         TREE_CODE(right) == INTEGER_CST && integer_zerop(right)) ||
        (right == result && left != NULL_TREE &&
         TREE_CODE(left) == INTEGER_CST && integer_zerop(left)))
      has_direct_guard = true;
  }
  // Do not return or break from FOR_EACH_IMM_USE_STMT.  GCC 10 temporarily
  // links an iterator marker into the immediate-use chain and its loop
  // increment removes that marker.  Leaving early corrupts the SSA use list
  // and makes a later optimization pass crash.
  if (!has_direct_guard)
    return false;

  // A direct guard is not sufficient if the result is subsequently merged
  // with NULL before an ownership operation.  That lowering loses the direct
  // realloc-result/status identity (e.g. `selected = result ? result : 0`).
  // Keep such a candidate inconclusive until the PHI path predicate is
  // explicitly substituted into the realloc ownership state.
  if (expansion != nullptr)
    for (const UseSite &site : expansion->sites)
      if (site.edge == FlowEdgeKind::phi && phi_has_null_arm(site.statement))
        return false;
  return true;
}

unsigned int reallocation_event_count(const AllocationSite &allocation) {
  unsigned int count = 0;
  for (const gimple *event : allocation.release_events)
    if (ownership_reallocation_call_p(event))
      ++count;
  return count;
}

bool site_mentions_allocation(const UseSite &site, tree allocation,
                              bool allow_interior) {
  if (!allow_interior)
    return same_owned_object(site.incoming_value, allocation);

  return alias::may_share_allocation_origin(site.incoming_value,
                                            allocation);
}

bool site_proves_allocation(const UseSite &site,
                            const AllocationSite &allocation) {
  if (same_returned_object_origin(site.incoming_value, allocation, true))
    return true;
  if (alias::proven_same_allocation(site.incoming_value,
                                    allocation.value))
    return true;
  // A mixed PHI is not a must-alias as a whole.  The immediate-use walker
  // retains the exact incoming edge for each arm; accepting that arm still
  // requires the correlated feasibility query below.
  return !allocation.expansion.witnesses_truncated &&
         !site.required_edges.empty() &&
         alias::may_share_allocation_origin(site.incoming_value,
                                            allocation.value);
}

// GCC can lower several source-level returns into one artificial
// GIMPLE_RETURN fed by an SSA PHI.  In that form the return statement itself
// often carries only the function declaration location; the exact source
// locus is retained on the PHI argument.  Use the already-recorded
// allocation-relative edge witness to recover that locus.  This is diagnostic
// provenance only: the same required edge is independently checked by the
// feasibility query before a finding is emitted.
location_t return_provenance_location(const AllocationSite &allocation,
                                      gimple *statement,
                                      const UseSite &site) {
  if (statement == nullptr || gimple_code(statement) != GIMPLE_RETURN ||
      site.statement != statement ||
      site.edge != FlowEdgeKind::return_value ||
      !site_proves_allocation(site, allocation) ||
      site.incoming_value == NULL_TREE ||
      TREE_CODE(site.incoming_value) != SSA_NAME)
    return UNKNOWN_LOCATION;

  tree returned = gimple_return_retval(as_a<greturn *>(statement));
  if (returned == NULL_TREE || site.incoming_value != returned)
    return UNKNOWN_LOCATION;

  gimple *definition = SSA_NAME_DEF_STMT(site.incoming_value);
  if (definition == nullptr || gimple_code(definition) != GIMPLE_PHI ||
      gimple_bb(definition) == nullptr)
    return UNKNOWN_LOCATION;

  const gphi *phi = as_a<gphi *>(definition);
  for (auto required = site.required_edges.rbegin();
       required != site.required_edges.rend(); ++required) {
    if (*required == nullptr)
      continue;
    for (std::size_t index = 0; index < gimple_phi_num_args(phi); ++index) {
      if (gimple_phi_arg_edge(phi, index) != *required)
        continue;
      tree argument = gimple_phi_arg_def(phi, index);
      if (!same_owned_object(argument, allocation.value) &&
          !alias::may_share_allocation_origin(argument, allocation.value))
        continue;
      const location_t location = gimple_phi_arg_location(phi, index);
      if (source_location_p(location))
        return location;
    }
  }
  return UNKNOWN_LOCATION;
}

bool statement_mentions_allocation(const AllocationSite &allocation,
                                    gimple *statement,
                                    bool allow_interior) {
  const auto found = allocation.uses.find(statement);
  if (found == allocation.uses.end())
    return false;
  for (const UseSite &site : found->second)
    if (same_returned_object_origin(site.incoming_value, allocation,
                                    allow_interior) ||
        site_mentions_allocation(site, allocation.value, allow_interior))
      return true;
  return false;
}

bool pointer_store_escape(const AllocationSite &allocation,
                          gimple *statement,
                          bool allow_interior = false) {
  if (!is_gimple_assign(statement) ||
      gimple_assign_single_p(statement) == false)
    return false;
  tree lhs = gimple_assign_lhs(statement);
  tree rhs = gimple_assign_rhs1(statement);
  if (lhs == NULL_TREE || rhs == NULL_TREE || TREE_CODE(lhs) == SSA_NAME)
    return false;
  // Proof-only ownership escape needs the exact allocation base.  Sharing the
  // same allocation origin is insufficient: an interior pointer stored
  // outside the function need not let the receiver release the object
  // correctly.  Exploratory mode retains its historical may-transfer model.
  if (!(allow_interior ? same_allocation(rhs, allocation.value)
                       : same_owned_object(rhs, allocation.value)))
    return false;

  // A write to this function's automatic storage is an intra-procedural
  // spill, not an ownership escape.  Parameter/global/indirect storage can
  // outlive the current scope and is conservatively treated as an escape.
  tree base = get_base_address(lhs);
  if (base != NULL_TREE && DECL_P(base) &&
      auto_var_in_fn_p(base, cfun->decl))
    return false;
  return true;
}

bool pointer_local_spill(const AllocationSite &allocation,
                         gimple *statement) {
  if (!is_gimple_assign(statement) || !gimple_assign_single_p(statement))
    return false;
  tree lhs = gimple_assign_lhs(statement);
  tree rhs = gimple_assign_rhs1(statement);
  if (lhs == NULL_TREE || rhs == NULL_TREE || TREE_CODE(lhs) == SSA_NAME ||
      !same_allocation(rhs, allocation.value))
    return false;
  tree base = get_base_address(lhs);
  return base != NULL_TREE && DECL_P(base) &&
         auto_var_in_fn_p(base, cfun->decl);
}

bool pointer_store_may_transfer(const AllocationSite &allocation,
                                gimple *statement) {
  if (!is_gimple_assign(statement) || !gimple_assign_single_p(statement))
    return false;
  tree lhs = gimple_assign_lhs(statement);
  tree rhs = gimple_assign_rhs1(statement);
  return lhs != NULL_TREE && rhs != NULL_TREE &&
         TREE_CODE(lhs) != SSA_NAME && pointer_value(rhs) &&
         alias::may_share_allocation_origin(rhs, allocation.value);
}

bool same_use_site(const UseSite &left, const UseSite &right) {
  return left.statement == right.statement &&
         left.incoming_value == right.incoming_value &&
         left.produced_value == right.produced_value &&
         left.edge == right.edge &&
         left.required_edges == right.required_edges;
}

void append_use(AllocationSite *allocation, const UseSite &site) {
  std::vector<UseSite> &statement_uses = allocation->uses[site.statement];
  for (const UseSite &existing : statement_uses)
    if (same_use_site(existing, site))
      return;
  statement_uses.push_back(site);
  allocation->expansion.sites.push_back(site);
}

// Immediate-use iteration intentionally follows SSA names only.  Bridge an
// allocation through pointer-valued memory loads with GCC's virtual SSA
// walker, then resume the normal immediate-use traversal from the loaded SSA
// value.  Only a complete, unique must-quality reaching store is accepted.
std::vector<MemoryLoadBridge> collect_memory_load_bridges() {
  std::vector<MemoryLoadBridge> bridges;
  basic_block block;
  FOR_EACH_BB_FN(block, cfun) {
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::lifetime_discovery))
        return bridges;
      gimple *statement = gsi_stmt(gsi);
      if (!is_gimple_assign(statement) ||
          !gimple_assign_load_p(statement))
        continue;
      tree loaded_value = gimple_assign_lhs(statement);
      if (!pointer_value(loaded_value))
        continue;

      const MemoryValueResult memory_value =
          resolve_pointer_load(statement);
      dump_memory_value_result(memory_value);
      tree stored_value = memory_value.unique_stored_value();
      if (stored_value == NULL_TREE)
        continue;

      MemoryLoadBridge bridge;
      bridge.statement = statement;
      bridge.stored_value = stored_value;
      bridge.loaded_value = loaded_value;
      bridge.loaded_uses = expand_immediate_uses(loaded_value);
      bridges.push_back(bridge);
    }
  }
  return bridges;
}

void add_memory_load_uses(
    AllocationSite *allocation,
    const std::vector<MemoryLoadBridge> &bridges) {
  for (const MemoryLoadBridge &bridge : bridges) {
    if (!same_returned_object_origin(bridge.stored_value, *allocation, true) &&
        !same_allocation(bridge.stored_value, allocation->value))
      continue;

    UseSite load_site;
    load_site.statement = bridge.statement;
    load_site.incoming_value = bridge.stored_value;
    load_site.produced_value = bridge.loaded_value;
    load_site.edge = FlowEdgeKind::memory_load;
    load_site.propagates_value = true;
    append_use(allocation, load_site);

    allocation->expansion.values_visited +=
        bridge.loaded_uses.values_visited;
    allocation->expansion.truncated =
        allocation->expansion.truncated || bridge.loaded_uses.truncated;
    allocation->expansion.witnesses_truncated =
        allocation->expansion.witnesses_truncated ||
        bridge.loaded_uses.witnesses_truncated;
    for (const UseSite &use : bridge.loaded_uses.sites)
      append_use(allocation, use);
  }
}

feasibility::Result release_path_result(
    const AllocationSite &allocation, gimple *statement,
    const char *purpose, FeasibilityCache *cache, gimple **witness,
    const std::vector<edge> &required_edges = {},
    const std::vector<feasibility::RequiredPredicate> &required_predicates =
        {}) {
  // An invalidated state can only be introduced by one of these free/realloc
  // events.  Ask the bounded solver which candidate can really precede this
  // statement on one execution; UNSAT removes a path-insensitive false alarm.
  feasibility::Result last_infeasible;
  last_infeasible.verdict = feasibility::Verdict::infeasible;
  last_infeasible.reason = "no feasible release/use ordering";
  feasibility::Result first_unknown;
  bool saw_unknown = false;
  if (allocation.released_on_return()) {
    // The release is part of the completed call certificate, not a second
    // execution of the call statement. Caller event order has only two
    // events, and retains every caller edge/contract predicate in one query.
    const std::pair<gimple *, gimple *> key(allocation.statement, statement);
    feasibility::Result result;
    if (required_edges.empty() && required_predicates.empty()) {
      auto found = cache->find(key);
      if (found == cache->end()) {
        result = feasibility::sequence_feasible(
            {allocation.statement, statement});
        feasibility::dump_result(result, purpose);
        found = cache->emplace(key, result).first;
      }
      result = found->second;
    } else {
      result = feasibility::sequence_feasible_with_predicates(
          {allocation.statement, statement}, required_edges,
          required_predicates);
      feasibility::dump_result(result, purpose);
    }
    if (result.verdict == feasibility::Verdict::feasible) {
      if (witness != nullptr)
        *witness = allocation.statement;
      return result;
    }
    if (result.verdict == feasibility::Verdict::unknown) {
      first_unknown = result;
      saw_unknown = true;
    } else {
      last_infeasible = result;
    }
  }
  for (gimple *release : allocation.release_events) {
    if (!required_edges.empty() || !required_predicates.empty()) {
      const feasibility::Result result =
          feasibility::sequence_feasible_with_predicates(
              {allocation.statement, release, statement}, required_edges,
              required_predicates);
      feasibility::dump_result(result, purpose);
      if (result.verdict == feasibility::Verdict::feasible) {
        if (witness != nullptr)
          *witness = release;
        return result;
      }
      if (result.verdict == feasibility::Verdict::unknown && !saw_unknown) {
        first_unknown = result;
        saw_unknown = true;
      } else if (result.verdict == feasibility::Verdict::infeasible) {
        last_infeasible = result;
      }
      continue;
    }
    const std::pair<gimple *, gimple *> key(release, statement);
    auto found = cache->find(key);
    if (found == cache->end()) {
      const feasibility::Result result = feasibility::sequence_feasible(
          {allocation.statement, release, statement});
      feasibility::dump_result(result, purpose);
      found = cache->insert(std::make_pair(key, result)).first;
    }
    if (found->second.verdict == feasibility::Verdict::feasible) {
      if (witness != nullptr)
        *witness = release;
      return found->second;
    }
    if (found->second.verdict == feasibility::Verdict::unknown &&
        !saw_unknown) {
      first_unknown = found->second;
      saw_unknown = true;
    } else if (found->second.verdict == feasibility::Verdict::infeasible) {
      last_infeasible = found->second;
    }
  }

  for (const AllocationSite::ConditionalRelease &release :
       allocation.conditional_release_events) {
    std::vector<edge> constrained_edges;
    constrained_edges.push_back(release.incoming);
    for (edge required : required_edges) {
      // A later use of the same merged SSA value carries the same PHI edge
      // witness.  One traversal satisfies both facts; conditional releases
      // are rejected in loop SCCs, so eliding this duplicate cannot erase a
      // required dynamic loop iteration.
      if (constrained_edges.empty() ||
          constrained_edges.back() != required)
        constrained_edges.push_back(required);
    }
    const feasibility::Result result =
        feasibility::sequence_feasible_with_predicates(
            {allocation.statement, release.statement, statement},
            constrained_edges, required_predicates);
    feasibility::dump_result(result, purpose);
    if (result.verdict == feasibility::Verdict::feasible) {
      if (witness != nullptr)
        *witness = release.statement;
      return result;
    }
    if (result.verdict == feasibility::Verdict::unknown && !saw_unknown) {
      first_unknown = result;
      saw_unknown = true;
    } else if (result.verdict == feasibility::Verdict::infeasible) {
      last_infeasible = result;
    }
  }
  return saw_unknown ? first_unknown : last_infeasible;
}

const char *inconclusive_feasibility_reason(
    const feasibility::Result &result) {
  if (result.reason.find("timeout") != std::string::npos)
    return "path-feasibility-timeout";
  if (result.truncated)
    return "path-feasibility-budget";
  if (result.unsupported_conditions != 0)
    return "unsupported-path-condition";
  if (!feasibility::solver_compiled_in())
    return "solver-unavailable";
  if (!feasibility::default_limits().use_solver)
    return "solver-disabled";
  return "path-feasibility-unknown";
}

bool accept_path_candidate(const PluginOptions &options,
                           report::FindingKind kind, location_t location,
                           const feasibility::Result &result,
                           const AllocationSite *allocation = nullptr) {
  if (!analysis_budget::available()) {
    record_budget_inconclusive(options, location);
    return false;
  }
  if (options.analysis_policy == AnalysisPolicy::proof_only &&
      allocation != nullptr && allocation->inline_asm_ownership_unknown) {
    report::record_inconclusive(kind, location,
                                "inline-asm-ownership-unknown");
    return false;
  }
  if (options.analysis_policy == AnalysisPolicy::proof_only &&
      allocation != nullptr && allocation->opaque_reallocation_status) {
    report::record_inconclusive(
        kind, location, "reallocation-status-unproven",
        "realloc success/failure is not guarded by a direct pointer/null "
        "predicate supported by the lifetime model");
    return false;
  }
  if (result.verdict == feasibility::Verdict::feasible)
    return true;
  if (result.verdict == feasibility::Verdict::infeasible)
    return false;
  if (options.analysis_policy == AnalysisPolicy::exploratory)
    return true;
  report::record_inconclusive(kind, location,
                              inconclusive_feasibility_reason(result),
                              result.reason);
  return false;
}

unsigned int transfer_allocation(const AllocationSite &allocation,
                                 gimple *statement, unsigned int state,
                                 const PluginOptions &options,
                                 FeasibilityCache *feasibility_cache,
                                 edge incoming) {
  if (statement == allocation.statement) {
    if (allocation.released_on_return())
      return state == state_none ? state_none : state_freed;
    if ((state & state_live) != 0 && options.memory_leak) {
      const location_t location = best_source_location({statement});
      if (allocation.expansion.truncated) {
        if (options.analysis_policy == AnalysisPolicy::proof_only)
          report::record_inconclusive(report::FindingKind::memory_leak,
                                      location, "value-flow-truncated");
      } else {
        const feasibility::Result path = feasibility::sequence_feasible(
            {allocation.statement, allocation.statement});
        feasibility::dump_result(path, "repeated-allocation-leak");
        if (accept_path_candidate(options, report::FindingKind::memory_leak,
                                  location, path, &allocation))
          report::emit(report::FindingKind::memory_leak, location,
                       allocation_origin_trace(allocation));
      }
    }
    return state == state_none ? state_none : state_live;
  }

  if (ownership_deallocation_call_p(statement) &&
      gimple_call_num_args(statement) > 0 &&
      (contains_statement(allocation.release_events, statement) ||
       contains_conditional_release(allocation, statement, incoming))) {
    gimple *previous_release = nullptr;
    if (options.double_free && invalidated_state(state)) {
      const feasibility::Result path = release_path_result(
          allocation, statement, "double-free", feasibility_cache,
          &previous_release);
      const location_t location = best_source_location(
          {statement, previous_release, allocation.statement});
      if (accept_path_candidate(options, report::FindingKind::double_free,
                                location, path, &allocation)) {
        std::vector<report::TracePoint> trace = lifetime_release_trace(
            allocation, previous_release, previous_release == statement);
        append_release_origin(&trace, allocation, previous_release);
        report::emit(report::FindingKind::double_free, location, trace);
      }
    }
    unsigned int result = state & ~(state_live | state_reallocated);
    if ((state & (state_live | state_freed | state_escaped |
                  state_reallocated)) != 0)
      result |= state_freed;
    return result;
  }

  if (ownership_reallocation_call_p(statement) &&
      gimple_call_num_args(statement) > 0 &&
      contains_statement(allocation.release_events, statement)) {
    gimple *previous_release = nullptr;
    if (options.use_after_free && invalidated_state(state)) {
      const feasibility::Result path = release_path_result(
          allocation, statement, "realloc-after-free", feasibility_cache,
          &previous_release);
      const location_t location = best_source_location(
          {statement, previous_release, allocation.statement});
      if (accept_path_candidate(options, report::FindingKind::use_after_free,
                                location, path, &allocation)) {
        std::vector<report::TracePoint> trace =
            lifetime_release_trace(allocation, previous_release);
        append_release_origin(&trace, allocation, previous_release);
        report::emit(report::FindingKind::use_after_free, location, trace);
      }
    }
    // Finalize an older realloc split before creating this call's split.  This
    // prevents a NULL test for the current result from reviving an object that
    // an earlier successful realloc had already invalidated.
    if ((state & state_reallocated) != 0) {
      state &= ~state_reallocated;
      state |= state_freed;
    }
    // Failure preserves the old object (state_live); success invalidates it
    // (state_reallocated).  The successor-edge transfer below separates the
    // alternatives when code tests this result against NULL.
    if ((state & state_live) != 0)
      state |= state_reallocated;
    return state;
  }

  if (has_summary_effect(allocation, statement)) {
    const bool may_free =
        options.analysis_policy == AnalysisPolicy::proof_only
            ? summary_effect_definitely_frees(allocation, statement)
            : summary_effect_may_free(allocation, statement);
    const bool may_dereference =
        summary_effect_may_dereference(allocation, statement);
    gimple *previous_release = nullptr;
    if (options.double_free && may_free && invalidated_state(state)) {
      const feasibility::Result path = release_path_result(
          allocation, statement, "interprocedural-double-free",
          feasibility_cache, &previous_release);
      const location_t location = best_source_location(
          {statement, previous_release, allocation.statement});
      if (accept_path_candidate(options, report::FindingKind::double_free,
                                location, path, &allocation)) {
        std::vector<report::TracePoint> trace =
            lifetime_release_trace(allocation, previous_release);
        append_release_origin(&trace, allocation, previous_release);
        append_release_origin(&trace, allocation, statement);
        report::emit(report::FindingKind::double_free, location, trace);
      }
    }
    if (options.use_after_free && may_dereference &&
        invalidated_state(state)) {
      bool has_access_guarantee = false;
      bool emitted = false;
      bool saw_unknown = false;
      feasibility::Result first_unknown;
      gimple *first_unknown_release = nullptr;

      if (options.analysis_policy == AnalysisPolicy::proof_only) {
        for (const AllocationSite::SummaryEffect &item :
             allocation.summary_effects) {
          if (item.statement != statement ||
              !item.effect.has_access_guarantee())
            continue;
          has_access_guarantee = true;
          std::vector<feasibility::RequiredPredicate> predicates;
          if (ownership_summary::access_is_conditional(item.effect.access) ||
              ownership_summary::access_is_live_requirement(item.effect.access)) {
            // free(NULL) followed by an invalid string call is not UAF of a
            // heap object. Prove a nonnull allocation on the same path even
            // when the library's entry precondition itself is unconditional.
            tree predicate_value = item.accessed_value != NULL_TREE ? item.accessed_value :
                nonnull_call_argument_value(statement, item.argument_index, allocation);
            tree zero = predicate_value != NULL_TREE &&
                                TREE_TYPE(predicate_value) != NULL_TREE
                            ? build_zero_cst(TREE_TYPE(predicate_value))
                            : NULL_TREE;
            predicates.push_back(
                {feasibility::ScalarRelation::not_equal, predicate_value, zero});
          }

          gimple *access_release = nullptr;
          const feasibility::Result path = release_path_result(
              allocation, statement, "callee-use-after-free",
              feasibility_cache, &access_release, {}, predicates);
          if (path.verdict == feasibility::Verdict::unknown &&
              !saw_unknown) {
            first_unknown = path;
            first_unknown_release = access_release;
            saw_unknown = true;
            continue;
          }
          if (path.verdict != feasibility::Verdict::feasible)
            continue;
          const location_t location = best_source_location(
              {statement, access_release, allocation.statement});
          if (accept_path_candidate(options,
                                    report::FindingKind::use_after_free,
                                    location, path, &allocation)) {
            const std::string access_role =
                std::string(ownership_summary::access_is_live_requirement(
                                item.effect.access)
                                ? "callee requires a live object by "
                                : "callee access guaranteed by ") +
                ownership_summary::proof_origin_name(
                    item.effect.access_origin);
            std::vector<report::TracePoint> trace =
                lifetime_release_trace(allocation, access_release);
            append_release_origin(&trace, allocation, access_release);
            trace.push_back({best_source_location({statement}), access_role});
            if (LOCATION_LOCUS(item.effect.access_location) != UNKNOWN_LOCATION)
              trace.push_back({item.effect.access_location, "callee accesses the contained pointer"});
            report::emit(report::FindingKind::use_after_free, location, trace);
            emitted = true;
            break;
          }
        }

        if (!emitted && saw_unknown) {
          const location_t location = best_source_location(
              {statement, first_unknown_release, allocation.statement});
          // This call records one precise UNKNOWN reason and cannot emit in
          // proof-only mode; finding traces are reserved for actual proofs.
          accept_path_candidate(options,
                                report::FindingKind::use_after_free,
                                location, first_unknown, &allocation);
        } else if (!emitted && !has_access_guarantee) {
          report::record_inconclusive(
              report::FindingKind::use_after_free,
              best_source_location({statement, allocation.statement}),
              "callee-dereference-not-proven",
              "the summary proves that the callee may dereference this "
              "argument, not that this invocation must dereference it");
        }
      } else {
        const feasibility::Result path = release_path_result(
            allocation, statement, "interprocedural-use-after-free",
            feasibility_cache, &previous_release);
        const location_t location = best_source_location(
            {statement, previous_release, allocation.statement});
        if (accept_path_candidate(options,
                                  report::FindingKind::use_after_free,
                                  location, path, &allocation)) {
          std::vector<report::TracePoint> trace =
              lifetime_release_trace(allocation, previous_release);
          append_release_origin(&trace, allocation, previous_release);
          report::emit(report::FindingKind::use_after_free, location, trace);
        }
      }
    }
    return apply_summary_effects(allocation, statement, state, options);
  }

  // A merged return value only transfers ownership when every PHI arm that
  // can execute after this allocation denotes this exact object.  Merely
  // finding the allocation on one arm is not enough: another reachable arm
  // may return NULL while leaving the object live.
  if (gimple_code(statement) == GIMPLE_RETURN) {
    const greturn *return_statement = as_a<greturn *>(statement);
    tree returned = gimple_return_retval(return_statement);
    if (same_owned_object_after_allocation(returned, allocation) ||
        contains_conditional_return(allocation, statement, incoming)) {
      // Returning an exact pointer whose allocation has already been
      // invalidated exports a dangling value.  Treat this as a lifetime use,
      // but retain the same allocation/release/return feasibility proof used
      // for ordinary dereferences so mutually exclusive branches and loop
      // epochs cannot manufacture a diagnostic.
      if (options.use_after_free && invalidated_state(state)) {
        const auto found = allocation.uses.find(statement);
        feasibility::Result first_unknown;
        gimple *first_unknown_release = nullptr;
        const UseSite *first_unknown_site = nullptr;
        bool saw_unknown = false;
        bool emitted = false;
        if (found != allocation.uses.end()) {
          for (const UseSite &site : found->second) {
            if (site.edge != FlowEdgeKind::return_value ||
                !site_proves_allocation(site, allocation))
              continue;
            gimple *previous_release = nullptr;
            const feasibility::Result path = release_path_result(
                allocation, statement, "return-after-free",
                feasibility_cache, &previous_release, site.required_edges);
            if (path.verdict == feasibility::Verdict::unknown &&
                !saw_unknown) {
              first_unknown = path;
              first_unknown_release = previous_release;
              first_unknown_site = &site;
              saw_unknown = true;
              continue;
            }
            if (path.verdict != feasibility::Verdict::feasible)
              continue;
            location_t location =
                return_provenance_location(allocation, statement, site);
            if (!source_location_p(location))
              location = best_source_location(
                  {statement, previous_release, allocation.statement});
            if (accept_path_candidate(options,
                                      report::FindingKind::use_after_free,
                                      location, path, &allocation)) {
              std::vector<report::TracePoint> trace =
                  lifetime_release_trace(allocation, previous_release);
              append_release_origin(&trace, allocation, previous_release);
              trace.push_back({location, "dangling pointer returned"});
              report::emit(report::FindingKind::use_after_free, location, trace);
              emitted = true;
              break;
            }
          }
        }
        // UNKNOWN is reported once, and only after every exact return-edge
        // witness failed to produce a feasible proof.  This avoids attaching
        // an inconclusive record to one PHI arm when another arm proves the
        // diagnostic.
        if (!emitted && saw_unknown && first_unknown_site != nullptr) {
          location_t location = return_provenance_location(
              allocation, statement, *first_unknown_site);
          if (!source_location_p(location))
            location = best_source_location(
                {statement, first_unknown_release, allocation.statement});
          if (accept_path_candidate(options,
                                    report::FindingKind::use_after_free,
                                    location, first_unknown, &allocation)) {
            std::vector<report::TracePoint> trace =
                lifetime_release_trace(allocation, first_unknown_release);
            append_release_origin(&trace, allocation, first_unknown_release);
            trace.push_back({location, "dangling pointer returned"});
            report::emit(report::FindingKind::use_after_free, location, trace);
          }
        }
      }
      unsigned int result = state & ~state_live;
      if ((state & state_live) != 0)
        result |= state_escaped;
      return result;
    }
    return state;
  }

  if (pointer_store_escape(
          allocation, statement,
          options.analysis_policy == AnalysisPolicy::exploratory)) {
    unsigned int result = state & ~state_live;
    if ((state & state_live) != 0)
      result |= state_escaped;
    return result;
  }

  const bool mentioned =
      statement_mentions_allocation(allocation, statement, true);
  if (!mentioned)
    return state;

  if (options.use_after_free && invalidated_state(state)) {
    const auto found = allocation.uses.find(statement);
    for (const UseSite &site : found->second) {
      const bool call_argument_use =
          is_gimple_call(statement) &&
          !ownership_deallocation_call_p(statement) &&
          site.edge == FlowEdgeKind::call_argument;
      const bool lifetime_use =
          (call_argument_use &&
           options.analysis_policy == AnalysisPolicy::exploratory &&
           call_use_may_dereference(statement, site.incoming_value)) ||
          site.edge == FlowEdgeKind::memory_reference;
      if (!site_proves_allocation(site, allocation))
        continue;

      if (!lifetime_use) {
        if (call_argument_use &&
            options.analysis_policy == AnalysisPolicy::proof_only) {
          const bool summarized_may_dereference =
              call_use_has_resolved_may_dereference(statement,
                                                    site.incoming_value);
          report::record_inconclusive(
              report::FindingKind::use_after_free,
              best_source_location({statement, allocation.statement}),
              summarized_may_dereference
                  ? "callee-dereference-not-proven"
                  : "opaque-call-dereference",
              summarized_may_dereference
                  ? "the callee may dereference this derived or merged "
                    "allocation pointer, but this invocation is not proved "
                    "to dereference it"
                  : "the call may dereference this allocation pointer, but "
                    "no body-bound must-dereference proof is available");
        }
        continue;
      }

      gimple *previous_release = nullptr;
      const feasibility::Result path = release_path_result(
          allocation, statement, "use-after-free", feasibility_cache,
          &previous_release, site.required_edges);
      const location_t location = best_source_location(
          {statement, previous_release, allocation.statement});
      if (!accept_path_candidate(options, report::FindingKind::use_after_free,
                                 location, path, &allocation))
        continue;
      std::vector<report::TracePoint> trace =
          lifetime_release_trace(allocation, previous_release);
      append_release_origin(&trace, allocation, previous_release);
      report::emit(report::FindingKind::use_after_free, location, trace);
      break;
    }
  }
  return state;
}

unsigned int refine_allocation_edge(const AllocationSite &allocation,
                                    basic_block block, edge successor,
                                    unsigned int state) {
  if ((state & (state_live | state_freed | state_escaped |
                state_reallocated)) == 0)
    return state;
  gimple_stmt_iterator last = gsi_last_bb(block);
  if (gsi_end_p(last))
    return state;
  gimple *condition = gsi_stmt(last);
  if (gimple_code(condition) != GIMPLE_COND)
    return state;

  const enum tree_code code = gimple_cond_code(condition);
  if (code != EQ_EXPR && code != NE_EXPR)
    return state;
  tree left = gimple_cond_lhs(condition);
  tree right = gimple_cond_rhs(condition);
  tree compared = NULL_TREE;
  if (left != NULL_TREE && TREE_CODE(left) == INTEGER_CST &&
      integer_zerop(left))
    compared = right;
  else if (right != NULL_TREE && TREE_CODE(right) == INTEGER_CST &&
           integer_zerop(right))
    compared = left;
  if (compared == NULL_TREE)
    return state;

  const bool condition_true_when_null = code == EQ_EXPR;
  const bool true_edge = (successor->flags & EDGE_TRUE_VALUE) != 0;
  const bool false_edge = (successor->flags & EDGE_FALSE_VALUE) != 0;
  if (!true_edge && !false_edge)
    return state;

  const bool null_edge =
      (true_edge && condition_true_when_null) ||
      (false_edge && !condition_true_when_null);

  if (same_allocation(compared, allocation.value) ||
      same_returned_object_origin(compared, allocation, false)) {
    if (!null_edge)
      return state;

    // The abstract live/freed/escaped bits describe an allocation that really
    // exists.  On the null result edge there is no object whose lifetime can
    // be leaked or released.  This also removes the synthetic null arm GCC
    // emits before lowered C++ delete calls.
    return state & ~(state_live | state_freed | state_escaped |
                     state_reallocated);
  }

  // For the allocation passed to realloc, split success from failure using
  // the result's defining GIMPLE call.  Multiple distinct realloc sites for
  // one object are deliberately left merged: one anonymous pending bit could
  // not identify which call a later condition refines.
  const gimple *reallocation = single_reallocation_origin(compared);
  if (reallocation == nullptr || reallocation_event_count(allocation) != 1 ||
      gimple_call_num_args(reallocation) == 0 ||
      !same_owned_object(gimple_call_arg(reallocation, 0), allocation.value))
    return state;

  if (null_edge)
    return state & ~state_reallocated;
  return state & ~state_live;
}

struct SolverPoint {
  basic_block block = nullptr;
  edge incoming = nullptr;

  bool operator<(const SolverPoint &other) const {
    if (block != other.block)
      return std::less<basic_block>()(block, other.block);
    return std::less<edge>()(incoming, other.incoming);
  }
};

template <typename Transfer, typename EdgeTransfer>
unsigned int solve_cfg(unsigned int initial_state, bool preserve_phi_edges,
                       Transfer transfer,
                       EdgeTransfer edge_transfer,
                       unsigned long *iterations, unsigned long *merges,
                       bool *complete) {
  *complete = true;
  // Allocations with a conditional PHI release retain one finite abstract
  // state per CFG edge.  All other allocations and thread handles keep the
  // cheaper block state.  Both converge by monotone bit-union: at most one
  // state grows per static edge (or block), so neither enumerates paths.
  const std::size_t block_slots =
      static_cast<std::size_t>(last_basic_block_for_fn(cfun));
  const std::size_t live_blocks =
      static_cast<std::size_t>(n_basic_blocks_for_fn(cfun));
  constexpr std::size_t max_dense_block_slots = 262144;
  const bool dense_block_indices =
      block_slots <= max_dense_block_slots &&
      block_slots <= live_blocks * 8u + 256u;
  if (!preserve_phi_edges && dense_block_indices) {
    // Basic-block indices are dense enough for GCC's own CFG vectors and are
    // stable for the duration of this read-only pass.  The ordinary case
    // therefore needs neither tree comparisons nor one heap node per lookup.
    // Keep the edge-keyed map below only for the uncommon conditional-PHI
    // release case where the incoming edge is part of the abstract state.
    std::vector<unsigned int> input(block_slots, 0);
    std::deque<basic_block> worklist;
    basic_block entry = ENTRY_BLOCK_PTR_FOR_FN(cfun);
    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, entry->succs) {
      const int index = successor->dest->index;
      if (index < 0 || static_cast<unsigned int>(index) >= input.size()) {
        *complete = false;
        return 0;
      }
      input[index] |= initial_state;
      worklist.push_back(successor->dest);
    }

    unsigned int exit_state = 0;
    while (!worklist.empty()) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::lifetime_cfg)) {
        *complete = false;
        return 0;
      }
      basic_block block = worklist.front();
      worklist.pop_front();
      const int index = block->index;
      if (index < 0 || static_cast<unsigned int>(index) >= input.size()) {
        *complete = false;
        return 0;
      }
      if (block == EXIT_BLOCK_PTR_FOR_FN(cfun)) {
        exit_state |= input[index];
        continue;
      }

      ++*iterations;
      unsigned int output = input[index];
      for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
           gsi_next(&gsi)) {
        if (!analysis_budget::consume(
                analysis_budget::WorkKind::lifetime_cfg)) {
          *complete = false;
          return 0;
        }
        output = transfer(gsi_stmt(gsi), output, nullptr);
      }

      FOR_EACH_EDGE(successor, iterator, block->succs) {
        const int destination = successor->dest->index;
        if (destination < 0 ||
            static_cast<unsigned int>(destination) >= input.size()) {
          *complete = false;
          return 0;
        }
        const unsigned int previous = input[destination];
        const unsigned int propagated =
            edge_transfer(block, successor, output);
        const unsigned int combined = previous | propagated;
        if (combined == previous)
          continue;
        if (previous != 0)
          ++*merges;
        input[destination] = combined;
        worklist.push_back(successor->dest);
      }
    }
    return exit_state;
  }

  std::map<SolverPoint, unsigned int> input;
  std::deque<SolverPoint> worklist;
  basic_block entry = ENTRY_BLOCK_PTR_FOR_FN(cfun);
  edge successor;
  edge_iterator iterator;
  FOR_EACH_EDGE(successor, iterator, entry->succs) {
    const SolverPoint point{successor->dest,
                            preserve_phi_edges ? successor : nullptr};
    input[point] |= initial_state;
    worklist.push_back(point);
  }

  unsigned int exit_state = 0;
  while (!worklist.empty()) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::lifetime_cfg)) {
      *complete = false;
      return 0;
    }
    const SolverPoint point = worklist.front();
    worklist.pop_front();
    basic_block block = point.block;
    if (block == EXIT_BLOCK_PTR_FOR_FN(cfun)) {
      exit_state |= input[point];
      continue;
    }

    ++*iterations;
    unsigned int output = input[point];
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::lifetime_cfg)) {
        *complete = false;
        return 0;
      }
      output = transfer(gsi_stmt(gsi), output, point.incoming);
    }

    FOR_EACH_EDGE(successor, iterator, block->succs) {
      const SolverPoint destination{
          successor->dest, preserve_phi_edges ? successor : nullptr};
      const unsigned int previous = input[destination];
      const unsigned int propagated =
          edge_transfer(block, successor, output);
      const unsigned int combined = previous | propagated;
      if (combined == previous)
        continue;
      if (previous != 0)
        ++*merges;
      input[destination] = combined;
      worklist.push_back(destination);
    }
  }
  return exit_state;
}

void analyze_allocation(const AllocationSite &allocation,
                        const PluginOptions &options,
                        unsigned long *iterations, unsigned long *merges) {
  if (options.analysis_policy == AnalysisPolicy::proof_only &&
      allocation.opaque_reallocation_status) {
    const location_t location =
        best_source_location({allocation.statement});
    if (options.memory_leak)
      report::record_inconclusive(
          report::FindingKind::memory_leak, location,
          "reallocation-status-unproven",
          "realloc success/failure is not guarded by a direct pointer/null "
          "predicate supported by the lifetime model");
    if (options.use_after_free)
      report::record_inconclusive(
          report::FindingKind::use_after_free, location,
          "reallocation-status-unproven",
          "old-object invalidation cannot be correlated with realloc "
          "success");
  }
  if (options.analysis_policy == AnalysisPolicy::proof_only &&
      options.use_after_free && allocation.expansion.witnesses_truncated)
    report::record_inconclusive(
        report::FindingKind::use_after_free,
        best_source_location({allocation.statement}),
        "value-flow-witnesses-truncated");
  if (options.analysis_policy == AnalysisPolicy::proof_only &&
      allocation.inline_asm_ownership_unknown) {
    const location_t location =
        best_source_location({allocation.statement});
    if (options.memory_leak)
      report::record_inconclusive(report::FindingKind::memory_leak,
                                  location, "inline-asm-ownership-unknown");
    if (options.use_after_free)
      report::record_inconclusive(report::FindingKind::use_after_free,
                                  location, "inline-asm-ownership-unknown");
  }
  FeasibilityCache feasibility_cache;
  bool complete = true;
  const unsigned int exit_state = solve_cfg(
      state_unseen, !allocation.conditional_release_events.empty() ||
                        !allocation.conditional_return_events.empty(),
      [&](gimple *statement, unsigned int state, edge incoming) {
        return transfer_allocation(allocation, statement, state, options,
                                   &feasibility_cache, incoming);
      },
      [&](basic_block block, edge successor, unsigned int state) {
        return refine_allocation_edge(allocation, block, successor, state);
      },
      iterations, merges, &complete);
  if (!complete) {
    record_budget_inconclusive(
        options, best_source_location({allocation.statement}));
    return;
  }
  // Exhausting the bounded value-flow walk means a release may exist beyond
  // the explored graph.  That is unknown, not proof of a leak.
  if (options.memory_leak && (exit_state & state_live) != 0) {
    const location_t location =
        best_source_location({allocation.statement});
    if (allocation.expansion.truncated) {
      if (options.analysis_policy == AnalysisPolicy::proof_only)
        report::record_inconclusive(report::FindingKind::memory_leak,
                                    location, "value-flow-truncated");
      return;
    }
    if (options.analysis_policy == AnalysisPolicy::proof_only &&
        allocation.inline_asm_ownership_unknown)
      return;
    if (options.analysis_policy == AnalysisPolicy::proof_only &&
        allocation.opaque_ownership_transfer) {
      report::record_inconclusive(
          report::FindingKind::memory_leak, location,
          "opaque-ownership-transfer",
          "allocated pointer reaches a return, cast, store, local-memory spill, or "
          "call without a complete ownership-transfer proof",
          allocation.opaque_ownership_call == nullptr
              ? std::vector<report::TracePoint>()
              : std::vector<report::TracePoint>{
                    {best_source_location({allocation.opaque_ownership_call}),
                     "ownership barrier requiring a verified effect"}});
      return;
    }
    // A call that returned NULL did not create an ownership obligation.
    // The symbolic leak witness must exclude these same failure edges that
    // the finite typestate transfer removes, or a joined live state could
    // borrow an unrelated allocation-failure path as its SAT witness.
    std::vector<edge> terminal_edges = allocation.allocation_failure_edges;
    for (const AllocationSite::ConditionalRelease &event :
         allocation.conditional_return_events)
      terminal_edges.push_back(event.incoming);
    for (const AllocationSite::ConditionalRelease &event :
         allocation.conditional_release_events)
      terminal_edges.push_back(event.incoming);
    std::vector<gimple *> release_barriers = allocation.definite_terminal_events;
    release_barriers.insert(release_barriers.end(),
                            allocation.uncertain_release_events.begin(),
                            allocation.uncertain_release_events.end());
    release_barriers.insert(release_barriers.end(),
                            allocation.ambiguous_ownership_events.begin(),
                            allocation.ambiguous_ownership_events.end());
    feasibility::Result leak_path = feasibility::path_to_exit_avoiding(
        allocation.statement, release_barriers, terminal_edges);
    bool used_borrow_conditions = false;
    if (options.analysis_policy == AnalysisPolicy::proof_only &&
        leak_path.verdict != feasibility::Verdict::feasible &&
        !allocation.guarded_borrows.empty()) {
      std::vector<gimple *> guarded_barriers = release_barriers;
      std::vector<feasibility::RequiredPredicate> predicates;
      for (const AllocationSite::GuardedBorrow &borrow : allocation.guarded_borrows) {
        guarded_barriers.erase(std::remove(guarded_barriers.begin(), guarded_barriers.end(),
                                           borrow.statement), guarded_barriers.end());
        predicates.push_back({borrow.condition.when_zero ? feasibility::ScalarRelation::equal :
                                                          feasibility::ScalarRelation::not_equal,
                              borrow.condition.selector, build_zero_cst(TREE_TYPE(borrow.condition.selector))});
      }
      const feasibility::Result guarded_path = feasibility::path_to_exit_avoiding_with_predicates(
          allocation.statement, guarded_barriers, terminal_edges, predicates, feasibility::default_limits());
      if (guarded_path.verdict == feasibility::Verdict::feasible) {
        leak_path = guarded_path;
        used_borrow_conditions = true;
      }
    }
    feasibility::dump_result(leak_path, "memory-leak");
    if (options.analysis_policy == AnalysisPolicy::proof_only &&
        leak_path.verdict != feasibility::Verdict::feasible &&
        !allocation.ambiguous_ownership_events.empty()) {
      std::vector<report::TracePoint> related;
      for (gimple *event : allocation.ambiguous_ownership_events) {
        if (related.size() >= 8)
          break;
        related.push_back({best_source_location({event}),
                           "call with unresolved ownership outcomes"});
      }
      report::record_inconclusive(
          report::FindingKind::memory_leak, location,
          "ambiguous-callee-ownership",
          "no proven leak path avoids every body-bound call with "
          "unresolved or mixed ownership outcomes", related);
      return;
    }
    if (options.analysis_policy == AnalysisPolicy::proof_only &&
        leak_path.verdict != feasibility::Verdict::feasible &&
        !allocation.uncertain_release_events.empty()) {
      report::record_inconclusive(
          report::FindingKind::memory_leak, location,
          "possible-release-alias-unproven",
          "no proven leak path avoids every possibly aliasing release; "
          "missing exact release or allocation-epoch evidence is not safety");
      return;
    }
    if (accept_path_candidate(options, report::FindingKind::memory_leak,
                              location, leak_path, &allocation)) {
      std::vector<report::TracePoint> trace = allocation_origin_trace(allocation);
      if (trace.empty())
        trace.push_back({location, "allocation"});
      if (used_borrow_conditions) for (const auto &borrow : allocation.guarded_borrows) {
        if (trace.size() >= 10)
          break;
        trace.push_back({borrow.condition.guard_location,
                         "caller constraint proves the callee borrow branch whenever invoked"});
      }
      for (const AllocationSite::SummaryEffect &item : allocation.summary_effects)
        if (item.effect.resolved) {
          if (item.effect.ownership_origin ==
              ownership_summary::ProofOrigin::user_contract)
            trace.push_back({best_source_location({item.statement}),
                             "ownership preserved by user-contract"});
          else if (item.effect.ownership_origin ==
                   ownership_summary::ProofOrigin::provider_body)
            trace.push_back({best_source_location({item.statement}),
                             "ownership preserved by provider-body"});
        }
      report::emit(report::FindingKind::memory_leak, location,
                   trace);
    }
  }
}

tree strip_address(tree value) {
  if (value != NULL_TREE && TREE_CODE(value) == ADDR_EXPR)
    return TREE_OPERAND(value, 0);
  return value;
}

void scalar_origins(tree value, unsigned int depth, std::set<tree> *active,
                    std::set<tree> *origins, bool *complete) {
  if (value == NULL_TREE || depth == 0) {
    *complete = false;
    return;
  }
  value = strip_address(value);
  if (TREE_CODE(value) == VAR_DECL || TREE_CODE(value) == PARM_DECL) {
    origins->insert(value);
    return;
  }
  if (TREE_CODE(value) != SSA_NAME || !active->insert(value).second) {
    *complete = false;
    return;
  }
  gimple *definition = SSA_NAME_DEF_STMT(value);
  if (definition == nullptr || gimple_code(definition) == GIMPLE_NOP) {
    *complete = false;
  } else if (gimple_code(definition) == GIMPLE_PHI) {
    for (unsigned int index = 0; index < gimple_phi_num_args(definition);
         ++index)
      scalar_origins(gimple_phi_arg_def(definition, index), depth - 1, active,
                     origins, complete);
  } else if (is_gimple_assign(definition) &&
             gimple_assign_single_p(definition)) {
    scalar_origins(gimple_assign_rhs1(definition), depth - 1, active, origins,
                   complete);
  } else {
    *complete = false;
  }
  active->erase(value);
}

bool proven_thread_handle(tree value, tree storage) {
  std::set<tree> active;
  std::set<tree> origins;
  bool complete = true;
  scalar_origins(value, 32, &active, &origins, &complete);
  return complete && origins.size() == 1 && *origins.begin() == storage;
}

bool thread_release(gimple *statement, tree storage) {
  if ((!named(statement, "pthread_join") &&
       !named(statement, "pthread_detach")) ||
      gimple_call_num_args(statement) == 0)
    return false;
  return proven_thread_handle(gimple_call_arg(statement, 0), storage);
}

unsigned int transfer_thread(const ThreadSite &site, gimple *statement,
                             unsigned int state) {
  if (statement == site.statement)
    return state == thread_none ? thread_none : thread_outstanding;
  if (!thread_release(statement, site.storage))
    return state;
  unsigned int result = state & ~thread_outstanding;
  if ((state & (thread_outstanding | thread_released)) != 0)
    result |= thread_released;
  return result;
}

void analyze_thread(const ThreadSite &site, const PluginOptions &options,
                    unsigned long *iterations, unsigned long *merges) {
  bool complete = true;
  const unsigned int exit_state = solve_cfg(
      thread_unseen, false,
      [&](gimple *statement, unsigned int state, edge) {
        return transfer_thread(site, statement, state);
      },
      [](basic_block, edge, unsigned int state) { return state; },
      iterations, merges, &complete);
  if (!complete) {
    record_budget_inconclusive(options,
                               best_source_location({site.statement}));
    return;
  }
  if ((exit_state & thread_outstanding) != 0) {
    const location_t location = best_source_location({site.statement});
    if (options.analysis_policy == AnalysisPolicy::proof_only) {
      report::record_inconclusive(
          report::FindingKind::joinable_thread_leak, location,
          "thread-create-success-unproven",
          "pthread_create return status is not part of the current "
          "ownership proof model");
      return;
    }
    std::vector<gimple *> releases;
    basic_block block;
    FOR_EACH_BB_FN(block, cfun) {
      for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
           gsi_next(&gsi)) {
        gimple *statement = gsi_stmt(gsi);
        if (thread_release(statement, site.storage))
          releases.push_back(statement);
      }
    }
    const feasibility::Result path = feasibility::path_to_exit_avoiding(
        site.statement, releases);
    feasibility::dump_result(path, "joinable-thread-leak");
    if (accept_path_candidate(options,
                              report::FindingKind::joinable_thread_leak,
                              location, path))
      report::emit(report::FindingKind::joinable_thread_leak, location);
  }
}

void dump_engine_summary(const char *function_name, unsigned int allocations,
                         unsigned int threads, unsigned long iterations,
                         unsigned long merges) {
  if (dump_file == nullptr)
    return;
  dump_printf(MSG_NOTE,
              "bounded-path summary: function=%s allocations=%u threads=%u "
              "block-iterations=%lu merges=%lu state-bits=5\n",
              function_name == nullptr ? "<anonymous>" : function_name,
              allocations, threads, iterations, merges);
}

void analyze_function(const PluginOptions &options) {
  alias::reset_function_cache();
  memory_value::reset_function_cache();
  reset_value_expansion_cache();
  if (options.memory_leak && options.ffmpeg_library_model)
    resource_protocol::analyze_current_function();
  std::vector<AllocationSite> allocations;
  std::vector<ThreadSite> threads;
  bool local_confinement_checked = false;
  bool local_confinement_proved = false;
  basic_block block;
  FOR_EACH_BB_FN(block, cfun) {
    for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
         gsi_next(&gsi)) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::lifetime_discovery)) {
        record_budget_inconclusive(
            options, best_source_location({gsi_stmt(gsi)}));
        return;
      }
      gimple *statement = gsi_stmt(gsi);
      if (options.memory_leak || options.double_free ||
          options.use_after_free) {
        const bool intrinsic_allocation =
            ownership_allocation_call_p(statement);
        const bool summary_may_allocate =
            !intrinsic_allocation && is_gimple_call(statement) &&
            pointer_value(gimple_call_lhs(statement)) &&
            ownership_summary::call_returns_fresh_owned(statement);
        const bool summary_allocation =
            summary_may_allocate &&
            (options.analysis_policy == AnalysisPolicy::exploratory ||
             ownership_summary::call_returns_definitely_fresh_owned(
                 statement));
        const bool fresh_allocation =
            intrinsic_allocation || summary_allocation;
        const bool reallocation =
            ownership_reallocation_call_p(statement);
        ownership_summary::ReturnedObjectEffect returned_object;
        if (!fresh_allocation && !reallocation &&
            (options.double_free || options.use_after_free) &&
            is_gimple_call(statement) &&
            pointer_value(gimple_call_lhs(statement)))
          returned_object = ownership_summary::returned_object_effect(statement);
        const bool returned_released = returned_object.resolved &&
            !returned_object.may_be_null && returned_object.lifetime ==
                ownership_summary::ReturnedObjectLifetime::released;
        if ((fresh_allocation || reallocation || returned_released) &&
            pointer_value(gimple_call_lhs(statement))) {
          AllocationSite site;
          site.statement = statement;
          site.value = gimple_call_lhs(statement);
          site.returned_object = returned_object;
          site.summarized_origins =
              ownership_summary::call_fresh_origin_locations(statement);
          site.expansion = expand_immediate_uses(site.value);
          if (!analysis_budget::consume(
                  analysis_budget::WorkKind::lifetime_discovery,
                  site.expansion.sites.size() + 1)) {
            record_budget_inconclusive(
                options, best_source_location({statement}));
            return;
          }
          site.opaque_reallocation_status =
              reallocation &&
              !reallocation_result_has_direct_null_guard(statement,
                                                         &site.expansion);
          for (const UseSite &use : site.expansion.sites)
            site.uses[use.statement].push_back(use);
          allocations.push_back(site);
        } else if (options.analysis_policy == AnalysisPolicy::proof_only &&
                   options.memory_leak && summary_may_allocate) {
          report::record_inconclusive(
              report::FindingKind::memory_leak,
              best_source_location({statement}),
              "conditional-summary-allocation",
              "callee return alternatives include NULL and are not "
              "specialized to this invocation");
        } else if (options.analysis_policy == AnalysisPolicy::proof_only &&
                   options.memory_leak &&
                   ownership_unproved_allocation_call_p(statement)) {
          report::record_inconclusive(
              report::FindingKind::memory_leak,
              best_source_location({statement}),
              "allocator-release-contract-unproven",
              "fresh/no-alias storage metadata does not identify a "
              "compatible ownership release contract");
        }
      }
      if (options.thread && named(statement, "pthread_create") &&
          gimple_call_num_args(statement) >= 2) {
        tree storage = strip_address(gimple_call_arg(statement, 0));
        tree attributes = gimple_call_arg(statement, 1);
        // A null attribute is definitely joinable.  Non-null attributes need
        // a separate attr-state proof before a high-confidence report.
        if (storage != NULL_TREE && attributes != NULL_TREE &&
            TREE_CODE(attributes) == INTEGER_CST && integer_zerop(attributes))
          threads.push_back({statement, storage});
      }
    }
  }

  // Resolve each pointer-valued memory load once per function.  Reusing these
  // bridges across allocation sites avoids repeating a virtual-SSA walk for
  // every allocation/load pair.
  const std::vector<MemoryLoadBridge> memory_load_bridges =
      allocations.empty() ? std::vector<MemoryLoadBridge>{}
                          : collect_memory_load_bridges();
  const std::vector<MemoryReleaseBridge> memory_release_bridges =
      allocations.empty() ? std::vector<MemoryReleaseBridge>{}
                          : collect_memory_release_bridges();
  const std::vector<MemoryAccessBridge> memory_access_bridges =
      allocations.empty() || !options.use_after_free ? std::vector<MemoryAccessBridge>{}
                                                   : collect_memory_access_bridges();
  if (!analysis_budget::available()) {
    record_budget_inconclusive(options,
                               best_source_location({}));
    return;
  }

  std::map<const gimple *, const UseExpansion *> allocation_expansions;
  for (const AllocationSite &allocation : allocations)
    allocation_expansions[allocation.statement] = &allocation.expansion;

  // Record every free/realloc that the ownership-grade alias oracle proves
  // belongs to each allocation.  These finite event sets replace path
  // enumeration and are refined only when a diagnostic candidate appears.
  for (AllocationSite &allocation : allocations) {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::lifetime_allocation_statement,
            memory_load_bridges.size() + 1)) {
      record_budget_inconclusive(
          options, best_source_location({allocation.statement}));
      return;
    }
    add_memory_load_uses(&allocation, memory_load_bridges);
    for (const MemoryAccessBridge &bridge : memory_access_bridges) {
      if (!analysis_budget::consume(analysis_budget::WorkKind::lifetime_allocation_statement)) {
        record_budget_inconclusive(options, best_source_location({allocation.statement}));
        return;
      }
      if (!same_owned_object_after_allocation(bridge.stored_value, allocation))
        continue;
      ownership_summary::ArgumentEffect effect;
      effect.may_dereference = true;
      effect.access = bridge.access;
      effect.access_origin = ownership_summary::ProofOrigin::body;
      effect.access_location = bridge.access_location;
      // This is an access theorem only. It makes no claim that the callee
      // preserves, captures or releases the object after that access.
      allocation.summary_effects.push_back({bridge.statement, 0, effect, bridge.stored_value});
    }
    for (const MemoryReleaseBridge &bridge : memory_release_bridges) {
      if (!analysis_budget::consume(
              analysis_budget::WorkKind::lifetime_allocation_statement)) {
        record_budget_inconclusive(
            options, best_source_location({allocation.statement}));
        return;
      }
      if (same_owned_object_after_allocation(bridge.stored_value, allocation)) {
        ownership_summary::ArgumentEffect effect;
        effect.resolved = true;
        effect.fates = ownership_summary::fate_freed;
        effect.may_dereference = false;
        effect.ownership_origin = ownership_summary::ProofOrigin::body;
        effect.release_origin = {bridge.release_location, true};
        // This effect concerns the pointer stored in a memory cell, never
        // the pointer/aggregate actual itself. No argument-level access
        // guarantee is inferred from releasing that contained object.
        allocation.summary_effects.push_back(
            {bridge.statement, 0, effect});
        if (!contains_statement(allocation.release_events, bridge.statement))
          allocation.release_events.push_back(bridge.statement);
        if (!contains_statement(allocation.definite_terminal_events,
                                bridge.statement))
          allocation.definite_terminal_events.push_back(bridge.statement);
      } else if (alias::may_alias(bridge.stored_value, allocation.value)) {
        allocation.uncertain_release_events.push_back(bridge.statement);
      }
    }
    dump_use_expansion(allocation.value, allocation.expansion);
    FOR_EACH_BB_FN(block, cfun) {
      for (gimple_stmt_iterator gsi = gsi_start_bb(block); !gsi_end_p(gsi);
           gsi_next(&gsi)) {
        if (!analysis_budget::consume(
                analysis_budget::WorkKind::lifetime_allocation_statement)) {
          record_budget_inconclusive(
              options, best_source_location({allocation.statement}));
          return;
        }
        gimple *statement = gsi_stmt(gsi);
        const bool deallocation =
            ownership_deallocation_call_p(statement);
        if (gimple_code(statement) == GIMPLE_COND &&
            (gimple_cond_code(statement) == EQ_EXPR ||
             gimple_cond_code(statement) == NE_EXPR)) {
          tree compared = gimple_cond_lhs(statement);
          tree zero = gimple_cond_rhs(statement);
          if (TREE_CODE(compared) == INTEGER_CST && integer_zerop(compared))
            std::swap(compared, zero);
          if (TREE_CODE(zero) == INTEGER_CST && integer_zerop(zero) &&
              same_owned_object(compared, allocation.value)) {
            edge successor;
            edge_iterator iterator;
            const int failure_flag = gimple_cond_code(statement) == EQ_EXPR
                                         ? EDGE_TRUE_VALUE : EDGE_FALSE_VALUE;
            FOR_EACH_EDGE(successor, iterator, block->succs)
              if ((successor->flags & failure_flag) != 0)
                allocation.allocation_failure_edges.push_back(successor);
          }
        }
        const bool reallocation =
            ownership_reallocation_call_p(statement);
        if (options.analysis_policy == AnalysisPolicy::proof_only &&
            gimple_code(statement) == GIMPLE_ASM) {
          const gasm *assembly = as_a<gasm *>(statement);
          if (gimple_asm_clobbers_memory_p(assembly) ||
              statement_mentions_allocation(allocation, statement, true))
            allocation.inline_asm_ownership_unknown = true;
        }
        if ((deallocation || reallocation) &&
            gimple_call_num_args(statement) > 0) {
          if (deallocation)
            report::record_callee_effect(statement, 0, "freed", false, false);
          if (same_owned_object_after_allocation(
                  gimple_call_arg(statement, 0), allocation)) {
            allocation.release_events.push_back(statement);
            if (deallocation)
              allocation.definite_terminal_events.push_back(statement);
            if (reallocation &&
                !reallocation_result_has_direct_null_guard(
                    statement,
                    allocation_expansions.count(statement) == 0
                        ? nullptr
                        : allocation_expansions.find(statement)->second))
              allocation.opaque_reallocation_status = true;
          } else {
            const bool conditional_relation_complete =
                deallocation &&
                add_conditional_phi_releases(&allocation, statement);
            if (!conditional_relation_complete &&
                alias::may_alias(gimple_call_arg(statement, 0),
                                 allocation.value))
              allocation.uncertain_release_events.push_back(statement);
          }
        }

        if (is_gimple_call(statement) && !deallocation && !reallocation) {
          for (unsigned int argument_index = 0;
               argument_index < gimple_call_num_args(statement);
               ++argument_index) {
            if (!pointer_value(
                    gimple_call_arg(statement, argument_index)))
              continue;
            const ownership_summary::ArgumentEffect effect =
                ownership_summary::argument_effect(statement,
                                                   argument_index);
            if (dump_file != nullptr && (dump_flags & TDF_DETAILS)) {
              dump_printf(MSG_NOTE,
                          "call-effect: argument=%u resolved=%s fates=%s "
                          "ownership-origin=%s access=%s access-origin=%s\n",
                          argument_index, effect.resolved ? "yes" : "no",
                          ownership_summary::fate_set_name(effect.fates),
                          ownership_summary::proof_origin_name(effect.ownership_origin),
                          ownership_summary::access_guarantee_name(effect.access),
                          ownership_summary::proof_origin_name(effect.access_origin));
              dump_gimple_stmt(MSG_NOTE, TDF_SLIM, statement, 0);
            }
            if (!effect.resolved)
              allocation.unresolved_call_present = true;
            const bool exact_object = same_owned_object_after_allocation(
                gimple_call_arg(statement, argument_index), allocation);
            if (!exact_object) {
              if (options.analysis_policy == AnalysisPolicy::proof_only &&
                  alias::may_share_allocation_origin(
                      gimple_call_arg(statement, argument_index),
                      allocation.value)) {
                // Ownership is an argument-level effect.  A different
                // pointer operand in the same multi-argument call is not
                // evidence that this operand may denote the allocation.
                // A statement-wide use test here made calls such as
                // memcpy(owned, literal, n) classify the unrelated literal
                // as an opaque transfer of owned.
                allocation.opaque_ownership_transfer = true;
                if (allocation.opaque_ownership_call == nullptr)
                  allocation.opaque_ownership_call = statement;
              }
              continue;
            }
            if (!effect.resolved) {
              if (options.analysis_policy == AnalysisPolicy::proof_only) {
                allocation.opaque_ownership_transfer = true;
                if (allocation.opaque_ownership_call == nullptr)
                  allocation.opaque_ownership_call = statement;
              }
              // Access and ownership are independent dimensions.  A
              // must-access-only contract can prove UAF at this call while
              // its missing borrow/consume contract correctly leaves leak
              // ownership unknown.
              if (effect.has_access_guarantee())
                allocation.summary_effects.push_back(
                    {statement, argument_index, effect});
              continue;
            }
            allocation.summary_effects.push_back(
                {statement, argument_index, effect});
            if (options.analysis_policy == AnalysisPolicy::proof_only &&
                effect.may_remain_live() &&
                effect.fates != ownership_summary::fate_live) {
              if (!contains_statement(allocation.ambiguous_ownership_events, statement))
                allocation.ambiguous_ownership_events.push_back(statement);
              ownership_summary::BorrowCondition borrow;
              if (allocation.guarded_borrows.size() < 32 &&
                  ownership_summary::conditional_borrow(statement, argument_index, &borrow))
                allocation.guarded_borrows.push_back({statement, borrow});
            }
            const bool establishes_release =
                options.analysis_policy == AnalysisPolicy::proof_only
                    ? effect.definitely_freed()
                    : effect.may_free();
            if (establishes_release &&
                !contains_statement(allocation.release_events, statement))
              allocation.release_events.push_back(statement);
            if (effect.definitely_consumed() &&
                !contains_statement(allocation.definite_terminal_events,
                                    statement))
              allocation.definite_terminal_events.push_back(statement);
          }
        }

        const bool local_spill = pointer_local_spill(allocation, statement);
        if (local_spill) {
          allocation.local_memory_spill = true;
          if (options.analysis_policy == AnalysisPolicy::proof_only &&
              !local_confinement_checked) {
            local_confinement_checked = true;
            local_confinement_proved =
                local_confinement::fresh_local_ownership_is_confined();
          }
        }

        if (options.analysis_policy == AnalysisPolicy::proof_only &&
            is_gimple_assign(statement) && gimple_assign_cast_p(statement) &&
            !pointer_value(gimple_assign_lhs(statement)) &&
            alias::may_share_allocation_origin(
                gimple_assign_rhs1(statement), allocation.value)) {
          // GCC can lower a pointer-sized memcpy/store to an integer cast.
          // The pointer SSA walker cannot follow that representation through
          // a later store, return, or conversion back to a pointer.
          allocation.opaque_ownership_transfer = true;
          if (allocation.opaque_ownership_call == nullptr)
            allocation.opaque_ownership_call = statement;
        }

        if (options.analysis_policy == AnalysisPolicy::proof_only &&
            pointer_store_may_transfer(allocation, statement) &&
            !pointer_store_escape(allocation, statement) &&
            !(local_spill && local_confinement_proved)) {
          allocation.opaque_ownership_transfer = true;
          if (allocation.opaque_ownership_call == nullptr)
            allocation.opaque_ownership_call = statement;
        }

        const bool store_escape = pointer_store_escape(
            allocation, statement,
            options.analysis_policy == AnalysisPolicy::exploratory);
        if ((gimple_code(statement) == GIMPLE_RETURN || store_escape) &&
            !contains_statement(allocation.definite_terminal_events,
                                statement)) {
          bool terminal = store_escape;
          if (gimple_code(statement) == GIMPLE_RETURN) {
            const greturn *return_statement = as_a<greturn *>(statement);
            terminal = same_owned_object_after_allocation(
                gimple_return_retval(return_statement), allocation);
          }
          if (terminal)
            allocation.definite_terminal_events.push_back(statement);
          else if (options.analysis_policy == AnalysisPolicy::proof_only &&
                   gimple_code(statement) == GIMPLE_RETURN) {
            tree returned =
                gimple_return_retval(as_a<greturn *>(statement));
            if (pointer_value(returned) &&
                alias::may_share_allocation_origin(returned,
                                                   allocation.value)) {
              // A PHI return transfers ownership on its exact incoming
              // arm.  Both finite typestate and the final leak witness must
              // retain that edge; finding a NULL arm alone proves no leak.
              if (!add_conditional_phi_returns(&allocation, statement)) {
                allocation.opaque_ownership_transfer = true;
                if (allocation.opaque_ownership_call == nullptr)
                  allocation.opaque_ownership_call = statement;
              }
            }
          }
        }
      }
    }

    if (options.analysis_policy == AnalysisPolicy::proof_only &&
        allocation.local_memory_spill && !local_confinement_proved)
      allocation.opaque_ownership_transfer = true;
  }

  unsigned long iterations = 0;
  unsigned long merges = 0;
  for (const AllocationSite &allocation : allocations)
  {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::lifetime_cfg)) {
      record_budget_inconclusive(
          options, best_source_location({allocation.statement}));
      return;
    }
    analyze_allocation(allocation, options, &iterations, &merges);
  }
  for (const ThreadSite &thread : threads)
  {
    if (!analysis_budget::consume(
            analysis_budget::WorkKind::lifetime_cfg)) {
      record_budget_inconclusive(
          options, best_source_location({thread.statement}));
      return;
    }
    analyze_thread(thread, options, &iterations, &merges);
  }

  memory_value::dump_statistics();
  dump_value_expansion_statistics();

  const char *name = nullptr;
  if (cfun->decl != NULL_TREE && DECL_NAME(cfun->decl) != NULL_TREE)
    name = IDENTIFIER_POINTER(DECL_NAME(cfun->decl));
  dump_engine_summary(name, allocations.size(), threads.size(), iterations,
                      merges);
}

}  // namespace

void run_lifetime_engine(const PluginOptions &options) {
  cgraph_node *node;
  FOR_EACH_DEFINED_FUNCTION(node) {
    if (!gimple_has_body_p(node->decl) || node->get_fun() == nullptr)
      continue;
    if (!analysis_budget::begin_function(node->decl)) {
      record_budget_inconclusive(options, DECL_SOURCE_LOCATION(node->decl));
      if (!analysis_budget::tu_available())
        break;
      continue;
    }
    push_cfun(node->get_fun());
    analyze_function(options);
    pop_cfun();
    analysis_budget::end_function();
  }
}

}  // namespace static_analyzer
