#include <stddef.h>
#include <stdlib.h>

static unsigned char pool[64];

static __attribute__((noinline)) void *xmalloc(size_t size) {
  (void)size;
  return pool;
}

static __attribute__((noinline)) void xfree(void *value) {
  (void)value;
}

void xmalloc_name_is_not_an_allocation_contract(void) {
  void *value = xmalloc(8);
  (void)value;
}

void xfree_name_is_not_a_release_contract(void) {
  void *value = malloc(8);
  xfree(value);
  xfree(value);
}
