#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Five SA003 positives from library entry preconditions.  The compiler's
   exact builtin identity identifies the string/format argument.  Neither
   these calls nor the ordinary source-defined wrappers promise to retain
   ownership or to perform a physical read before every I/O failure. */
static volatile int stdio_live_result;

static __attribute__((noinline)) void print_if_nonnull(const char *value) {
  if (value != NULL)
    puts(value);
}

static __attribute__((noinline)) void print_nested(const char *value) {
  print_if_nonnull(value);
}

void stdio_live_direct_puts_uaf(void) {
  char *value = (char *)malloc(16);
  if (value == NULL)
    return;
  value[0] = '\0';
  free(value);
  puts(value);
}

void stdio_live_nonnull_wrapper_uaf(void) {
  char *value = (char *)malloc(16);
  if (value == NULL)
    return;
  value[0] = '\0';
  free(value);
  print_if_nonnull(value);
}

void stdio_live_nested_wrapper_uaf(void) {
  char *value = (char *)malloc(16);
  if (value == NULL)
    return;
  value[0] = '\0';
  free(value);
  print_nested(value);
}

void stdio_live_printf_format_uaf(void) {
  char *format = (char *)malloc(16);
  if (format == NULL)
    return;
  strcpy(format, "value: %d\n");
  free(format);
  stdio_live_result = printf(format, 23);
}

void stdio_live_fprintf_format_uaf(void) {
  char *format = (char *)malloc(16);
  if (format == NULL)
    return;
  strcpy(format, "value: %d\n");
  free(format);
  stdio_live_result = fprintf(stderr, format, 23);
}

/* Negative for the UAF classification, NOT an assertion that puts(NULL)
   is valid: no live heap allocation existed on the malloc-failure path. */
void stdio_live_free_null_is_not_uaf(void) {
  char *value = (char *)malloc(16);
  if (value == NULL) {
    free(value);
    puts(value);
    return;
  }
  free(value);
}

/* Only printf's format argument gets a valid-string requirement.  A %p
   argument must not be classified as a string access.  This is a negative
   test of that theorem, not a general endorsement of evaluating stale
   pointer values under every C implementation. */
void stdio_live_pointer_format_is_not_string_access(void) {
  char *value = (char *)malloc(16);
  if (value == NULL)
    return;
  free(value);
  stdio_live_result = printf("pointer: %p\n", (void *)value);
}
