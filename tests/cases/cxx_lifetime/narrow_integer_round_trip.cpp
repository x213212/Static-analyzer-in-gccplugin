#include <cstdint>
#include <cstdlib>

void narrow_integer_round_trip_is_not_alias_proof() {
  void *allocated = std::malloc(32);
  const std::uintptr_t wide = reinterpret_cast<std::uintptr_t>(allocated);
  const unsigned int narrow = static_cast<unsigned int>(wide);
  void *reconstructed =
      reinterpret_cast<void *>(static_cast<std::uintptr_t>(narrow));

  // This may be an invalid pointer at runtime, but truncation means it is not
  // proof that the original allocation was released.  In particular, it must
  // not create a false double-free relation with the release below.
  std::free(reconstructed);
  std::free(allocated);
}
