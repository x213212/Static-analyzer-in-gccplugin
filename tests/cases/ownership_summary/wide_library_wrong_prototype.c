#include <stdlib.h>

extern void misleading_copy(char *, const char *) __asm__("wcscpy");

void wrong_prototype_remains_unknown(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  misleading_copy(pointer, "output");
}
