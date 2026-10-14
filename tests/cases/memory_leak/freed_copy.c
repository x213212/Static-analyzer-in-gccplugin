#include <stdlib.h>

void freed_copy(void) {
  void *pointer = malloc(32);
  void *alias = pointer;
  free(alias);
}
