#include <stdlib.h>

struct holder {
  void *value;
};

extern void take_holder(struct holder *);

void opaque_aggregate_transfer_stays_inconclusive(void) {
  void *value = malloc(8);
  struct holder local = {value};
  take_holder(&local);
}
