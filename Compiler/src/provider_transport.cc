#include "gcc-plugin.h"
#include "bversion.h"
#include "context.h"
#include "tree.h"
#include "tree-pass.h"
#include "cgraph.h"
#include "stringpool.h"
#include "dumpfile.h"
#include "gtype-desc.h"
#include "options.h"

#include "static_analyzer/provider_transport.h"

#include <cstring>
#include <map>

namespace static_analyzer {
namespace provider_transport {
namespace {

// An ordinary source-language attribute identifier cannot spell the leading
// '*'. This is deliberately NOT registered as a trusted source attribute.
constexpr const char *transport_attribute = "*static_analyzer_provider_v1";
constexpr std::size_t max_records = 4096;

struct Entry {
  tree declaration;
  tree payload;
  int node_uid;
};

struct Statistics {
  unsigned long generations = 0;
  unsigned long recorded = 0;
  unsigned long imported = 0;
  unsigned long attached = 0;
  unsigned long detached = 0;
  unsigned long rejected = 0;
  unsigned long cap_skips = 0;
  unsigned long stale_drops = 0;
  unsigned long queries = 0;
  unsigned long hits = 0;
};

std::map<unsigned int, Entry> entries;
Statistics statistics;
GenerateFacts generator = nullptr;
bool generated_for_unit = false;
bool registered = false;
register_pass_info registration;

bool lto_active() { return flag_lto || in_lto_p; }

bool thunk_p(const cgraph_node *node) {
#if BUILDING_GCC_MAJOR >= 11
  return node->thunk;
#else
  return node->thunk.thunk_p;
#endif
}

cgraph_node *eligible_node(tree declaration) {
  if (declaration == NULL_TREE || TREE_CODE(declaration) != FUNCTION_DECL)
    return nullptr;
  cgraph_node *node = cgraph_node::get(declaration);
  if (node == nullptr || node->decl != declaration ||
      (!node->definition && !node->in_other_partition) || node->alias ||
      thunk_p(node) || node->clone_of != nullptr ||
      node->former_clone_of != NULL_TREE || node->inlined_to != nullptr ||
      DECL_ABSTRACT_ORIGIN(declaration) != NULL_TREE || DECL_WEAK(declaration))
    return nullptr;
  return node;
}

bool same_current_node(const Entry &entry) {
  cgraph_node *node = eligible_node(entry.declaration);
  return node != nullptr && node->get_uid() == entry.node_uid;
}

bool marker_p(tree attribute) {
  tree purpose = TREE_PURPOSE(attribute);
  return purpose != NULL_TREE && TREE_CODE(purpose) == IDENTIFIER_NODE &&
         std::strcmp(IDENTIFIER_POINTER(purpose), transport_attribute) == 0;
}

// Never call remove_attribute() on a possibly shared declaration list: that
// helper can modify TREE_CHAIN links in place. Preserve the original list if
// there is no marker, otherwise copy only the retained list spine in order.
tree without_markers(tree attributes, tree *payload = nullptr,
                     unsigned int *marker_count = nullptr) {
  unsigned int count = 0;
  for (tree attribute = attributes; attribute != NULL_TREE;
       attribute = TREE_CHAIN(attribute)) {
    if (!marker_p(attribute))
      continue;
    ++count;
    if (payload != nullptr)
      *payload = TREE_VALUE(attribute);
  }
  if (marker_count != nullptr)
    *marker_count = count;
  if (count == 0)
    return attributes;

  tree result = NULL_TREE;
  tree *tail = &result;
  for (tree attribute = attributes; attribute != NULL_TREE;
       attribute = TREE_CHAIN(attribute)) {
    if (marker_p(attribute))
      continue;
    tree copy = copy_node(attribute);
    TREE_CHAIN(copy) = NULL_TREE;
    *tail = copy;
    tail = &TREE_CHAIN(copy);
  }
  statistics.detached += count;
  return result;
}

void store_payload(tree declaration, tree payload, bool imported) {
  cgraph_node *node = eligible_node(declaration);
  if (node == nullptr || payload == NULL_TREE) {
    ++statistics.rejected;
    return;
  }
  const unsigned int uid = DECL_UID(declaration);
  auto found = entries.find(uid);
  if (found != entries.end()) {
    if (found->second.declaration != declaration ||
        found->second.node_uid != node->get_uid()) {
      entries.erase(found);
      ++statistics.stale_drops;
      return;
    }
    found->second.payload = payload;
  } else {
    if (entries.size() >= max_records) {
      ++statistics.cap_skips;
      return;
    }
    entries.emplace(uid, Entry{declaration, payload, node->get_uid()});
  }
  if (imported)
    ++statistics.imported;
  else
    ++statistics.recorded;
}

void dump_phase(const char *phase) {
  if (dump_file == nullptr || !(dump_flags & TDF_DETAILS))
    return;
  dump_printf(MSG_NOTE,
              "provider-transport: phase=%s lto=%s wpa=%s ltrans=%s "
              "records=%lu\n",
              phase, in_lto_p ? "yes" : "no", flag_wpa ? "yes" : "no",
              flag_ltrans ? "yes" : "no",
              static_cast<unsigned long>(entries.size()));
  dump_statistics();
}

void detach_all() {
  // All incoming markers must disappear even when their record is invalid or
  // the cache is full. Otherwise private metadata can change compiler ICF.
  // This traversal is linear in GCC's live symbol/attribute population; only
  // the stored fact set, not the compiler's own graph, is capped at 4096.
  cgraph_node *node;
  FOR_EACH_FUNCTION(node) {
    if (node->decl != NULL_TREE)
      DECL_ATTRIBUTES(node->decl) =
          without_markers(DECL_ATTRIBUTES(node->decl));
  }
}

void attach_current() {
  for (auto iterator = entries.begin(); iterator != entries.end();) {
    Entry &entry = iterator->second;
    if (!same_current_node(entry)) {
      iterator = entries.erase(iterator);
      ++statistics.stale_drops;
      continue;
    }
    tree attributes = without_markers(DECL_ATTRIBUTES(entry.declaration));
    DECL_ATTRIBUTES(entry.declaration) =
        tree_cons(get_identifier(transport_attribute), entry.payload, attributes);
    ++statistics.attached;
    ++iterator;
  }
}

void import_and_detach() {
  cgraph_node *node;
  FOR_EACH_FUNCTION(node) {
    if (node->decl == NULL_TREE)
      continue;
    tree payload = NULL_TREE;
    unsigned int count = 0;
    DECL_ATTRIBUTES(node->decl) =
        without_markers(DECL_ATTRIBUTES(node->decl), &payload, &count);
    if (count == 0)
      continue;
    // A duplicate envelope is not a choice between alternative theorems.
    if (count != 1 || payload == NULL_TREE) {
      entries.erase(DECL_UID(node->decl));
      ++statistics.rejected;
      continue;
    }
    store_payload(node->decl, payload, true);
  }
}

void generate_hook() {
  // Never create a provider theorem from an unloaded WPA/LTRANS declaration.
  if (!flag_lto || in_lto_p || generated_for_unit || generator == nullptr)
    return;
  generated_for_unit = true;
  ++statistics.generations;
  generator();
  dump_phase("generate-summary");
}

void write_hook() {
  // GCC invokes this before write_lto()/produce_asm_for_decls(). Removing the
  // marker before returning would remove it from the serialized declaration.
  attach_current();
  dump_phase("write-summary");
}

void read_hook() {
  // At WPA this follows declaration/resolution input but PRECEDES final
  // cgraph symbol merging. Queries/restreams recheck exact surviving nodes.
  import_and_detach();
  dump_phase("read-summary");
}

void write_optimization_hook() {
  // Regular IPA optimization is finished; GCC now streams each partition.
  attach_current();
  dump_phase("write-optimization-summary");
}

void read_optimization_hook() {
  import_and_detach();
  dump_phase("read-optimization-summary");
}

void start_unit(void *, void *) {
  entries.clear();
  statistics = Statistics{};
  generated_for_unit = false;
}

void all_ipa_end(void *, void *) {
  if (lto_active())
    detach_all();
}

void mark_payloads(void *, void *) {
  for (const auto &item : entries) {
    gt_ggc_mx_tree_node(item.second.declaration);
    gt_ggc_mx_tree_node(item.second.payload);
  }
}

const pass_data transport_pass_data = {
    IPA_PASS, "static_analyzer_transport", OPTGROUP_NONE, TV_PLUGIN_RUN,
    0, 0, 0, 0, 0,
};

class TransportPass : public ipa_opt_pass_d {
 public:
  explicit TransportPass(gcc::context *context)
      : ipa_opt_pass_d(transport_pass_data, context, generate_hook, write_hook,
                       read_hook, write_optimization_hook,
                       read_optimization_hook, nullptr, 0, nullptr, nullptr) {}

