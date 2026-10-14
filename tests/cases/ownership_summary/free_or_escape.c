#include <stdlib.h>

void *escaped;

static void consume(void *value, int release) {
  if (release)
    free(value);
  else
    escaped = value;
}

void free_or_escape(int release) {
  void *value = malloc(32);
  consume(value, release);
}
