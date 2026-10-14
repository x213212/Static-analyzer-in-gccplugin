#include <stddef.h>
#include <stdlib.h>

#if __GNUC__ >= 11
extern void *project_heap_allocate(size_t)
    __attribute__((malloc(free, 1)));
#else
// GCC 10 accepts only the zero-argument malloc attribute.  A body-bound
// wrapper gives the plugin equivalent proof without weakening production
// classification for that compiler.
static __attribute__((noinline)) void *project_heap_allocate(size_t size) {
  return malloc(size);
}
#endif

void free_compatible_allocator_leak(void) {
  unsigned char *pointer = project_heap_allocate(32);
  pointer[0] = 1;
}
