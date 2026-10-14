#include <cstdlib>
#include "fixture.h"

namespace provider_transport_fixture {
void borrow(const volatile int *value);
void read(const volatile int *value);

PT_NOINLINE void leak() {
  int *value = static_cast<int *>(std::malloc(sizeof(*value)));
  if (value == nullptr)
    return;
  *value = 7;
  borrow(value); /* SA001: the provider keeps ownership with this caller. */
}

PT_NOINLINE void uaf() {
  int *value = static_cast<int *>(std::malloc(sizeof(*value)));
  if (value == nullptr)
    return;
  *value = 9;
  std::free(value);
  read(value); /* SA003: source-derived access, through a mangled symbol. */
}
}

int main() {
  provider_transport_fixture::leak();
  provider_transport_fixture::uaf();
  return 0;
}
