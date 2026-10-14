int scalar_use_after_delete() {
  int *pointer = new int(7);
  delete pointer;
  return *pointer;
}
