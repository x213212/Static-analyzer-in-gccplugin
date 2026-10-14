#include <cstdlib>

static volatile int observed;

/* The exceptional path never reaches the caller's condition.  A constant
   normal return does not authorize treating the invocation as total. */
__attribute__((noinline)) static int
constant_or_throw(bool should_throw) {
  if (should_throw)
    throw 1;
  return 1;
}

void exceptional_constant_return_must_remain_unknown() {
  int *pointer = static_cast<int *>(std::malloc(sizeof(*pointer)));
  std::free(pointer);
  if (constant_or_throw(true))
    observed = pointer[0];
}
