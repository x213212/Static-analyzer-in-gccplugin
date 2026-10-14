#include "fixture.h"

PT_NOINLINE void pt_forward(int *value, void (*consume)(int *)) {
  consume(value);
}
