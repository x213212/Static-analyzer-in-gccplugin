#include <stdlib.h>
#include <wchar.h>

void freed_wide_string_is_invalid(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  wcscpy(pointer, L"output");
  free(pointer);
  wprintf(L"%ls\n", pointer);
}

void freed_narrow_string_in_wide_output(void) {
  char *pointer = malloc(32);
  if (pointer == NULL)
    return;
  pointer[0] = 0;
  free(pointer);
  wprintf(L"%s\n", pointer);
}

void pointer_address_output(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  free(pointer);
  wprintf(L"%p\n", (void *)pointer);
}

void zero_precision_does_not_prove_access(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  free(pointer);
  wprintf(L"%.0ls\n", pointer);
}
