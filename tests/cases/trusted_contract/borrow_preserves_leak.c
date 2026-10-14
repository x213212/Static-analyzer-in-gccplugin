#include <stdlib.h>

extern void inspect_without_ownership(void *)
    __attribute__((static_analyzer_borrow(1)));

// Expected after trusted-contract integration:
//   SA001 memory leak: 1
//   SA002/SA003 findings: 0
// A borrow contract proves that the call neither frees nor captures the
// allocation, so the object is still owned by this function at normal exit.
void borrow_preserves_leak(void) {
  void *allocation = malloc(32);
  if (allocation == NULL)
    return;
  inspect_without_ownership(allocation);
}
