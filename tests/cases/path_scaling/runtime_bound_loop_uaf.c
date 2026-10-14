#include <stddef.h>
#include <stdlib.h>

static volatile int observed;

/* This loop is finite in C, but its trip count is not a constant in the
   current bounded proof.  It is an intentional fail-closed control: the
   counted-loop recognizer must leave the candidate inconclusive rather than
   generalizing a syntactic backedge into a proof.  */
void runtime_bound_loop_uaf(size_t limit) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  for (size_t index = 0; index < limit; ++index)
    observed = (int)index;
  free(pointer);
  observed = pointer[0];
}
