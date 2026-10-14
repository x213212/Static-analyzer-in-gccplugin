#include <stdlib.h>

static volatile int observed;

/* All syntactic returns are 1, but this particular invocation cannot return.
   A return-value summary must prove totality, not merely inspect returns. */
__attribute__((noinline)) static int constant_or_spin(int spin) {
  if (spin) {
    for (;;) {
    }
  }
  return 1;
}

/* Totality cannot be inferred through a nested call either.  The leaf is
   deliberately non-returning, while the outer function has a plain return. */
__attribute__((noinline)) static void spin_forever(void) {
  for (;;) {
  }
}

__attribute__((noinline)) static int
call_spin_or_return(int spin) {
  if (spin)
    spin_forever();
  return 1;
}

/* A returns-twice operation makes local control flow non-standard even when
   the enclosing helper's only GIMPLE_RETURN is a constant. */
__attribute__((noinline, returns_twice)) static int
returns_twice_leaf(void) {
  return 0;
}

__attribute__((noinline)) static int
call_returns_twice_then_return(void) {
  (void)returns_twice_leaf();
  return 1;
}

/* __builtin_return can return an arbitrary saved result without executing a
   normal GIMPLE_RETURN.  The argument is intentionally opaque: this fixture
   checks the summary's control-flow gate, not runtime use of the builtin. */
__attribute__((noinline)) static int
builtin_return_or_constant(int escape) {
  void *saved = __builtin_apply_args();
  if (escape)
    __builtin_return(saved);
  return 1;
}

void constant_return_spin_must_remain_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (constant_or_spin(1))
    observed = pointer[0];
}

void nested_spin_must_remain_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (call_spin_or_return(1))
    observed = pointer[0];
}

void nested_returns_twice_must_remain_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (call_returns_twice_then_return())
    observed = pointer[0];
}

void builtin_return_must_remain_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (builtin_return_or_constant(1))
    observed = pointer[0];
}
