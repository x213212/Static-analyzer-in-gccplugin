#include <stddef.h>
#include <stdlib.h>

extern void *project_allocate(size_t) __attribute__((malloc));

void malloc_attribute_leak(void) {
  unsigned char *pointer = project_allocate(32);
  pointer[0] = 1;
}

void builtin_aligned_alloc_released(void) {
  void *pointer = aligned_alloc(16, 32);
  free(pointer);
}
