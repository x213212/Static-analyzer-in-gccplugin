#include <stdlib.h>

static volatile unsigned char observed;

void direct_use_after_free(void) {
  unsigned char *pointer = (unsigned char *)malloc(32);
  free(pointer);
  observed = pointer[0];
}
