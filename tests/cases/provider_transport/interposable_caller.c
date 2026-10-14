#include <stdlib.h>
#include "fixture.h"

void pt_interposable(int *value);

/* Build as a shared library, not an executable with locally bound symbols. */
PT_NOINLINE void pt_interposable_client(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  *value = 29;
  pt_interposable(value);
  /* This may leak for the default provider, but an interposed provider may
     free value.  Reject a proof-only leak based on the unbound local body. */
}
