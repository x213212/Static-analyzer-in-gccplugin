#include <pthread.h>

static void *worker(void *argument) { return argument; }

void detached(void) {
  pthread_t thread;
  pthread_create(&thread, 0, worker, 0);
  pthread_detach(thread);
}
