#include <stdlib.h>

/* GNU C permits zero-size aggregate types. A GIMPLE memory-reference operand
   is not by itself evidence of a nonzero load/store. The body-access theorem
   must ask GCC for the referenced extent before publishing must-access.
   GCC 16 currently removes these empty copies before GIMPLE: this fixture
   checks the end-to-end result, not coverage of the zero-extent guard. */
__extension__ struct body_empty_record {
};

struct body_record_with_empty_field {
  struct body_empty_record empty;
  int payload;
};

_Static_assert(sizeof(struct body_empty_record) == 0,
               "this regression requires GNU C zero-size aggregates");

static volatile struct body_empty_record empty_observed;
static const int *volatile address_observed;

static __attribute__((noinline)) void
copy_empty_record(const struct body_empty_record *value) {
  empty_observed = *value;
}

static __attribute__((noinline)) void copy_empty_field(
    const struct body_record_with_empty_field *value) {
  empty_observed = value->empty;
}

static __attribute__((noinline)) void observe_field_address(
    const struct body_record_with_empty_field *value) {
  address_observed = &value->payload;
}

/* These test absence of an actual memory-access witness, not portability of
   evaluating dangling pointer values under every interpretation of C.
   No helper reads or writes a byte of the released allocation. */
void body_access_zero_size_copy_has_no_access(void) {
  struct body_empty_record *value =
      (struct body_empty_record *)malloc(8);
  if (value == NULL)
    return;
  free(value);
  copy_empty_record(value);
}

void body_access_zero_size_field_has_no_access(void) {
  struct body_record_with_empty_field *value =
      (struct body_record_with_empty_field *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  copy_empty_field(value);
}

void body_access_field_address_has_no_access(void) {
  struct body_record_with_empty_field *value =
      (struct body_record_with_empty_field *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  observe_field_address(value);
}
