#pragma once

/**
 * Store all utility functions
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
void create_thread(pthread_t *thread_ptr, pthread_attr_t *attr_ptr, int priority, void *args, void *(*handler)());
