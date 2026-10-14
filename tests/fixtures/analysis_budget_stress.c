#include <stdlib.h>

/* 512 independent static allocation sites force the lifetime phase's
   allocation-by-statement relation above its per-function work cap.  Every
   pointer is released by the loop; budget exhaustion must therefore produce
   zero warnings and an explicit INCONCLUSIVE record, never a partial leak. */
#define ALLOC_1(n) slots[(n)] = malloc(8);
#define ALLOC_2(n) ALLOC_1(n) ALLOC_1((n) + 1)
#define ALLOC_4(n) ALLOC_2(n) ALLOC_2((n) + 2)
#define ALLOC_8(n) ALLOC_4(n) ALLOC_4((n) + 4)
#define ALLOC_16(n) ALLOC_8(n) ALLOC_8((n) + 8)
#define ALLOC_32(n) ALLOC_16(n) ALLOC_16((n) + 16)
#define ALLOC_64(n) ALLOC_32(n) ALLOC_32((n) + 32)
#define ALLOC_128(n) ALLOC_64(n) ALLOC_64((n) + 64)
#define ALLOC_256(n) ALLOC_128(n) ALLOC_128((n) + 128)
#define ALLOC_512(n) ALLOC_256(n) ALLOC_256((n) + 256)

void analysis_budget_stress(void) {
  void *slots[512];
  ALLOC_512(0)
  for (unsigned int index = 0; index < 512; ++index)
    free(slots[index]);
}
