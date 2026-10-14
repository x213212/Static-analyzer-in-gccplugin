#include <stdlib.h>

static volatile int observed;

/* The manifest disables IPA constant propagation and noinline keeps the call
   in the caller, so this exercises the plugin's bounded body summary. */
__attribute__((noinline)) static int returns_true(void) { return 1; }

__attribute__((noinline)) static int returns_false(void) { return 0; }

void direct_constant_return_exposes_uaf(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (returns_true())
    observed = pointer[0];
}

/* The inverse constant is an adversarial control: expanding a known callee
   must also be able to prove that a syntactic sink is unreachable. */
void direct_constant_return_prunes_safe_sink(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (returns_false())
    observed = pointer[0];
}
