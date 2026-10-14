#include <stdlib.h>

static __attribute__((noinline, noclone)) void *maybe_allocate(int create) {
  if (create)
    return malloc(8);
  return NULL;
}

void known_null_summary_return_is_not_a_leak(void) {
  void *value = maybe_allocate(0);
  (void)value;
}
