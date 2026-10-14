#include <stdlib.h>
#include <string.h>

extern void inspect_without_ownership(void *)
    __attribute__((static_analyzer_borrow(1)));

// The literal is a second pointer argument in the same memcpy statement, but
// it has no allocation provenance.  Ownership effects must be matched to the
// exact actual argument rather than inherited from another operand in the
// statement.
void multi_pointer_argument_identity(void) {
  void *allocation = malloc(32);
  if (allocation == NULL)
    return;
  memcpy(allocation, "copy", 5);
  inspect_without_ownership(allocation);
}
