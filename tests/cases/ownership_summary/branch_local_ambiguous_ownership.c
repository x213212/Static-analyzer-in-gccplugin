#include <stdlib.h>

__attribute__((noinline))
static void maybe_release(void *value, int release) {
  if (release)
    free(value);
}

/* The skip arm is an exact leak witness. The other arm is opaque to the
   caller's ownership proof and must not invalidate this independent path. */
void branch_skips_ambiguous_call(int enter, int release) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (enter)
    maybe_release(value, release);
}

/* The exact source guard permits a caller witness with release == 0. */
void unavoidable_ambiguous_call(int release) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  maybe_release(value, release);
}

/* The bypass is consumed; enter != 0 and release == 0 prove a leak. */
void bypass_is_released(int enter, int release) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (enter)
    maybe_release(value, release);
  else
    free(value);
}

/* Exactly one call runs; its source guard proves preservation at release == 0. */
void complementary_calls(int enter, int release) {
  void *value = malloc(32);
  if (value == NULL)
    return;
  if (enter)
    maybe_release(value, release);
  if (!enter)
    maybe_release(value, release);
}
