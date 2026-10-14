#include "fixture.h"

PT_NOINLINE void pt_interposable(int *value) {
  (void)*(volatile int *)value;
}
