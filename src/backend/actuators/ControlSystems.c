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
  printf("I'm Manual Driver\n");
  // actuatorChanges_t data;
  name_attach_t *attach;
  struct _pulse pulse_msg;

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
        printf("MANUAL DRIVER GOT pulse code: %d; value: %d\n", pulse_msg.code, pulse_msg.value.sival_int);
    }
  }

  return NULL;
}

/**
 * Handler of the ACC thread.
*/
void *ACC() {
  printf("I'm ACC\n");
  // int rcvid;
  // actuatorChanges_t data;
  name_attach_t *attach;
  struct _pulse pulse_msg;

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
        printf("ACC GOT pulse code: %d; value: %d\n", pulse_msg.code, pulse_msg.value.sival_int);
    }
  }

  return NULL;
}

/**
 * Handler of the ABS thread.
 */
void *ABS() {
  printf("I'm ABS\n");
  // int rcvid;
  // actuatorChanges_t data;
  name_attach_t *attach;
  struct _pulse pulse_msg;

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
        printf("ABS GOT pulse code: %d; value: %d\n", pulse_msg.code, pulse_msg.value.sival_int);
    }
  }

  return NULL;
}
