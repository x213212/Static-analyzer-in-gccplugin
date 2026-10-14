#include <stdlib.h>

static void release_leaf(void *value) { free(value); }

static void release_wrapper(void *value) { release_leaf(value); }

void definite_deallocator(void) {
  void *value = malloc(32);
  release_wrapper(value);
}
