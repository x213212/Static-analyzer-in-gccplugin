void scalar_new_released() {
  int *pointer = new int(7);
  delete pointer;
}
