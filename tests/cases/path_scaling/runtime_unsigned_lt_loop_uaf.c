#include <stddef.h>
#include <stdlib.h>

static volatile int observed;

/* The bound is not a constant.  Nevertheless, for every size_t value,
   `index = 0; index < count; ++index` reaches its exit without wrapping.
   The assignment-only body is event-neutral for the later free/use witness. */
void runtime_unsigned_lt_loop_uaf(size_t count) {
  int *pointer = (int *)malloc(count * sizeof(*pointer));
  for (size_t index = 0; index < count; ++index)
    pointer[index] = 5;
  free(pointer);
  observed = pointer[0];
}

/* Operand order is syntax, not semantics.  The theorem normalizes
   `bound > induction` to the same unsigned-LT recurrence. */
void runtime_reversed_operands_uaf(size_t count) {
  int *pointer = (int *)malloc(count * sizeof(*pointer));
  for (size_t index = 0; count > index; ++index)
    pointer[index] = 6;
  free(pointer);
  observed = pointer[0];
}

/* This source form normally lowers with the loop body on the false CFG edge.
   The recognizer must negate the edge predicate rather than assuming that the
   true successor is always the latch path. */
void runtime_false_edge_body_uaf(size_t count) {
  int *pointer = (int *)malloc(count * sizeof(*pointer));
  size_t index = 0;
  for (;;) {
    if (index >= count)
      break;
    pointer[index] = 7;
    ++index;
  }
  free(pointer);
  observed = pointer[0];
}

/* The universal proof is valid for every constant initial value, not merely
   the common zero-based spelling. */
void runtime_nonzero_initial_uaf(size_t count) {
  int *pointer = (int *)malloc(count * sizeof(*pointer));
  for (size_t index = 7; index < count; ++index)
    pointer[index] = 8;
  free(pointer);
  observed = pointer[0];
}

/* A step of two is not covered: it can jump over the bound and wrap. */
void runtime_step_two_remains_unknown(size_t count) {
  int *pointer = (int *)malloc(count * sizeof(*pointer));
  for (size_t index = 0; index < count; index += 2)
    pointer[index] = 5;
  free(pointer);
  observed = pointer[0];
}

/* <= is not universally finite because count may be SIZE_MAX. */
void runtime_le_remains_unknown(size_t count) {
  int *pointer = (int *)malloc((count + 1) * sizeof(*pointer));
  for (size_t index = 0; index <= count; ++index)
    pointer[index] = 5;
  free(pointer);
  observed = pointer[0];
}

/* A second loop-carried PHI is not a universally fixed bound.  This loop is
   harmlessly finite, but it intentionally falls outside the narrow theorem
   and must remain UNKNOWN rather than being generalized by pattern alone. */
void runtime_loop_carried_bound_remains_unknown(size_t count) {
  int *pointer = (int *)malloc(count * sizeof(*pointer));
  size_t bound = count;
  for (size_t index = 0; index < bound; ++index) {
    pointer[index] = 9;
    --bound;
  }
  free(pointer);
  observed = pointer[0];
}

/* Signed induction is deliberately outside the modular unsigned theorem. */
void runtime_signed_bound_remains_unknown(long count) {
  int *pointer = (int *)malloc((size_t)count * sizeof(*pointer));
  for (long index = 0; index < count; ++index)
    pointer[index] = 10;
  free(pointer);
  observed = pointer[0];
}
