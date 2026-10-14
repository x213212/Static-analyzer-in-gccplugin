#include <stdlib.h>

void merged_result_can_lose_old_object(size_t size) {
  unsigned char *old_buffer = (unsigned char *)malloc(8);
  if (old_buffer == NULL)
    return;

  unsigned char *replacement =
      (unsigned char *)realloc(old_buffer, size);
  unsigned char *selected = NULL;
  if (replacement != NULL)
    selected = replacement;

  // The success arm releases the replacement, but the NULL failure arm does
  // not release OLD_BUFFER.  An edge-correlated PHI proof must retain that
  // real failure-path leak.
  free(selected);
}
