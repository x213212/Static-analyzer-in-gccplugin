#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void output_to_unknown_stream(FILE *replacement) {
  FILE *saved = stdout;
  stdout = replacement;
  char *pointer = malloc(32);
  if (pointer == NULL) {
    stdout = saved;
    return;
  }
  strcpy(pointer, "output");
  puts(pointer);
  stdout = saved;
}
