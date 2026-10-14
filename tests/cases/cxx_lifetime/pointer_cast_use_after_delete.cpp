int pointer_cast_use_after_delete() {
  int *pointer = new int(9);
  unsigned char *bytes = reinterpret_cast<unsigned char *>(pointer);
  delete pointer;
  return bytes[0];
}
