#include <stdlib.h>

static volatile unsigned char observed;

__attribute__((noinline, noclone))
void maybe_read_pointer(unsigned char *pointer, int read_it) {
  if (read_it)
    observed = *pointer;
}

void derived_pointer_is_not_a_must_dereference(void) {
  unsigned char *allocation = (unsigned char *)malloc(16);
  unsigned char *interior = allocation + 1;
  free(allocation);
  maybe_read_pointer(interior, 0);
}
