#include <stdlib.h>

extern void inspect_byte(unsigned char);

void leaked_reallocation_result(void) {
  unsigned char *buffer = (unsigned char *)realloc(NULL, 32);
  if (buffer != NULL)
    inspect_byte(buffer[0]);
}
