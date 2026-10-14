#include <pthread.h>

static void *worker(void *argument) { return argument; }

void conditional_join(int do_join) {
  pthread_t thread;
  pthread_create(&thread, 0, worker, 0);
  if (do_join)
    pthread_join(thread, 0);
}
