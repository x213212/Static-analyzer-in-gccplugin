#include <stdlib.h>

void free_in_loop(unsigned int count) {
  void *pointer = malloc(32);
  do {
    free(pointer);
  } while (count-- > 1);
}
