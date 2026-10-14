#include <stdlib.h>

void direct_leak(void) {
  unsigned char *pointer = (unsigned char *)malloc(32);
  pointer[0] = 1;
}
