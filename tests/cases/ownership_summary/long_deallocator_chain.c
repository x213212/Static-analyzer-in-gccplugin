#include <stdlib.h>

static void release_00(void *value) { free(value); }
static void release_01(void *value) { release_00(value); }
static void release_02(void *value) { release_01(value); }
static void release_03(void *value) { release_02(value); }
static void release_04(void *value) { release_03(value); }
static void release_05(void *value) { release_04(value); }
static void release_06(void *value) { release_05(value); }
static void release_07(void *value) { release_06(value); }
static void release_08(void *value) { release_07(value); }
static void release_09(void *value) { release_08(value); }
static void release_10(void *value) { release_09(value); }
static void release_11(void *value) { release_10(value); }
static void release_12(void *value) { release_11(value); }
static void release_13(void *value) { release_12(value); }
static void release_14(void *value) { release_13(value); }
static void release_15(void *value) { release_14(value); }

void long_deallocator_chain(void) {
  void *value = malloc(32);
  release_15(value);
}
