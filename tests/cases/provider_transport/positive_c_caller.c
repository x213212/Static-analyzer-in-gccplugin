#include <stdlib.h>
#include "fixture.h"

void pt_c_borrow(const volatile int *value);
void pt_c_read(const volatile int *value);

PT_NOINLINE void pt_c_leak(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  *value = 7;
  pt_c_borrow(value);
  /* SA001: the source-defined provider neither frees nor retains value. */
}

PT_NOINLINE void pt_c_uaf(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  *value = 9;
  free(value);
  pt_c_read(value); /* SA003: the remote provider necessarily loads value. */
}

int main(void) {
  pt_c_leak();
  pt_c_uaf();
  return 0;
}
