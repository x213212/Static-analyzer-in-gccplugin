#include <stdlib.h>

static __attribute__((noinline)) void *identity(void *value) {
  return value;
}

void *returned_parameter_escapes(void) {
  void *value = malloc(8);
  return identity(value);
}
