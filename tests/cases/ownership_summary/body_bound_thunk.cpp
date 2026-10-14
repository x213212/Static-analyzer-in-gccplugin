#include <cstdlib>

/* A real secondary-base adjustment: Secondary::allocation receives the
   secondary-subobject address, whereas Object::allocation returns the
   complete-object address.  Flattening its thunk to the underlying body
   must not turn that return into an exact copy of the incoming this value.
   This fixture contains no analyzer contracts or hand-written ABI symbols. */
namespace {
struct Primary {
  virtual void anchor() {}
  unsigned long padding;
};

struct Secondary {
  virtual void *allocation() = 0;
};

struct Object final : Primary, Secondary {
  __attribute__((noinline)) void *allocation() override { return this; }
};

__attribute__((noinline)) void *through_secondary(Secondary *value) {
  return value->allocation();
}
}  // namespace

void body_bound_thunk_returns_complete_object() {
  Object *object = new Object;
  Secondary *secondary = object;
  void *returned = through_secondary(secondary);
  if (returned != static_cast<void *>(object) ||
      returned == static_cast<void *>(secondary))
    std::abort();
  delete static_cast<Object *>(returned);
}
