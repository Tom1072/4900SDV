void create_thread(pthread_t *thread_ptr, pthread_attr_t *attr_ptr, int priority, void *(*handler)());
void send_pulse_msg(char *attach_name, char *msg_buffer, int code);