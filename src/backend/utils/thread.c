#include <pthread.h>
#include <string.h>

#include "../includes/utils.h"

/**
 * Implementation of all utility functions
 * 
 */

/** 
 * @brief Create a thread with the given priority
 * 
 * @param thread_ptr Pointer to the thread
 * @param attr_ptr Pointer to the thread attributes
 * @param priority Priority of the thread
 * @param args Arguments to the thread
 * @param handler Function to be executed by the thread
*/
void create_thread(pthread_t *thread_ptr, pthread_attr_t *attr_ptr, int priority, void *args, void *(*handler)())
{
  struct sched_param param;

  memset(&param, 0, sizeof(param));

  param.sched_priority = priority;
  pthread_attr_init(attr_ptr);
  pthread_attr_setschedparam(attr_ptr, &param);
  pthread_attr_setinheritsched(attr_ptr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(attr_ptr, SCHED_RR);
  pthread_create(thread_ptr, attr_ptr, (*handler), args);
}