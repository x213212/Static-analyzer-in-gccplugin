#include <pthread.h>

static void *worker(void *argument) { return argument; }

void two_handles(void) {
  pthread_t first;
  pthread_t second;
  pthread_create(&first, 0, worker, 0);
  pthread_create(&second, 0, worker, 0);
  pthread_join(first, 0);
}
