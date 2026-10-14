#include <stdlib.h>

static void recursive_release(void *value, unsigned int depth) {
  if (depth != 0)
    recursive_release(value, depth - 1);
  else
    free(value);
}

void recursive_summary_stays_unknown(unsigned int depth) {
  void *value = malloc(32);
  recursive_release(value, depth);
}
