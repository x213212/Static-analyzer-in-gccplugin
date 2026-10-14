#include <stdlib.h>

__attribute__((weak)) void replaceable_release(void *value) { free(value); }

void weak_deallocator_is_not_a_must_free(void) {
  void *value = malloc(32);
  replaceable_release(value);
}
