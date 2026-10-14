#include <stdlib.h>

/* Performance-only fixture: expand one identical ownership question into
   2,048 GIMPLE release calls without maintaining a generated source file.
   Every release returns, so no dynamic path contains a double free. */
extern int choose_release(void);

#define RELEASE_1(pointer)                         \
  do {                                             \
    if (choose_release()) {                        \
      free(pointer);                               \
      return;                                      \
    }                                              \
  } while (0)
#define RELEASE_2(pointer) RELEASE_1(pointer); RELEASE_1(pointer)
#define RELEASE_4(pointer) RELEASE_2(pointer); RELEASE_2(pointer)
#define RELEASE_8(pointer) RELEASE_4(pointer); RELEASE_4(pointer)
#define RELEASE_16(pointer) RELEASE_8(pointer); RELEASE_8(pointer)
#define RELEASE_32(pointer) RELEASE_16(pointer); RELEASE_16(pointer)
#define RELEASE_64(pointer) RELEASE_32(pointer); RELEASE_32(pointer)
#define RELEASE_128(pointer) RELEASE_64(pointer); RELEASE_64(pointer)
#define RELEASE_256(pointer) RELEASE_128(pointer); RELEASE_128(pointer)
#define RELEASE_512(pointer) RELEASE_256(pointer); RELEASE_256(pointer)
#define RELEASE_1024(pointer) RELEASE_512(pointer); RELEASE_512(pointer)
#define RELEASE_2048(pointer) RELEASE_1024(pointer); RELEASE_1024(pointer)

void alias_cache_stress(void) {
  void *pointer = malloc(32);
  RELEASE_2048(pointer);
}
