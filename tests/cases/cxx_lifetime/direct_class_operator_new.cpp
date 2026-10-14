#include <cstddef>

struct ArenaAllocated {
  int value;
  static void *operator new(std::size_t);
};

void direct_class_operator_new_is_not_freshness_proof() {
  void *raw = ArenaAllocated::operator new(sizeof(ArenaAllocated));
  static_cast<ArenaAllocated *>(raw)->value = 1;
}
