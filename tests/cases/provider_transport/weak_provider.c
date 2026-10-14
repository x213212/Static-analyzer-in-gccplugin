#include "fixture.h"

/* GNU weak symbols are the feature under test; no portable C replacement
   would exercise the linker-resolution gate. */
__attribute__((weak)) PT_NOINLINE void pt_replaceable(int *value) {
  (void)*(volatile int *)value;
}
