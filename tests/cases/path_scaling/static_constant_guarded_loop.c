#include <stddef.h>
#include <stdlib.h>

static int static_one = 1;
static int static_zero = 0;
static volatile int observed;

/* GCC's constant-value oracle can prove both guards and the counted-loop
   summary can then discharge the otherwise unrelated initialization loop. */
void static_constant_guarded_loop_uaf(void) {
  int *pointer = (int *)malloc(100 * sizeof(*pointer));
  if (static_one) {
    for (size_t index = 0; index < 100; ++index)
      pointer[index] = 5;
    free(pointer);
  }
  if (static_one)
    observed = pointer[0];
}

/* The same oracle must not turn a compile-time false sink into a finding. */
void static_constant_guarded_loop_safe(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  if (static_one)
    free(pointer);
  if (static_zero)
    observed = pointer[0];
}
