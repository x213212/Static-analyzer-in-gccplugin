#include <stdlib.h>

void distinct_successful_allocations_never_equal(void) {
  unsigned char *left = (unsigned char *)malloc(8);
  unsigned char *right = (unsigned char *)malloc(8);

  /* The body is unreachable: two simultaneously live, successful malloc
     results denote distinct objects.  A solver model which treats their
     values as unrelated unconstrained bit-vectors can otherwise invent the
     equality and report a synthetic use-after-free. */
  if (left != NULL && right != NULL && left == right) {
    free(left);
    left[0] = 1;
  }

  if (left != right)
    free(left);
  free(right);
}
