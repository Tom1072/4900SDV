#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "../includes/actuators.h"
#include "../includes/commons.h"

/**
 * Handler of the ManualDriver thread.
*/
void *ManualDriver() {
  name_attach_t *attach;
  struct _pulse pulse_msg;
  ActuatorInputPayload *payload;

  if ((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
    return (void *)EXIT_FAILURE;

  while(1) {
    // rcvid =
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    switch(pulse_msg.code) {
      case _PULSE_CODE_DISCONNECT: // check the pulse code to see if the client is gone/disconnected
        ConnectDetach(pulse_msg.scoid);
        break;
      default:
        payload = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;
        printf("MANUAL DRIVER GOT pulse code:\n");
        printf("  Brake level: %d\n", payload->brake_level);
        printf("  Gas level: %d\n", payload->gas_level);
        free(pulse_msg.value.sival_ptr);
    }
  }

  return NULL;
}

/**
 * Handler of the ACC thread.
*/
void *ACC() {
  // int rcvid;
  name_attach_t *attach;
  struct _pulse pulse_msg;
  ActuatorInputPayload *payload;

  if ((attach = name_attach(NULL, ACC_NAME, 0)) == NULL) 
    return (void *)EXIT_FAILURE;

  while(1) {
    // rcvid =
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    switch(pulse_msg.code) {
      case _PULSE_CODE_DISCONNECT: // check the pulse code to see if the client is gone/disconnected
        ConnectDetach(pulse_msg.scoid);
        break;
      default:
        payload = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;
        printf("ACC GOT pulse code:\n");
        printf("  Distance: %d\n", payload->distance);
        printf("  Desired speed: %d\n", payload->desired_speed);
        printf("  Current speed: %d\n", payload->current_speed);
        free(pulse_msg.value.sival_ptr);
    }
  }

  return NULL;
}

/**
 * Handler of the ABS thread.
 */
void *ABS() {
  // int rcvid;
  name_attach_t *attach;
  struct _pulse pulse_msg;
  ActuatorInputPayload *payload;

  if ((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
    return (void *)EXIT_FAILURE;

  while(1) {
    // rcvid =
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    switch(pulse_msg.code) {
      case _PULSE_CODE_DISCONNECT: // check the pulse code to see if the client is gone/disconnected
        ConnectDetach(pulse_msg.scoid);
        break;
      default:
        payload = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;
        printf("ACC GOT pulse code:\n");
        printf("  Brake level: %d\n", payload->brake_level);
        printf("  Is skidding: %d\n", payload->skidding);
        free(pulse_msg.value.sival_ptr);
    }
  }

  return NULL;
}
