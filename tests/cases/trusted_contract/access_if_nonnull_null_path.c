#include <stdlib.h>

extern void access_nonnull_pointer(const int *)
    __attribute__((static_analyzer_access_if_nonnull(1)));

// Expected after trusted-contract integration:
//   SA001/SA002/SA003 findings: 0
// The call is reachable only on the malloc == NULL path.  free(NULL) is a
// no-op, and the access-if-nonnull guard is false, so it must not become UAF.
void access_if_nonnull_only_on_null_path(void) {
  int *allocation = (int *)malloc(sizeof(*allocation));
  if (allocation == NULL) {
    free(allocation);
    access_nonnull_pointer(allocation);
    return;
  }
  free(allocation);
}
