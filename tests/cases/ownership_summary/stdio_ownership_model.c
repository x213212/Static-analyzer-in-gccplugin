#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static __attribute__((noinline)) void output(const char *pointer) {
  if (pointer != NULL)
    puts(pointer);
}

void puts_leak(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  strcpy(pointer, "output");
  puts(pointer);
}

void wrapper_leak(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  strcpy(pointer, "output");
  output(pointer);
}

void printf_leak(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  strcpy(pointer, "output");
  printf("%s!", pointer);
}

void output_then_release(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  strcpy(pointer, "output");
  output(pointer);
  free(pointer);
}
