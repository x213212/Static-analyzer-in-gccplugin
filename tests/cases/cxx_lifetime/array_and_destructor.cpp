struct TrackedValue {
  int value;
  TrackedValue() : value(1) {}
  ~TrackedValue() { value = 0; }
};

void array_new_released() {
  TrackedValue *pointer = new TrackedValue[3];
  delete[] pointer;
}

void class_new_released() {
  TrackedValue *pointer = new TrackedValue;
  delete pointer;
}
