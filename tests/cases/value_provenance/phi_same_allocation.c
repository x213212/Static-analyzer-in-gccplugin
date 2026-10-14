#include <stdlib.h>

extern void inspect_byte(unsigned char);

void phi_arms_from_same_allocation(int choose_offset) {
  unsigned char *buffer = (unsigned char *)malloc(2);
  if (buffer == NULL)
    return;

  unsigned char *selected = choose_offset ? buffer + 1 : buffer;
  free(buffer);
  inspect_byte(*selected);
}
