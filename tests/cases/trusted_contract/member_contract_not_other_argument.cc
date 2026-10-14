#include <cstdlib>

struct Reader {
  void read(const int *unobserved, const int *observed)
      __attribute__((static_analyzer_must_access(2)));
};

void member_contract_not_other_argument(Reader *reader) {
  int *allocation = static_cast<int *>(std::malloc(sizeof(int)));
  if (allocation == nullptr)
    return;
  int live_value = 7;
  std::free(allocation);
  // Neither the receiver nor the first explicit pointer gets the second
  // explicit parameter's must-access promise.
  reader->read(allocation, &live_value);
}
