/* Dangling-return analysis is language independent: C++ delete is the
   release event and GCC's SSA/PHI edge locations still identify the source
   return. */
__attribute__((noinline)) static int *return_deleted_pointer() {
  int *pointer = new int(11);
  delete pointer;
  return pointer;
}

/* A representation-preserving pointer cast must not hide the returned
   allocation identity. */
__attribute__((noinline)) static unsigned char *return_deleted_cast() {
  int *pointer = new int(12);
  delete pointer;
  return reinterpret_cast<unsigned char *>(pointer);
}

/* The only released branch returns null; the allocation-derived return is
   live and therefore must not be diagnosed. */
__attribute__((noinline)) static int *disjoint_delete_and_return(bool release) {
  int *pointer = new int(13);
  if (release) {
    delete pointer;
    return nullptr;
  }
  return pointer;
}

void exercise_returned_dangling_cpp() {
  int *first = return_deleted_pointer();
  unsigned char *second = return_deleted_cast();
  int *third = disjoint_delete_and_return(true);
  (void)first;
  (void)second;
  (void)third;
}
