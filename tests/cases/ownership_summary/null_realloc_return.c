#include <stdlib.h>

static __attribute__((noinline)) char *create_by_realloc(char *prior) {
  char *pointer = realloc(prior, 32);
  if (pointer == NULL)
    exit(1);
  __builtin_strcpy(pointer, "output");
  return pointer;
}

void null_input_returns_owned_storage(void) {
  char *pointer = create_by_realloc(NULL);
  (void)pointer;
}

void null_input_then_release(void) {
  free(create_by_realloc(NULL));
}

/* An arbitrary old allocation is not a new allocation at this call site. */
void nonnull_or_unknown_input_does_not_invent_new_object(char *prior) {
  char *pointer = create_by_realloc(prior);
  (void)pointer;
}
