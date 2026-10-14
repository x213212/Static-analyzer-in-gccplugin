#include <stdlib.h>

extern void access_with_unknown_ownership(void *)
    __attribute__((static_analyzer_must_access(1)));

// Expected after trusted-contract integration in proof-only mode:
//   SA001 findings: 0
//   INCONCLUSIVE opaque-ownership-transfer: 1
// must-access proves a lifetime use only.  Without a separate borrow contract,
// the callee may still free, capture, or publish the allocation, so local leak
// ownership remains unknown.
void access_contract_is_not_an_ownership_contract(void) {
  void *allocation = malloc(32);
  if (allocation == NULL)
    return;
  access_with_unknown_ownership(allocation);
}
