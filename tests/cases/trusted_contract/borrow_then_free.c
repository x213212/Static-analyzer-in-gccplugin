#include <stdlib.h>

extern void inspect_without_ownership(void *)
    __attribute__((static_analyzer_borrow(1)));

// Expected after trusted-contract integration:
//   SA001/SA002/SA003 findings: 0
//   ownership-related inconclusive candidates: 0
// Borrowing preserves local ownership, and the following builtin free closes
// the allocation on every non-NULL path.
void borrow_then_free_is_released(void) {
  void *allocation = malloc(32);
  if (allocation == NULL)
    return;
  inspect_without_ownership(allocation);
  free(allocation);
}
