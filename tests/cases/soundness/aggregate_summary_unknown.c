#include <stdlib.h>

struct pointer_holder {
  void *value;
};

extern void opaque_holder_sink(struct pointer_holder *);

__attribute__((noinline, noclone))
void aggregate_wrapper(void *value) {
  struct pointer_holder holder = {value};
  opaque_holder_sink(&holder);
}

void aggregate_wrapper_caller(void) {
  void *allocation = malloc(16);
  aggregate_wrapper(allocation);
}
