#include "fixture.h"

PT_NOINLINE void pt_c_borrow(const volatile int *value) {
  if (value != 0)
    (void)*value;
}

PT_NOINLINE void pt_c_read(const volatile int *value) {
  (void)*value;
}
