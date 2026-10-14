#include <stdlib.h>

static volatile int observed;

__attribute__((noinline)) static int inconsistent_return(int select) {
  if (select)
    return 1;
  return 0;
}

/* Every syntactic return is 1, but the body is recursive.  A return scan that
   ignores its call graph would accidentally accept this function. */
__attribute__((noinline)) static int recursive_return(unsigned depth) {
  if (depth != 0)
    (void)recursive_return(depth - 1);
  return 1;
}

/* A definition in this TU does not make an interposable weak symbol the
   program's final definition. */
__attribute__((weak, noinline, noipa)) int weak_return(void) { return 1; }

/* The same applies to an ordinary externally visible, replaceable symbol. */
__attribute__((noinline, noipa)) int interposable_return(void) { return 1; }

__attribute__((returns_twice, noinline)) static int
returns_twice_return(void) {
  return 1;
}

static int ifunc_implementation(void) { return 1; }

static int (*ifunc_resolver(void))(void) { return ifunc_implementation; }

int ifunc_return(void) __attribute__((ifunc("ifunc_resolver")));

void inconsistent_return_remains_unknown(int select) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (inconsistent_return(select))
    observed = pointer[0];
}

void recursive_return_remains_unknown(unsigned depth) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (recursive_return(depth))
    observed = pointer[0];
}

void weak_return_remains_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (weak_return())
    observed = pointer[0];
}

void interposable_return_remains_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (interposable_return())
    observed = pointer[0];
}

void returns_twice_return_remains_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (returns_twice_return())
    observed = pointer[0];
}

void ifunc_return_remains_unknown(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  if (ifunc_return())
    observed = pointer[0];
}
