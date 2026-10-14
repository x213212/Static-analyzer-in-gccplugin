#include <stdlib.h>

extern void consume(int);

/* Keep the two correlated tests visible in late GIMPLE so this remains a
   regression for the feasibility engine rather than an optimizer test.  On
   every execution either q names p and p is still live, or p is released and
   q names other.  A solver that forgets the PHI edge can nevertheless combine
   the release arm with the q == p arm and invent a use-after-free.  */
__attribute__((noinline, optimize("O0")))
void phi_guard_correlation_safe(int condition) {
  int *p = (int *)malloc(sizeof(*p));
  int *other = (int *)malloc(sizeof(*other));
  int *q;

  if (condition)
    q = p;
  else
    q = other;

  if (!condition)
    free(p);

  consume(*q);

  if (condition)
    free(p);
  free(other);
}
