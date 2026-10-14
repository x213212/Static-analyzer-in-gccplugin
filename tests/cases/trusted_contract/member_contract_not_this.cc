#include <cstdlib>

struct Reader {
  void read(const int *value)
      __attribute__((static_analyzer_must_access(1)));
};

void member_contract_not_this(void) {
  Reader *allocation = static_cast<Reader *>(std::malloc(sizeof(Reader)));
  if (allocation == nullptr)
    return;
  int live_value = 7;
  std::free(allocation);
  // The call uses a stale this, but its declaration only promises access to
  // live_value.  That promise cannot prove a memory access through this.
  allocation->read(&live_value);
}
