#include <cstdlib>

struct Reader {
  // Source index 2 is the pointer, after the explicitly written integer.
  // GCC lowers this to (this, tag, allocation), where its operand index is 2.
  void read(int tag, const int *value)
      __attribute__((static_analyzer_must_access(2)));
};

void member_explicit_argument(Reader *reader) {
  int *allocation = static_cast<int *>(std::malloc(sizeof(int)));
  if (allocation == nullptr)
    return;
  std::free(allocation);
  reader->read(7, allocation);  // SA003: explicit pointer argument is accessed.
}
