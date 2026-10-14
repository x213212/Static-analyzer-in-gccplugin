#include <stddef.h>

typedef struct AVFilterContext { void *priv; } AVFilterContext;
typedef struct AVFilterLink { AVFilterContext *dst; int h; } AVFilterLink;
typedef struct AVFilterPad { int (*config_props)(AVFilterLink *); } AVFilterPad;
typedef struct AVFilter {
  size_t priv_size;
  void (*uninit)(AVFilterContext *);
  const AVFilterPad *inputs;
  int (*init)(AVFilterContext *);
} AVFilter;
typedef struct UserState { int *retained, *temporary; } UserState;
extern void *av_calloc(size_t, size_t);
extern void av_freep(void *);
extern void unknown_cleanup(void *);

static __attribute__((noinline)) int prepare(AVFilterLink *link) {
  UserState *state = link->dst->priv;
  state->retained = av_calloc(4, sizeof(int));
  if (!state->retained) return -1;
  state->temporary = av_calloc(3, sizeof(int));
  if (!state->temporary) return -1;
  return 0;
}

static __attribute__((noinline)) void destroy(AVFilterContext *ctx) {
  UserState *state = ctx->priv;
#ifdef OPAQUE_CLEANUP
  unknown_cleanup(state);
#else
  av_freep(&state->temporary);
#ifdef REPAIR_RETAINED
  av_freep(&state->retained);
#endif
#endif
}

static const AVFilterPad inputs[] = {{ .config_props = prepare }};
const AVFilter unrelated_name = {
  .priv_size = sizeof(UserState), .inputs = inputs, .uninit = destroy,
};
