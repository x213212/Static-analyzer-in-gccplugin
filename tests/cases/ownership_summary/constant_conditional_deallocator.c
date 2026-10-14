#include <stdlib.h>

static __attribute__((noinline, noclone)) void maybe_release(
    void *value, int release) {
  if (release)
    free(value);
}

void constant_true_conditional_deallocator_is_inconclusive(void) {
  void *value = malloc(32);
  maybe_release(value, 1);
}
