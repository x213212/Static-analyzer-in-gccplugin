#include <new>

struct PlacedValue {
  int value;
  PlacedValue() : value(3) {}
  ~PlacedValue() { value = 0; }
};

void placement_new_uses_existing_storage(void *storage) {
  PlacedValue *pointer = new (storage) PlacedValue;
  pointer->~PlacedValue();
}
