#include <stdlib.h>

static volatile int observed;

static __attribute__((noinline)) void maybe_read(int *value, int read_it) {
  if (read_it)
    observed = *value;
}

void known_false_callee_dereference_is_not_a_finding(void) {
  int *value = (int *)malloc(sizeof(*value));
  free(value);
  maybe_read(value, 0);
}
