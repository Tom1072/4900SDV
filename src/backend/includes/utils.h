#include <stdio.h>
#include "../includes/commons.h"
void create_thread(pthread_t *thread_ptr, pthread_attr_t *attr_ptr, int priority, void *args, void *(*handler)());
void send_pulse_msg(char *attach_name, char *msg_buffer, int code);
void write_data(FILE *fn, Environment *env);
