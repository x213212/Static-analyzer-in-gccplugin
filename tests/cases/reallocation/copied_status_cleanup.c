#include <stdlib.h>

void copied_realloc_status_stays_inconclusive(void) {
  void *pointer = malloc(8);
  void *replacement = realloc(pointer, 16);
  volatile int failed = replacement == 0;
  if (failed)
    free(pointer);
  else
    free(replacement);
}
