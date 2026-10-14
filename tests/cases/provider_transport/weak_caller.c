#include <stdlib.h>

void pt_replaceable(int *value);

int main(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return 0;
  *value = 17;
  pt_replaceable(value);
  /* No leak: weak_strong_provider.c supplies the actual freeing body. */
  return 0;
}
