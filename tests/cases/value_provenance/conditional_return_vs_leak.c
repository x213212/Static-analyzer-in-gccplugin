#include <stdlib.h>

extern int opaque_choice(void);

char *conditional_branch_allocations_returned(void) {
  char *data;

  if (opaque_choice())
    data = (char *)malloc(8);
  else
    data = (char *)calloc(1, 8);

  /* Relative to either allocation event, only its own incoming PHI edge is
     reachable.  Both objects transfer to the caller; neither is a leak. */
  return data;
}

char *allocation_returned_or_leaked(void) {
  char *data = (char *)malloc(8);

  if (opaque_choice())
    return data;

  /* This CFG arm does not transfer DATA, but an external opaque_choice could
     always return true.  Its reachability is UNKNOWN in proof-only analysis.
     phi_return_ownership.c covers a truly symbolic input and a proven leak. */
  return NULL;
}
