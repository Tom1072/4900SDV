#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "../includes/utils.h"

void send_pulse_msg(char *attach_name, char *msg_buffer, int code) {
  int coid = name_open(attach_name, 0);
  // char *msg = malloc(sizeof(msg_buffer));
  // strcpy(msg, msg_buffer);
  // char *msg = msg_buffer;
  char msg[10];
  memset(msg, 0, sizeof(msg));
  // printf("Client msg ptr: %p\n", msg);
  // printf("Client msg ptr: %s\n", msg));
  // MsgSendPulsePtr(coid, -1, code, (void *)msg);
  // MsgSendPulse(coid, -1, code, 2);
}