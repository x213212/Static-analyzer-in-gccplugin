#include "fixture.h"

/* Deliberately not extern "C": assembler identity includes C++ mangling. */
namespace provider_transport_fixture {
PT_NOINLINE void borrow(const volatile int *value) {
  if (value != nullptr)
    (void)*value;
}

PT_NOINLINE void read(const volatile int *value) {
  (void)*value;
}
}
