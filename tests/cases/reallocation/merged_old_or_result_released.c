#include <stdlib.h>

void merged_old_or_result_is_released(size_t size) {
  unsigned char *data = (unsigned char *)malloc(8);
  if (data == NULL)
    return;

  unsigned char *replacement =
      (unsigned char *)realloc(data, size);
  if (replacement != NULL)
    data = replacement;

  // On realloc failure DATA is the still-live old object.  On success it is
  // the replacement object.  The lowered GIMPLE PHI therefore releases one
  // exact allocation on each incoming edge.
  free(data);
}
