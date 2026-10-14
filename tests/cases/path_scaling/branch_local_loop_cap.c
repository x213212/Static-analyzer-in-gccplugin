#include <stdlib.h>

extern void tick(void);
static volatile int observed;

/* The repeat!=0 branch is cyclic, but repeat==0 is an exact finite sibling
   path to the release/use sequence.  Exhausting the former branch's visit
   cap must not terminate the whole existential search. */
void branch_local_loop_cap(int repeat) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  while (repeat)
    tick();
  free(pointer);
  observed = pointer[0];
}
