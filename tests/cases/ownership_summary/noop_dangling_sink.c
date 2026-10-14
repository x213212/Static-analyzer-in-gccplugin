#include <stdlib.h>

static void no_op(void *value) { (void)value; }

void noop_dangling_sink(void) {
  void *value = malloc(32);
  free(value);
  no_op(value);
}
