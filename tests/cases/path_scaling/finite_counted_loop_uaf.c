#include <stddef.h>
#include <stdlib.h>

static volatile int observed;

/* The initialization loop is ownership-event neutral and has a canonical
   unsigned SSA induction variable.  Its proven finite exit must not turn the
   later direct use-after-free into a loop-visit-cap abstention.  */
void finite_counted_loop_uaf(void) {
  int *pointer = (int *)malloc(100 * sizeof(*pointer));
  for (size_t index = 0; index < 100; ++index)
    pointer[index] = 5;
  free(pointer);
  observed = pointer[0];
}
