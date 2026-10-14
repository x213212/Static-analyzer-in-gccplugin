#include <stddef.h>
#include <stdlib.h>

extern void consume(int *);

void alias_flow(int condition) {
  int *first = malloc(sizeof(*first));
  int *same = first;
  int *second = malloc(sizeof(*second));
  int *merged = condition ? same : second;

  consume(same);
  consume(merged);
  free(same);
  free(second);
}
