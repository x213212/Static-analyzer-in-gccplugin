#include <stdlib.h>

/* INTENTIONALLY incompatible with mismatch_provider.c.  This is a rejected
   metadata-binding test, NOT a defined program or an accuracy/TN sample. */
void pt_mismatch(const volatile int *value);

int main(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return 0;
  *value = 31;
  pt_mismatch(value);
  return 0;
}
