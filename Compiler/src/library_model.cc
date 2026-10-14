#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "cgraph.h"
#include "stringpool.h"
#include "static_analyzer/library_model.h"

#include <cstring>
#include <vector>

namespace static_analyzer {
namespace library_model {
namespace {
bool enabled = false;
bool integer(tree t) {
  return t != NULL_TREE && TREE_CODE(t) == INTEGER_TYPE &&
         !TYPE_UNSIGNED(t) && TYPE_PRECISION(t) == TYPE_PRECISION(integer_type_node);
}
bool size_type(tree t) {
  return t != NULL_TREE && TYPE_MAIN_VARIANT(t) == TYPE_MAIN_VARIANT(size_type_node);
}
bool pointer(tree t) { return t != NULL_TREE && POINTER_TYPE_P(t); }
bool void_pointer(tree t) { return pointer(t) && VOID_TYPE_P(TREE_TYPE(t)); }
bool char_pointer(tree t) {
  return pointer(t) && TYPE_MAIN_VARIANT(TREE_TYPE(t)) == TYPE_MAIN_VARIANT(char_type_node);
}
bool named_record(tree t, const char *name) {
  if (t == NULL_TREE || TREE_CODE(t) != RECORD_TYPE) return false;
  tree n = TYPE_NAME(t);
  if (n != NULL_TREE && TREE_CODE(n) == TYPE_DECL) n = DECL_NAME(n);
  return n != NULL_TREE && TREE_CODE(n) == IDENTIFIER_NODE &&
         std::strcmp(IDENTIFIER_POINTER(n), name) == 0;
}
bool record_pointer(tree t, const char *name) {
  return pointer(t) && named_record(TREE_TYPE(t), name);
}
bool dictionary_slot(tree t) {
  return pointer(t) && record_pointer(TREE_TYPE(t), "AVDictionary");
}
}  // namespace

void configure(bool ffmpeg) { enabled = ffmpeg; }
bool ffmpeg_enabled() { return enabled; }
const char *profile_name() { return enabled ? "ffmpeg-sdk-v1" : "none"; }

CallEffect effect(const gimple *call) {
  CallEffect result;
  if (!enabled || call == nullptr || !is_gimple_call(call)) return result;
  if ((gimple_call_flags(call) & (ECF_NORETURN | ECF_RETURNS_TWICE)) != 0) return result;
  tree decl = gimple_call_fndecl(call);
  if (decl == NULL_TREE || DECL_WEAK(decl) || !DECL_ASSEMBLER_NAME_SET_P(decl)) return result;
  cgraph_node *node = cgraph_node::get(decl);
  if (gimple_has_body_p(decl) || (node != nullptr && node->definition)) return result;
  tree type = TREE_TYPE(decl);
  if (type == NULL_TREE || TREE_CODE(type) != FUNCTION_TYPE ||
      gimple_call_fntype(call) != type || !prototype_p(type)) return result;
  std::vector<tree> args;
  tree list = TYPE_ARG_TYPES(type);
  for (; list != NULL_TREE && list != void_list_node; list = TREE_CHAIN(list)) {
    if (args.size() >= 8) return result;
    args.push_back(TREE_VALUE(list));
  }
  if (list != void_list_node || args.size() != gimple_call_num_args(call)) return result;
  const char *name = IDENTIFIER_POINTER(DECL_ASSEMBLER_NAME(decl));
  if (name[0] == '*') ++name;
  tree ret = TREE_TYPE(type);
  if (!std::strcmp(name, "ffurl_get_multi_file_handle") && args.size() == 3 &&
      integer(ret) && record_pointer(args[0], "URLContext") &&
      pointer(args[1]) && pointer(TREE_TYPE(args[1])) &&
      integer(TREE_TYPE(TREE_TYPE(args[1]))) && pointer(args[2]) &&
      integer(TREE_TYPE(args[2]))) {
    result.kind = Effect::output_handles; result.slot_argument = 1;
  } else if (!std::strcmp(name, "av_dict_set") && args.size() == 4 &&
             integer(ret) && dictionary_slot(args[0]) &&
             char_pointer(args[1]) && char_pointer(args[2]) && integer(args[3])) {
    result.kind = Effect::dictionary_insert; result.dictionary = true;
  } else if (!std::strcmp(name, "av_dict_free") && args.size() == 1 &&
             VOID_TYPE_P(ret) && dictionary_slot(args[0])) {
    result.kind = Effect::release_slot; result.dictionary = true;
  } else if (!std::strcmp(name, "av_freep") && args.size() == 1 &&
             VOID_TYPE_P(ret) && void_pointer(args[0])) {
    result.kind = Effect::release_slot;
  } else if (!std::strcmp(name, "av_free") && args.size() == 1 &&
             VOID_TYPE_P(ret) && void_pointer(args[0])) {
    result.kind = Effect::heap_release;
  } else if (void_pointer(ret) &&
             (((!std::strcmp(name, "av_malloc") || !std::strcmp(name, "av_mallocz")) &&
               args.size() == 1 && size_type(args[0])) ||
              ((!std::strcmp(name, "av_calloc") || !std::strcmp(name, "av_malloc_array")) &&
               args.size() == 2 && size_type(args[0]) && size_type(args[1])))) {
    result.kind = Effect::heap_allocate;
  }
  return result;
}
}  // namespace library_model
}  // namespace static_analyzer