  bool gate(function *) override { return lto_active(); }

  unsigned int execute(function *) override {
    // In fat-LTO compilation regular IPA executes after initial streaming but
    // BEFORE ALL_IPA_PASSES_END. Strip here, before analyzer/ICF, as well as in
    // read hooks. Waiting only for the end callback changes optimization.
    detach_all();
    dump_phase("execute-detach");
    return 0;
  }
};

}  // namespace

void register_passes(const char *plugin_name, GenerateFacts generate) {
  if (registered || plugin_name == nullptr || plugin_name[0] == '\0')
    return;
  registered = true;
  generator = generate;
  registration.pass = new TransportPass(g);
  registration.reference_pass_name = "analyzer";
  registration.ref_pass_instance_number = 1;
  registration.pos_op = PASS_POS_INSERT_BEFORE;
  register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, nullptr,
                    &registration);
  register_callback(plugin_name, PLUGIN_START_UNIT, start_unit, nullptr);
  register_callback(plugin_name, PLUGIN_ALL_IPA_PASSES_END, all_ipa_end, nullptr);
  register_callback(plugin_name, PLUGIN_GGC_MARKING, mark_payloads, nullptr);
}

void record(tree declaration, tree payload) {
  if (!lto_active())
    return;
  if (declaration != NULL_TREE && TREE_CODE(declaration) == FUNCTION_DECL &&
      payload == NULL_TREE) {
    entries.erase(DECL_UID(declaration));
    return;
  }
  store_payload(declaration, payload, false);
}

tree lookup(tree declaration) {
  ++statistics.queries;
  if (!lto_active() || declaration == NULL_TREE ||
      TREE_CODE(declaration) != FUNCTION_DECL)
    return NULL_TREE;
  auto found = entries.find(DECL_UID(declaration));
  if (found == entries.end())
    return NULL_TREE;
  if (found->second.declaration != declaration ||
      !same_current_node(found->second)) {
    entries.erase(found);
    ++statistics.stale_drops;
    return NULL_TREE;
  }
  ++statistics.hits;
  return found->second.payload;
}

void dump_statistics() {
  if (dump_file == nullptr)
    return;
  dump_printf(MSG_NOTE,
              "provider-transport summary: records=%lu limit=%lu "
              "generations=%lu recorded=%lu imported=%lu attached=%lu "
              "detached=%lu rejected=%lu cap-skips=%lu stale-drops=%lu "
              "queries=%lu hits=%lu\n",
              static_cast<unsigned long>(entries.size()),
              static_cast<unsigned long>(max_records), statistics.generations,
              statistics.recorded, statistics.imported, statistics.attached,
              statistics.detached, statistics.rejected, statistics.cap_skips,
              statistics.stale_drops, statistics.queries, statistics.hits);
}

}  // namespace provider_transport
}  // namespace static_analyzer
