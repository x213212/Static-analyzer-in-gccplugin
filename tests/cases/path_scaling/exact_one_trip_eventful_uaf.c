#include <stdlib.h>

static volatile int observed;

/* Both loops contain ownership events, so they cannot be collapsed as
   event-neutral stutters.  Their signed SSA recurrences nevertheless prove
   that the body edge executes exactly once.  The pointer PHI after the first
   latch also exercises correlated provenance-edge consumption. */
void exact_one_trip_eventful_uaf(void) {
  int *pointer = NULL;
  for (int source_index = 0; source_index < 1; ++source_index) {
    pointer = (int *)malloc(sizeof(*pointer));
    free(pointer);
  }
  for (int sink_index = 0; sink_index < 1; ++sink_index)
    observed = pointer[0];
}

/* The body is the false edge of a comparison whose induction operand is on
   the right.  Successor vector order or an assumed true-body convention must
   not affect the one-trip proof. */
void exact_one_trip_reversed_false_edge_uaf(void) {
  int *pointer = NULL;
  int index = 0;

header:
  if (0 < index)
    goto done;
  pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  ++index;
  goto header;

done:
  observed = pointer[0];
}
