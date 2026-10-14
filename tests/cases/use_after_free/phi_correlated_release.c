#include <stdlib.h>

extern void consume(int);

void phi_correlated_release(int condition, int *other) {
  int *allocated = (int *)malloc(sizeof(*allocated));
  int *selected;
  if (condition) {
    free(allocated);
    selected = allocated;
  } else {
    selected = other;
  }
  consume(*selected);
  if (!condition)
    free(allocated);
}
