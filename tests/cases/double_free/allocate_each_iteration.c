#include <stdlib.h>

void allocate_each_iteration(unsigned int count) {
  for (unsigned int index = 0; index < count; ++index) {
    void *pointer = malloc(32);
    free(pointer);
  }
}
