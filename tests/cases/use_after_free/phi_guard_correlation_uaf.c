#include <stdlib.h>

extern void consume(int);

/* The matching feasible witness: q can name p, and p is unconditionally
   released before the dereference.  The PHI-constrained path must therefore
   remain reportable.  */
__attribute__((noinline, optimize("O0")))
void phi_guard_correlation_uaf(int condition) {
  int *p = (int *)malloc(sizeof(*p));
  int *other = (int *)malloc(sizeof(*other));
  int *q;

  if (condition)
    q = p;
  else
    q = other;

  free(p);
  consume(*q);
  free(other);
}
