#include <pthread.h>

static void *worker(void *argument) { return argument; }

void joinable_not_joined(void) {
  pthread_t thread;
  pthread_create(&thread, 0, worker, 0);
}
