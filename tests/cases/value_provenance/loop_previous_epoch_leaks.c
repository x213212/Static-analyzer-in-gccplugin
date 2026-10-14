#include <stdlib.h>

extern int opaque_choice(void);

void loop_previous_epoch_leaks(void) {
  char *previous = NULL;

  while (opaque_choice()) {
    char *current = (char *)malloc(8);

    /* PREVIOUS can be the allocation from the prior iteration, but it is not
       the allocation epoch created immediately above.  The final CURRENT is
       therefore leaked when the loop exits. */
    free(previous);
    previous = current;
  }
}
