#include <stdlib.h>

__attribute__((noinline))
static void maybe_release(void *value, int release) {
  if (release)
    free(value);
}

/* The finite prefix must be replayed into the more expressive SMT frame. */
void zero_prefix_then_smt(int guard) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (guard == 0) {
    if (guard > 0)
      return;
    free(value);
  } else {
    free(value);
  }
}

/* Inside an existing SMT frame zero predicates must use that same solver. */
void smt_prefix_then_zero(int guard) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (guard > 0) {
    if (guard == 0)
      return;
    free(value);
  } else {
    free(value);
  }
}

void zero_prefix_then_nonzero_constant(int guard) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (guard == 0) {
    if (guard == -1)
      return;
    free(value);
  } else {
    free(value);
  }
}

/* A sibling must not inherit the first arm's guard!=0 constraint. */
void finite_sibling_rollback(int guard, int release) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (guard != 0) {
    maybe_release(value, release);
  } else {
    if (guard == 0)
      return;
    free(value);
  }
}

/* Distinct formal parameters have independent exact identities. */
void finite_independent_parameters(int first, int second) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (first == 0 && second != 0)
    return;
  free(value);
}
