#include <stdlib.h>

void derived_pointer_use(void) {
  int *pointer = (int *)malloc(4 * sizeof(int));
  int *interior = pointer + 1;
  free(pointer);
  interior[0] = 7;
}
