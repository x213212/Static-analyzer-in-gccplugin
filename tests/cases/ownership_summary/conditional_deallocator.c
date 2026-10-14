#include <stdlib.h>

static void maybe_release(void *value, int release) {
  if (release)
    free(value);
}

void conditional_deallocator(int release) {
  void *value = malloc(32);
  maybe_release(value, release);
}
