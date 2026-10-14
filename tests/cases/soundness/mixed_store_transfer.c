#include <stdlib.h>

void *published_pointer;

void derived_pointer_store_or_free(int publish) {
  char *allocation = (char *)malloc(16);
  char *selected = allocation + publish;
  published_pointer = selected;
  if (publish == 0)
    free(allocation);
}
