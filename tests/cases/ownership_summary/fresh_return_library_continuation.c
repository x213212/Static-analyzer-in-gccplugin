#include <stdio.h>
#include <stdlib.h>

static __attribute__((noinline)) void output(const char *pointer) {
  if (pointer != NULL)
    puts(pointer);
}

static __attribute__((noinline)) char *create(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    exit(1);
  __builtin_strcpy(pointer, "output");
  puts(pointer);
  return pointer;
}

static __attribute__((noinline)) char *create_with_output_wrapper(void) {
  char *pointer = calloc(32, 1);
  if (pointer == NULL)
    exit(1);
  __builtin_strcpy(pointer, "output");
  output(pointer);
  return pointer;
}

void returned_allocation_leaks(void) {
  char *pointer = create();
  (void)pointer;
}

void returned_wrapped_allocation_leaks(void) {
  char *pointer = create_with_output_wrapper();
  (void)pointer;
}

void returned_allocation_is_released(void) {
  free(create());
  free(create_with_output_wrapper());
}

/* Purity rules out capture/free, but is not a normal-return certificate. */
extern __attribute__((pure)) int hidden_control(const char *pointer);
static volatile int result;

static __attribute__((noinline)) char *create_with_unknown_continuation(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    exit(1);
  result = hidden_control(pointer);
  return pointer;
}

void unknown_continuation_cannot_invent_second_free(void) {
  char *pointer = create_with_unknown_continuation();
  free(pointer);
  free(pointer);
}
