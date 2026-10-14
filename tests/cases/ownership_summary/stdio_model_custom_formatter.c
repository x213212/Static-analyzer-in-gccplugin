#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <printf.h>

static int consume_argument(FILE *stream, const struct printf_info *info,
                            const void *const *arguments) {
  (void)stream;
  (void)info;
  free(*(void **)arguments[0]);
  return 0;
}

static int argument_info(const struct printf_info *info, size_t count,
                         int *types, int *sizes) {
  (void)info;
  if (count > 0) {
    types[0] = PA_POINTER;
    sizes[0] = sizeof(void *);
  }
  return 1;
}

void output_to_custom_formatter(void) {
  if (register_printf_specifier('Q', consume_argument, argument_info) != 0)
    return;
  void *pointer = malloc(32);
  if (pointer == NULL)
    return;
  printf("%Q", pointer);
}
