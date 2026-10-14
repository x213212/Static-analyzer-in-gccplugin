#include <limits.h>
#include <stdlib.h>

static volatile int observed;

/* Zero body executions: treating every constant loop as one-trip would invent
   a release before the use.  The later free keeps this a non-leak control. */
void zero_trip_is_not_one_trip(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  for (int index = 1; index < 1; ++index)
    free(pointer);
  observed = pointer[0];
  free(pointer);
}

/* The next guard is still true, so exact one-trip unfolding must abstain. */
void two_trip_is_not_one_trip(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  for (int index = 0; index < 2; ++index) {
    if (index == 0)
      free(pointer);
  }
  observed = pointer[0];
}

/* This loop really executes once under unsigned modular arithmetic, but the
   strict theorem intentionally rejects a transition whose addition wraps. */
void wrapped_transition_is_not_accepted(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  for (unsigned index = UINT_MAX; index >= UINT_MAX; ++index)
    free(pointer);
  observed = pointer[0];
}

/* A runtime bound cannot establish the branch selected after the latch. */
void runtime_bound_is_not_exact_one_trip(unsigned limit) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  for (unsigned index = 0; index < limit; ++index) {
    if (index == 0)
      free(pointer);
  }
  observed = pointer[0];
}

/* Although the first two guard values choose body then exit, the nominal exit
   can recycle through the latch.  Region reachability must reject this CFG so
   an active one-trip schedule cannot be reused by the external cycle. */
void exit_reentry_is_not_canonical(int repeat_exit) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  int index = 0;

header:
  if (index <= 0) {
    free(pointer);
  } else {
    if (repeat_exit)
      goto latch;
    goto done;
  }

latch:
  ++index;
  goto header;

done:
  observed = pointer[0];
}
