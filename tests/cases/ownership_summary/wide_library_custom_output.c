#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

void custom_stdout_keeps_output_unknown(FILE *replacement) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  wcscpy(pointer, L"output");
  stdout = replacement;
  wprintf(L"%ls!", pointer);
}
