#include <stdlib.h>

extern void unknown_effect(void *value);

__attribute__((noinline))
static void release_when_nonzero(void *value, int selector) {
  if (selector)
    free(value);
}

__attribute__((noinline))
static void release_when_zero(void *value, int selector) {
  if (!selector)
    free(value);
}

/* A public entry admits selector == 0, which preserves the allocation. */
void conditional_borrow_leak(int selector) {
  void *value = malloc(32);
  if (value == NULL) return;
  release_when_nonzero(value, selector);
}

void literal_borrow_leak(void) {
  void *value = malloc(32);
  if (value == NULL) return;
  release_when_nonzero(value, 0);
}

void reversed_borrow_leak(int selector) {
  void *value = malloc(32);
  if (value == NULL) return;
  release_when_zero(value, selector);
}

/* The caller's condition contradicts the callee's borrow condition. */
void correlated_release_safe(int selector) {
  void *value = malloc(32);
  if (value == NULL) return;
  if (selector)
    release_when_nonzero(value, selector);
  else
    free(value);
}

/* Both borrow guards cannot hold together. Exactly one call releases. */
void opposite_guards_safe(int selector) {
  void *value = malloc(32);
  if (value == NULL) return;
  release_when_nonzero(value, selector);
  release_when_zero(value, selector);
}

void literal_release_safe(void) {
  void *value = malloc(32);
  if (value == NULL) return;
  release_when_nonzero(value, 1);
}

/* A private caller does not establish an arbitrary selector input. */
__attribute__((noinline))
static void private_release(int selector) {
  void *value = malloc(32);
  if (value == NULL) return;
  release_when_nonzero(value, selector);
}

void invoke_private_safe(void) {
  private_release(1);
}

/* Extra opaque effects invalidate the conditional-borrow certificate. */
__attribute__((noinline))
static void opaque_borrow(void *value, int selector) {
  if (selector)
    free(value);
  else
    unknown_effect(value);
}

void opaque_branch_unknown(int selector) {
  void *value = malloc(32);
  if (value == NULL) return;
  opaque_borrow(value, selector);
}
