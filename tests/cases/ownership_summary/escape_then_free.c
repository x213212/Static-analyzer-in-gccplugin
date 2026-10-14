#include <stdlib.h>

void *published_dangling;

static void publish_then_release(void *value) {
  published_dangling = value;
  free(value);
}

void escape_then_free_is_still_a_release(void) {
  void *value = malloc(32);
  publish_then_release(value);
  free(value);
}
