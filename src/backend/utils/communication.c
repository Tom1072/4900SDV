#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "../includes/utils.h"
#include "../includes/actuators.h"

void send_pulse_msg2(char *attach_name, ActuatorInputPayload *message, int code) {
  int coid = name_open(attach_name, 0);
  // char *msg = malloc(sizeof(msg_buffer));
  // strcpy(msg, msg_buffer);
  // char *msg = msg_buffer;
  // char msg[10];
  // memset(msg, 0, sizeof(msg));
  MsgSendPulsePtr(coid, -1, code, (void *)message);
  // MsgSendPulse(coid, -1, code, 2);
}