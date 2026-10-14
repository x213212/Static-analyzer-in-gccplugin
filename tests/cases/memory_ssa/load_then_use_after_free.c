#include <stdlib.h>

struct pointer_holder {
  int *value;
};

__attribute__((noinline, optimize("O0")))
void memory_load_then_use_after_free(void) {
  struct pointer_holder holder;
  int *allocated = (int *)malloc(sizeof(*allocated));
  holder.value = allocated;

  int *loaded = holder.value;
  free(allocated);
  *loaded = 7;
}
