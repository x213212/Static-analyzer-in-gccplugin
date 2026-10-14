void scalar_new_leak() {
  int *pointer = new int(7);
  *pointer += 1;
}
