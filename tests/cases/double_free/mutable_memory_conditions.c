#include <stdlib.h>

extern void mutate_flag(int *flag);

// Both reads of flag come from memory and are separated by an opaque call.
// They are different Memory-SSA versions: mutate_flag may make the first read
// zero and the second nonzero, so the two frees can execute on one path.
__attribute__((noinline, optimize("O0")))
void mutable_memory_conditions(int initial) {
  void *pointer = malloc(32);
  int flag = initial;

  mutate_flag(&flag);
  if (flag == 0)
    free(pointer);

  mutate_flag(&flag);
  if (flag != 0)
    free(pointer);
}
