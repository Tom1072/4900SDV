#include <pthread.h>
#include "../includes/utils.h"

void create_thread(pthread_t *thread_ptr, pthread_attr_t *attr_ptr, int priority, void *(*handler)()) {
  struct sched_param param;

  param.sched_priority = priority;
  pthread_attr_init(attr_ptr);
  pthread_attr_setschedparam(attr_ptr, &param);
  pthread_attr_setinheritsched(attr_ptr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy (attr_ptr, SCHED_RR);
  pthread_create(thread_ptr, attr_ptr, (*handler), NULL);
}