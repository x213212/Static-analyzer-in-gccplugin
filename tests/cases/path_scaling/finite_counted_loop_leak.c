#include <stddef.h>
#include <stdlib.h>

static volatile int observed;

/* The proven finite loop must preserve the active allocation state on its
   unique normal exit, so leak refinement can find the exit path. */
void finite_counted_loop_leak(void) {
  int *pointer = (int *)malloc(100 * sizeof(*pointer));
  for (size_t index = 0; index < 100; ++index)
    pointer[index] = 5;
  observed = pointer[0];
}
