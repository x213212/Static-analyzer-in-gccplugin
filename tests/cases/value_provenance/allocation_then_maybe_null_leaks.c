#include <stdlib.h>

extern int opaque_choice(void);

void allocation_then_maybe_null_leaks(int choose_null) {
  char *data = (char *)malloc(8);
  char *selected;

  if (choose_null)
    selected = NULL;
  else
    selected = data;

  /* Both PHI incoming edges are reachable after the allocation.  Freeing
     NULL on one of them must not be mistaken for releasing DATA. */
  free(selected);
}

/* The old positive used opaque_choice() rather than a symbolic input.
   That function may always return zero, in which case data is always freed.
   Keep the original program as an UNKNOWN regression; an incomplete release
   event set must not fabricate a leak witness from the releasing PHI arm. */
void opaque_choice_does_not_prove_a_leak(void) {
  char *data = (char *)malloc(8);
  char *selected = opaque_choice() ? NULL : data;
  free(selected);
}
