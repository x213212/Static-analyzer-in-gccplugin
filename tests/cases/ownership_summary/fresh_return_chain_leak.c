#include <stdlib.h>

static void *fresh_leaf(void) { return malloc(32); }

static void *fresh_wrapper(void) { return fresh_leaf(); }

void fresh_return_chain_leak(void) {
  void *value = fresh_wrapper();
  (void)value;
}
