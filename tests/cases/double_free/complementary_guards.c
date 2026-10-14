#include <stdlib.h>

__attribute__((noinline, optimize("O0")))
void complementary_guard_frees(int condition) {
  void *pointer = malloc(32);
  if (condition)
    free(pointer);
  if (!condition)
    free(pointer);
}
