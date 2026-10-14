void scalar_double_delete() {
  int *pointer = new int(7);
  delete pointer;
  delete pointer;
}
