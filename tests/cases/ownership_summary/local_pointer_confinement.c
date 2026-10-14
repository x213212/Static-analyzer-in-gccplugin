#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct pointer_carrier {
  int *value;
};

static __attribute__((noinline)) void observe_slot(int **slot) {
  int *value = *slot;
  (void)value;
}

static __attribute__((noinline)) void
observe_carrier(struct pointer_carrier carrier) {
  int *value = carrier.value;
  (void)value;
}

static __attribute__((noinline)) void release_slot(int **slot) {
  free(*slot);
}

static __attribute__((noinline)) void release_bits(uintptr_t bits) {
  free((void *)bits);
}

static __attribute__((noinline)) void release_opaque_slot(void *slot) {
  uintptr_t bits;
  memcpy(&bits, slot, sizeof(bits));
  release_bits(bits);
}

/* Two real leaks: the local container and every bound consumer are closed.
   A direct-pointer borrow fact alone would not establish either theorem. */
void confined_local_slot_leaks(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  *value = 5;
  observe_slot(&value);
}

void confined_local_carrier_leaks(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  struct pointer_carrier carrier = {value};
  observe_carrier(carrier);
}

/* Four released allocations: none may gain SA001 when spill conservatism
   is removed. The representation cases use GCC's uintptr_t round-trip. */
void contained_release_is_not_confined(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  release_slot(&value);
}

void pointer_integer_escape_is_not_confined(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  observe_slot(&value);
  release_bits((uintptr_t)value);
}

void copied_pointer_bits_are_not_confined(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  uintptr_t bits;
  memcpy(&bits, &value, sizeof(bits));
  release_bits(bits);
}

void opaque_slot_representation_is_not_confined(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  release_opaque_slot(&value);
}
