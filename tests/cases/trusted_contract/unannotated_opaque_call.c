#include <stdlib.h>

extern void opaque_pointer_call(int *);

// Expected after trusted-contract integration in proof-only mode:
//   SA003 findings: 0
//   INCONCLUSIVE opaque-call-dereference: 1
// No function spelling or ordinary pointer type may manufacture a trusted
// dereference contract for an unannotated declaration.
void unannotated_opaque_call_stays_inconclusive(void) {
  int *allocation = (int *)malloc(sizeof(*allocation));
  if (allocation == NULL)
    return;
  free(allocation);
  opaque_pointer_call(allocation);
}
