#include <stdlib.h>

__attribute__((noinline, optimize("O0")))
int unrelated_memory_operand_after_free(int *other) {
  int *allocated = (int *)malloc(sizeof(*allocated));
  free(allocated);

  // This statement has a VUSE because it reads *other, but allocated is only
  // inspected as a scalar pointer value.  It does not dereference allocated.
  return *other + (allocated != NULL);
}
