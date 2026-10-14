#include <stdlib.h>

/* These are ordinary source-defined functions, without analyzer contracts.
   Only a body-bound call whose access is guaranteed may establish SA003.
   noinline and the volatile output preserve the interprocedural use. */
static volatile int observed;
extern volatile int body_access_loop_gate;
extern void body_access_unknown_operation(void);

struct body_access_record {
  int first;
  int second;
};

static __attribute__((noinline)) void read_direct(const int *value) {
  observed = *value;
}

static __attribute__((noinline)) void read_nonnull(const int *value) {
  if (value != NULL)
    observed = *value;
}

static __attribute__((noinline)) void read_nested(const int *value) {
  read_nonnull(value);
}

static __attribute__((noinline)) void
read_field(const struct body_access_record *value) {
  observed = value->second;
}

static __attribute__((noinline)) void
read_every_branch(const int *value, int choice) {
  if (choice)
    observed = *value;
  else
    observed = *value + 1;
}

static __attribute__((noinline)) void
read_before_unknown_operation(const int *value) {
  observed = *value;
  body_access_unknown_operation();
}

static __attribute__((noinline)) void
read_other_argument(const int *ignored, const int *value) {
  (void)ignored;
  observed = *value;
}

static __attribute__((noinline)) void
read_under_scalar_condition(const int *value, int read_it) {
  if (read_it)
    observed = *value;
}

static __attribute__((noinline)) void
read_after_early_return(const int *value, int skip_it) {
  if (skip_it)
    return;
  observed = *value;
}

static __attribute__((noinline)) void
read_after_unknown_operation(const int *value) {
  /* The opaque operation may never return; a normal-return summary is not
     sufficient to prove that entering this wrapper reaches the load. */
  body_access_unknown_operation();
  observed = *value;
}

static __attribute__((noinline)) void read_after_loop(const int *value) {
  /* The loop may diverge without reading value. */
  while (body_access_loop_gate) {
  }
  observed = *value;
}

__attribute__((weak, noinline)) void body_access_replaceable_read(
    const int *value) {
  observed = *value;
}

/* Six positive callers: each has one real, source-derived UAF witness. */
void body_access_direct_uaf(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_direct(value);
}

void body_access_nonnull_uaf(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_nonnull(value);
}

void body_access_nested_uaf(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_nested(value);
}

void body_access_field_uaf(void) {
  struct body_access_record *value =
      (struct body_access_record *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_field(value);
}

void body_access_all_branches_uaf(int choice) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_every_branch(value, choice);
}

void body_access_before_unknown_uaf(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_before_unknown_operation(value);
}

/* Definite negatives: the dangling argument is not the accessed argument,
   and free(NULL) followed by an if-nonnull reader does not access storage. */
void body_access_other_argument_is_safe(void) {
  int live = 17;
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_other_argument(value, &live);
}

void body_access_null_path_is_safe(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL) {
    free(value);
    read_nonnull(value);
    return;
  }
  free(value);
}

/* These five are negative tests of a MUST-access theorem, not assertions
   that the programs are universally safe. Without additional evidence the
   proof-only policy must leave them inconclusive, not invent a witness. */
void body_access_scalar_condition_is_unproven(int read_it) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_under_scalar_condition(value, read_it);
}

void body_access_early_return_is_unproven(int skip_it) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_after_early_return(value, skip_it);
}

void body_access_unknown_prefix_is_unproven(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_after_unknown_operation(value);
}

void body_access_loop_prefix_is_unproven(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  read_after_loop(value);
}

void body_access_weak_provider_is_unproven(void) {
  int *value = (int *)malloc(sizeof(*value));
  if (value == NULL)
    return;
  free(value);
  body_access_replaceable_read(value);
}
