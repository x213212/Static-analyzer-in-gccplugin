#include "fixture.h"

PT_NOINLINE void pt_mismatch(const volatile int *value, int required_argument) {
  (void)required_argument;
  (void)*value;
}
