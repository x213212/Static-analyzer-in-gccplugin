#include <stdlib.h>

extern void access_nonnull_pointer(const int *)
    __attribute__((static_analyzer_access_if_nonnull(1)));

// Expected after trusted-contract integration:
//   SA003 use after free: 1, at the call below
//   SA001/SA002 findings: 0
// Reaching the release and call implies that the allocation is non-NULL, so
// the conditional trusted access is enabled on a feasible caller path.
void access_if_nonnull_after_free(void) {
  int *allocation = (int *)malloc(sizeof(*allocation));
  if (allocation == NULL)
    return;
  free(allocation);
  access_nonnull_pointer(allocation);
}
