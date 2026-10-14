#include <stdlib.h>

void guarded_reallocation_cleanup(size_t size) {
  unsigned char *old_buffer = (unsigned char *)malloc(8);
  if (old_buffer == NULL)
    return;

  unsigned char *new_buffer =
      (unsigned char *)realloc(old_buffer, size);
  if (new_buffer != NULL)
    free(new_buffer);
  else
    free(old_buffer);
}
