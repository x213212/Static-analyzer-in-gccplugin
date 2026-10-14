#include <stdlib.h>

/* A direct dangling return is a lifetime violation even if the caller only
   stores the result.  The diagnostic trace must identify allocation, release,
   and return in the callee. */
__attribute__((noinline)) static int *return_freed_pointer(void) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  free(pointer);
  return pointer;
}

/* One feasible normal path returns the exact released allocation. */
__attribute__((noinline)) static int *
conditionally_return_freed_pointer(int release) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  if (release)
    free(pointer);
  return pointer;
}

/* Returning live ownership is not a use after free. */
__attribute__((noinline)) static int *return_live_pointer(void) {
  return (int *)malloc(sizeof(int));
}

/* Releasing a different allocation does not invalidate the return value. */
__attribute__((noinline)) static int *release_other_pointer(void) {
  int *returned = (int *)malloc(sizeof(*returned));
  int *released = (int *)malloc(sizeof(*released));
  free(released);
  return returned;
}

/* The released arm returns NULL; the allocation is returned only while live. */
__attribute__((noinline)) static int *
disjoint_release_and_return(int release) {
  int *pointer = (int *)malloc(sizeof(*pointer));
  if (release) {
    free(pointer);
    return NULL;
  }
  return pointer;
}

void exercise_returned_dangling_pointer(void) {
  int *first = return_freed_pointer();
  int *second = conditionally_return_freed_pointer(1);
  int *third = return_live_pointer();
  int *fourth = release_other_pointer();
  int *fifth = disjoint_release_and_return(1);
  (void)first;
  (void)second;
  (void)third;
  (void)fourth;
  (void)fifth;
}
