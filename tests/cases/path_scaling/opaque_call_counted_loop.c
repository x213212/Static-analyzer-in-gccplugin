#include <stddef.h>
#include <stdlib.h>

extern void opaque_tick(size_t);
static volatile int observed;

/* A syntactically counted loop containing an opaque call is outside the
   stutter theorem: the analyzer has no proof that the call returns normally
   on every required iteration. */
void opaque_call_counted_loop(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  for (size_t index = 0; index < 100; ++index)
    opaque_tick(index);
  free(pointer);
  observed = pointer[0];
}
