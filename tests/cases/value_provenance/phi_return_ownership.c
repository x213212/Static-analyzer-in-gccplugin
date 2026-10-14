#include <stdlib.h>

void *null_on_allocation_failure(void) {
  void *p = malloc(8);
  if (!p) return NULL;
  return p;
}

void *correlated_cleanup_or_transfer(int release) {
  void *p = malloc(8);
  if (!p) return NULL;
  if (release) free(p);
  if (release) return NULL;
  return p;
}

void *real_null_return_leak(int transfer) {
  void *p = malloc(8);
  if (!p) return NULL;
  if (transfer) return p;
  return NULL;  // A successful allocation can reach this exit unreleased.
}
