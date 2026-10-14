#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern void *pure_identity(void *) __attribute__((pure));
extern uintptr_t identity_bits(void *) __attribute__((const));
extern void *borrow_identity(void *)
    __attribute__((static_analyzer_borrow(1)));

// Every external function below can honestly implement identity while
// satisfying its attributes.  None establishes a leak at the caller.
void *pure_return_escape(void) {
  void *p = malloc(8);
  if (!p) return NULL;
  return pure_identity(p);
}

void pure_return_free(void) {
  void *p = malloc(8);
  if (!p) return;
  free(pure_identity(p));
}

void integer_return_free(void) {
  void *p = malloc(8);
  if (!p) return;
  free((void *)identity_bits(p));
}

void borrow_return_free(void) {
  void *p = malloc(8);
  if (!p) return;
  free(borrow_identity(p));
}

void *memcpy_return_escape(const char *source) {
  void *p = malloc(8);
  if (!p) return NULL;
  return memcpy(p, source, 8);
}

void memchr_return_free(void) {
  char *p = malloc(8);
  if (!p) return;
  p[0] = 'x';
  free(memchr(p, 'x', 8));
}

// Optimized GIMPLE can carry the pointer payload in a pointer-sized integer.
void memory_payload_escape(void **out) {
  void *p = malloc(8);
  if (!p) return;
  memcpy(out, &p, sizeof p);
}
