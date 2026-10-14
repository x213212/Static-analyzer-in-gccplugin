#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static __attribute__((noinline)) void observe_slot(char **slot) {
  char *value = *slot;
  (void)value;
}

static __attribute__((noinline)) void release_slot(char **slot) {
  free(*slot);
}

void string_output_and_local_slot_leak(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  strcpy(pointer, "output");
  puts(pointer);
  observe_slot(&pointer);
}

void returned_copy_and_local_slot_leak(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  char *alias = strcpy(pointer, "output");
  observe_slot(&alias);
}

void output_then_contained_release(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  strcpy(pointer, "output");
  puts(pointer);
  release_slot(&pointer);
}
