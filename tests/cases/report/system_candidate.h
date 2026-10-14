#pragma GCC system_header

#include <stdlib.h>

static __attribute__((noinline, used)) void system_header_candidate(void) {
  void *pointer = malloc(16);
  __asm__ volatile("" : : "r"(pointer) : "memory");
}
