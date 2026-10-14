#include <stdlib.h>

extern void consume(int);

void phi_disjoint_release(int condition, int *other) {
  int *allocated = (int *)malloc(sizeof(*allocated));
  int *selected;
  if (condition) {
    free(allocated);
    selected = other;
  } else {
    selected = allocated;
  }
  consume(*selected);
  if (!condition)
    free(allocated);
}
