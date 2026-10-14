#include <alloca.h>
#include <stddef.h>

extern void inspect_bytes(const void *, size_t);

void builtin_alloca_has_automatic_lifetime(void) {
  unsigned char *buffer = (unsigned char *)__builtin_alloca(32);
  buffer[0] = 1;
  inspect_bytes(buffer, 32);
}

void libc_alloca_has_automatic_lifetime(void) {
  unsigned char *buffer = (unsigned char *)alloca(32);
  buffer[0] = 2;
  inspect_bytes(buffer, 32);
}

void aligned_alloca_has_automatic_lifetime(void) {
  unsigned char *buffer =
      (unsigned char *)__builtin_alloca_with_align(32, 64);
  buffer[0] = 3;
  inspect_bytes(buffer, 32);
}
