#include <stddef.h>
typedef struct AVDictionary { int value; } AVDictionary;
typedef struct DifferentProtocol DifferentProtocol;
extern int ffurl_get_multi_file_handle(DifferentProtocol *, int **, int *);
static AVDictionary shared;
__attribute__((noinline)) int av_dict_set(AVDictionary **out,
    const char *key, const char *value, int flags) {
  (void)key; (void)value; (void)flags;
  *out = &shared;
  return 0;
}
void visible_body_is_not_replaced_with_a_heap_model(void) {
  AVDictionary *borrowed = NULL;
  av_dict_set(&borrowed, "key", "value", 0);
}
void wrong_provider_prototype_is_rejected(DifferentProtocol *provider) {
  int *pointer = NULL, count;
  ffurl_get_multi_file_handle(provider, &pointer, &count);
}
