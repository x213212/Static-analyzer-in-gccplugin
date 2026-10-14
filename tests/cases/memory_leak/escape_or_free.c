#include <stdlib.h>

void *saved;

void escape_or_free_has_no_leak(int publish) {
  void *pointer = malloc(8);
  if (publish)
    saved = pointer;
  if (!publish)
    free(pointer);
}
