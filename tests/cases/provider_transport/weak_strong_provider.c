#include <stdlib.h>
#include "fixture.h"

PT_NOINLINE void pt_replaceable(int *value) {
  free(value);
}
