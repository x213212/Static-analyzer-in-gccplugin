#include <stdlib.h>

void inline_asm_ownership_is_inconclusive(void) {
  void *value = malloc(8);
  __asm__ volatile("" : : "r"(value) : "memory");
}
