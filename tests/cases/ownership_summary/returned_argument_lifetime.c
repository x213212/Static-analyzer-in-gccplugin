#include <stdlib.h>

static __attribute__((noinline)) void *identity(void *p) { return p; }

void returned_argument_released(void) {
  void *p = malloc(8);
  if (!p) return;
  free(identity(p));
}

int returned_argument_uaf(void) {
  char *p = malloc(8);
  if (!p) return 0;
  char *q = identity(p);
  free(p);
  return *(volatile char *)q;
}

void returned_argument_double_free(void) {
  void *p = malloc(8);
  if (!p) return;
  void *q = identity(p);
  free(p);
  free(q);
}
