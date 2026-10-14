#include <cstdlib>

struct Observer {
  void observe(int tag, const void *value)
      __attribute__((static_analyzer_borrow(2)));
};

void member_borrow_explicit_argument(Observer *observer) {
  void *allocation = std::malloc(32);
  if (allocation == nullptr)
    return;
  observer->observe(7, allocation);
  // SA001: the explicit pointer was only borrowed and remains unreleased.
}
