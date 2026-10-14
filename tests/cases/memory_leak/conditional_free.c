#include <stdlib.h>

void conditional_free(int release) {
  void *pointer = malloc(32);
  if (release)
    free(pointer);
}
