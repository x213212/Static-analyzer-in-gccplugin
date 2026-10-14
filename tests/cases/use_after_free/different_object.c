#include <stdlib.h>

extern void consume(void *);

void different_object_is_safe(void) {
  void *first = malloc(32);
  void *second = malloc(32);
  free(first);
  consume(second);
  free(second);
}
