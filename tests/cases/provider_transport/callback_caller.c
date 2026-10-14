#include <stdlib.h>
#include "fixture.h"

void pt_forward(int *value, void (*consume)(int *));

static PT_NOINLINE void release(int *value) {
  free(value);
}

int main(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return 0;
  *value = 23;
  pt_forward(value, release);
  /* No leak: the forwarded callback frees value.  A generic provider
     summary must not invent a no-free/no-capture promise for pt_forward. */
  return 0;
}
