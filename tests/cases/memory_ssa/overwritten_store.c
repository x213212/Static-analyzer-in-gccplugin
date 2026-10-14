#include <stdlib.h>

struct pointer_holder {
  int *value;
};

__attribute__((noinline, optimize("O0")))
void memory_overwritten_store(void) {
  struct pointer_holder holder;
  int *first = (int *)malloc(sizeof(*first));
  int *second = (int *)malloc(sizeof(*second));
  holder.value = first;
  holder.value = second;

  int *loaded = holder.value;
  free(loaded);
  free(first);
}
