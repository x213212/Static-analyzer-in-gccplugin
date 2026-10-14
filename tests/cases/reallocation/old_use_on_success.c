#include <stdlib.h>

extern void inspect_byte(unsigned char);

void successful_reallocation_invalidates_old_object(size_t size) {
  unsigned char *old_buffer = (unsigned char *)malloc(8);
  if (old_buffer == NULL)
    return;

  unsigned char *new_buffer =
      (unsigned char *)realloc(old_buffer, size);
  if (new_buffer != NULL) {
    inspect_byte(old_buffer[0]);
    free(new_buffer);
  } else {
    free(old_buffer);
  }
}
