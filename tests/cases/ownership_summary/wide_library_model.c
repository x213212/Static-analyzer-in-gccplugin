#include <stdlib.h>
#include <wchar.h>

/* Exercise the direct ABI symbol even on distributions whose headers insert
   a fortify wrapper with an object-size branch at -O1. */
extern wchar_t *abi_wmemset(wchar_t *, wchar_t, size_t) __asm__("wmemset");

static __attribute__((noinline)) void output(const wchar_t *pointer) {
  if (pointer != NULL)
    wprintf(L"%ls!", pointer);
}

void wide_copy_leak(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  wcscpy(pointer, L"output");
  output(pointer);
}

void wide_set_leak(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  abi_wmemset(pointer, L'A', 31);
  pointer[31] = 0;
  wprintf(L"%ls!", pointer);
}

void wide_copy_return_is_same_allocation(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  wchar_t *alias = wcscpy(pointer, L"output");
  free(alias);
  free(pointer);
}

void wide_copy_then_release(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  wchar_t *alias = wcscpy(pointer, L"output");
  output(alias);
  free(alias);
}
