#include <stdlib.h>

extern void inspect_byte(unsigned char);

void failed_reallocation_keeps_old_object(size_t size) {
  unsigned char *old_buffer = (unsigned char *)malloc(8);
  if (old_buffer == NULL)
    return;

  unsigned char *new_buffer =
      (unsigned char *)realloc(old_buffer, size);
  if (new_buffer == NULL) {
    inspect_byte(old_buffer[0]);
    free(old_buffer);
    return;
  }
  free(new_buffer);
}
