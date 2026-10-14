#include <stdlib.h>

extern void uncertain_release(void *pointer);

void unresolved_leak(void) {
  int *pointer = malloc(sizeof(*pointer));
  if (pointer == NULL)
    return;
  uncertain_release(pointer);
}

void unresolved_double_free(void) {
  int *pointer = malloc(sizeof(*pointer));
  if (pointer == NULL)
    return;
  uncertain_release(pointer);
  free(pointer);
}

int unresolved_access(void) {
  int *pointer = malloc(sizeof(*pointer));
  if (pointer == NULL)
    return 0;
  *pointer = 5;
  uncertain_release(pointer);
  int result = *pointer;
  free(pointer);
  return result;
}
