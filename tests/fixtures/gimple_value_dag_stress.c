#include <stdlib.h>

/*
 * Two values at every level share both values from the preceding level.
 * Expanding x30 naively revisits the same provenance sub-DAG exponentially;
 * a memo keyed by (SSA value, remaining depth, derived) visits it linearly.
 * Volatile predicates and the observable final access keep the selects in
 * lowered GIMPLE without depending on testcase or function-name classifiers.
 */
#define MIX(level, previous, first_flag, second_flag)                    \
  void *x##level = flags[first_flag] ? x##previous : y##previous;       \
  void *y##level = flags[second_flag] ? y##previous : x##previous

static volatile int flags[160];

void gimple_value_dag_stress(void) {
  void *x0 = malloc(8);
  void *y0 = malloc(8);

  MIX(1, 0, 0, 30);
  MIX(2, 1, 1, 31);
  MIX(3, 2, 2, 32);
  MIX(4, 3, 3, 33);
  MIX(5, 4, 4, 34);
  MIX(6, 5, 5, 35);
  MIX(7, 6, 6, 36);
  MIX(8, 7, 7, 37);
  MIX(9, 8, 8, 38);
  MIX(10, 9, 9, 39);
  MIX(11, 10, 10, 40);
  MIX(12, 11, 11, 41);
  MIX(13, 12, 12, 42);
  MIX(14, 13, 13, 43);
  MIX(15, 14, 14, 44);
  MIX(16, 15, 15, 45);
  MIX(17, 16, 16, 46);
  MIX(18, 17, 17, 47);
  MIX(19, 18, 18, 48);
  MIX(20, 19, 19, 49);
  MIX(21, 20, 20, 50);
  MIX(22, 21, 21, 51);
  MIX(23, 22, 22, 52);
  MIX(24, 23, 23, 53);
  MIX(25, 24, 24, 54);
  MIX(26, 25, 25, 55);
  MIX(27, 26, 26, 56);
  MIX(28, 27, 27, 57);
  MIX(29, 28, 28, 58);
  MIX(30, 29, 29, 59);

  free(x30);
  *(volatile unsigned char *)y30 = 1;
}

#undef MIX

/* A wide PHI deterministically exercises the origin cap and its unknown
 * sentinel without requiring a deep recursion chain. */
static unsigned char objects[130];

#define PICK_CASE(index)                                                  \
  case index:                                                             \
    selected = &objects[index];                                           \
    break
#define PICK_TEN(base)                                                    \
  PICK_CASE((base) + 0);                                                  \
  PICK_CASE((base) + 1);                                                  \
  PICK_CASE((base) + 2);                                                  \
  PICK_CASE((base) + 3);                                                  \
  PICK_CASE((base) + 4);                                                  \
  PICK_CASE((base) + 5);                                                  \
  PICK_CASE((base) + 6);                                                  \
  PICK_CASE((base) + 7);                                                  \
  PICK_CASE((base) + 8);                                                  \
  PICK_CASE((base) + 9)

void gimple_value_origin_cap_stress(void) {
  void *selected;
  switch (flags[159]) {
    PICK_TEN(0);
    PICK_TEN(10);
    PICK_TEN(20);
    PICK_TEN(30);
    PICK_TEN(40);
    PICK_TEN(50);
    PICK_TEN(60);
    PICK_TEN(70);
    PICK_TEN(80);
    PICK_TEN(90);
    PICK_TEN(100);
    PICK_TEN(110);
    PICK_TEN(120);
    default:
      selected = malloc(8);
      break;
  }
  free(selected);
}

#undef PICK_TEN
#undef PICK_CASE
