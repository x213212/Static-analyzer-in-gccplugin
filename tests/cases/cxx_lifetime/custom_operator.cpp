#include <cstddef>

struct CustomAllocated {
  int value;
  static void *operator new(std::size_t) __attribute__((malloc));
  static void operator delete(void *) noexcept;
};

void custom_operator_new_leak() {
  CustomAllocated *pointer = new CustomAllocated;
  pointer->value = 1;
}

void custom_operator_new_released() {
  CustomAllocated *pointer = new CustomAllocated;
  delete pointer;
}
