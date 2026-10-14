#include <stdlib.h>

extern void consume(int);

static __attribute__((noinline)) void read_slot(int **slot) {
  int *pointer = *slot;
  consume(*pointer);
}

static int fallback = 7;
static __attribute__((noinline)) void replace_slot(int **slot) {
  *slot = &fallback;
  int *pointer = *slot;
  consume(*pointer);
}

void freed_object_in_slot_is_accessed(void) {
  int *pointer = malloc(sizeof(*pointer));
  if (pointer == NULL)
    return;
  *pointer = 7;
  free(pointer);
  read_slot(&pointer);
}

void live_object_in_slot_is_safe(void) {
  int *pointer = malloc(sizeof(*pointer));
  if (pointer == NULL)
    return;
  *pointer = 7;
  read_slot(&pointer);
  free(pointer);
}

void overwritten_slot_is_safe(void) {
  int *pointer = malloc(sizeof(*pointer));
  int *other = malloc(sizeof(*other));
  if (pointer == NULL || other == NULL) {
    free(pointer);
    free(other);
    return;
  }
  *other = 7;
  int *slot = pointer;
  free(pointer);
  slot = other;
  read_slot(&slot);
  free(other);
}

void callee_replaces_slot_before_access(void) {
  int *pointer = malloc(sizeof(*pointer));
  if (pointer == NULL)
    return;
  free(pointer);
  replace_slot(&pointer);
}
