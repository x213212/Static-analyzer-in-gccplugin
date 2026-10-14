#include <stdlib.h>

/* Each generated function contributes one structurally possible but
   contradictory
   free-then-use candidate.  The first 256 queries are proved infeasible;
   later candidates must abstain as UNKNOWN after the TU query cap, rather
   than extending compile time or manufacturing an infeasibility proof.  */
#define DEFINE_CASE(NAME)                                                  \
  __attribute__((noinline, optimize("O0"))) void NAME(int gate) {          \
    int *pointer = (int *)malloc(sizeof(*pointer));                         \
    if (gate)                                                              \
      free(pointer);                                                       \
    if (!gate)                                                             \
      *pointer = 1;                                                        \
  }

#define DEFINE_16(PREFIX)                                                  \
  DEFINE_CASE(feasibility_budget_##PREFIX##0)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##1)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##2)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##3)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##4)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##5)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##6)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##7)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##8)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##9)                              \
  DEFINE_CASE(feasibility_budget_##PREFIX##10)                             \
  DEFINE_CASE(feasibility_budget_##PREFIX##11)                             \
  DEFINE_CASE(feasibility_budget_##PREFIX##12)                             \
  DEFINE_CASE(feasibility_budget_##PREFIX##13)                             \
  DEFINE_CASE(feasibility_budget_##PREFIX##14)                             \
  DEFINE_CASE(feasibility_budget_##PREFIX##15)

DEFINE_16(A)
DEFINE_16(B)
DEFINE_16(C)
DEFINE_16(D)
DEFINE_16(E)
DEFINE_16(F)
DEFINE_16(G)
DEFINE_16(H)
DEFINE_16(I)
DEFINE_16(J)
DEFINE_16(K)
DEFINE_16(L)
DEFINE_16(M)
DEFINE_16(N)
DEFINE_16(O)
DEFINE_16(P)
DEFINE_16(Q)
DEFINE_16(R)
DEFINE_16(S)
DEFINE_16(T)
