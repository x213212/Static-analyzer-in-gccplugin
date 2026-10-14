#include <stdlib.h>

static int *fresh_or_borrowed(int *borrowed, int choose_fresh) {
  if (choose_fresh)
    return (int *)malloc(sizeof(int));
  return borrowed;
}

void fresh_or_borrowed_is_not_an_allocator(int choose_fresh) {
  int local = 0;
  int *value = fresh_or_borrowed(&local, choose_fresh);
  (void)value;
}
