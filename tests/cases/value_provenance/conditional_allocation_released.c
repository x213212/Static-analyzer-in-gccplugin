#include <stdlib.h>

extern int opaque_choice(void);

void conditional_allocation_released(void) {
  char *data = NULL;
  if (opaque_choice())
    data = (char *)malloc(8);

  /* The bypass edge contributes NULL to the PHI, but that edge cannot be
     reached after this function's allocation event.  On every execution on
     which the allocation exists, DATA denotes that allocation here. */
  free(data);
}
