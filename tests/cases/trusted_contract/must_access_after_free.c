#include <stdlib.h>

extern void unconditionally_access(const int *)
    __attribute__((static_analyzer_must_access(1)));

// Expected after trusted-contract integration:
//   SA003 use after free: 1, at the call below
//   SA001/SA002 findings: 0
// The must-access promise is access evidence even though it says nothing about
// whether the callee captures or otherwise changes ownership.
void must_access_after_free(void) {
  int *allocation = (int *)malloc(sizeof(*allocation));
  if (allocation == NULL)
    return;
  free(allocation);
  unconditionally_access(allocation);
}
