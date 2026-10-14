#include <stdlib.h>

extern void observe_pointer(void *);

#define FANOUT_0 observe_pointer(allocation);
#define FANOUT_1 FANOUT_0 FANOUT_0
#define FANOUT_2 FANOUT_1 FANOUT_1
#define FANOUT_3 FANOUT_2 FANOUT_2
#define FANOUT_4 FANOUT_3 FANOUT_3
#define FANOUT_5 FANOUT_4 FANOUT_4
#define FANOUT_6 FANOUT_5 FANOUT_5
#define FANOUT_7 FANOUT_6 FANOUT_6
#define FANOUT_8 FANOUT_7 FANOUT_7
#define FANOUT_9 FANOUT_8 FANOUT_8
#define FANOUT_10 FANOUT_9 FANOUT_9
#define FANOUT_11 FANOUT_10 FANOUT_10
#define FANOUT_12 FANOUT_11 FANOUT_11

void bounded_immediate_use_fanout(void) {
  void *allocation = malloc(16);
  FANOUT_12
  FANOUT_0
}
