#include <stdlib.h>

extern void take_interior_pointer(unsigned char *);

unsigned char *return_interior_pointer(void) {
  unsigned char *allocation = (unsigned char *)malloc(16);
  return allocation + 1;
}

void pass_interior_pointer_to_opaque_callee(void) {
  unsigned char *allocation = (unsigned char *)malloc(16);
  take_interior_pointer(allocation + 1);
}

void release_through_volatile_local_slot(void) {
  void *allocation = malloc(16);
  void *volatile slot = allocation;
  free(slot);
}
