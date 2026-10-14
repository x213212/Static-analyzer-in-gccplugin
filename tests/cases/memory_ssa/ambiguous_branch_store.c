#include <stdlib.h>

extern void consume(void *);

struct pointer_holder {
  int *value;
};

__attribute__((noinline, optimize("O0")))
void memory_ambiguous_branch_store(int choose_first) {
  struct pointer_holder holder;
  int *first = (int *)malloc(sizeof(*first));
  int *second = (int *)malloc(sizeof(*second));
  if (choose_first)
    holder.value = first;
  else
    holder.value = second;

  int *loaded = holder.value;
  consume(loaded);

  // The load has two reaching stores.  It must stay a may/unknown fact rather
  // than being promoted to must-alias with either allocation.
  free(first);
  free(second);
}
