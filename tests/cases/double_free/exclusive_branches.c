#include <stdlib.h>

void exclusive_branch_frees(int condition) {
  void *pointer = malloc(32);
  if (condition)
    free(pointer);
  else
    free(pointer);
}
