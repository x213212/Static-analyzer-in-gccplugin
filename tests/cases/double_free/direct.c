#include <stdlib.h>

void direct_double_free(void) {
  void *pointer = malloc(32);
  free(pointer);
  free(pointer);
}
