typedef __SIZE_TYPE__ size_t;

// Spelling a project-local function "free" does not give it the semantics of
// GCC's BUILT_IN_FREE.  The explicit malloc attribute is therefore not a
// supported heap-ownership contract in proof-only mode.
static __attribute__((noinline)) void free(void *value) { (void)value; }

#if __GNUC__ >= 11
extern void *project_allocate(size_t)
    __attribute__((malloc(free, 1)));
#else
// GCC 10 only accepts the zero-argument form.  It still exercises the same
// proof-only fallback (an attributed, non-builtin allocator whose release
// contract the plugin cannot prove), while GCC 11+ additionally verifies that
// a project-local function merely named "free" is not promoted to BUILT_IN_FREE.
extern void *project_allocate(size_t) __attribute__((malloc));
#endif

void user_named_free_is_not_a_builtin_contract(void) {
  void *value = project_allocate(8);
  (void)value;
}
