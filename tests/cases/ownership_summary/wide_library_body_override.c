#include <stdlib.h>
#include <wchar.h>

__attribute__((noinline)) wchar_t *wcscpy(wchar_t *destination,
                                        const wchar_t *source) {
  (void)source;
  free(destination);
  return destination;
}

void override_releases_storage(void) {
  wchar_t *pointer = malloc(32 * sizeof(*pointer));
  if (pointer == NULL)
    return;
  wcscpy(pointer, L"output");
  free(pointer);
}
