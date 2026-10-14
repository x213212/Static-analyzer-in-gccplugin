#include <stdlib.h>

static void release_again(void *value) { free(value); }

void interprocedural_double_free(void) {
  void *value = malloc(32);
  free(value);
  release_again(value);
}
