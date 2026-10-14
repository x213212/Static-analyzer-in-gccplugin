/* Test the source-defined alias itself, rather than a fortified header's
   always-inline wrapper. Other fixtures cover the fortified library ABI. */
#ifdef _FORTIFY_SOURCE
#undef _FORTIFY_SOURCE
#endif
#include <stdlib.h>
#include <wchar.h>

__attribute__((noinline)) wchar_t *private_copy(wchar_t *pointer,
                                               const wchar_t *source) {
  (void)source;
  free(pointer);
  return pointer;
}

extern wchar_t *wcscpy(wchar_t *, const wchar_t *)
    __attribute__((alias("private_copy")));

void alias_release_then_free(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  wcscpy(pointer, L"text");
  free(pointer);
}
