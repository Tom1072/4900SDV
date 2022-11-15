#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "../includes/actuators.h"
#include "../includes/commons.h"

/**
 * current controller
 */
volatile int state = NOT_ACQUIRED;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/**
 * Handler of the ManualDriver thread.
 */
void *ManualDriver()
{
  name_attach_t *attach;
  struct _pulse pulse_msg;
  ActuatorInputPayload *payload;

  if ((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
    return (void *)EXIT_FAILURE;

  while (1)
  {
    // printf("MANUAL before lock\n");
    // printf("MANUAL after lock\n");

    // rcvid =
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    // printf("MANUAL RECEIVED\n");

    pthread_mutex_lock(&mutex);

    while (state != NOT_ACQUIRED && state != MANUAL_DRIVER_STATE)
    {
      pthread_cond_wait(&cond, &mutex);
    }

    switch (pulse_msg.code)
    {
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

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

/**
 * Handler of the ACC thread.
 */
void *ACC()
{
  // int rcvid;
  name_attach_t *attach;
  struct _pulse pulse_msg;
  ActuatorInputPayload *payload;

  if ((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
    return (void *)EXIT_FAILURE;

  while (1)
  {
    // rcvid =
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    pthread_mutex_lock(&mutex);

    while (state != NOT_ACQUIRED && state != ACC_STATE)
    {
      pthread_cond_wait(&cond, &mutex);
    }

    switch (pulse_msg.code)
    {
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

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

/**
 * Handler of the ABS thread.
 */
void *ABS()
{
  // int rcvid;
  name_attach_t *attach;
  struct _pulse pulse_msg;
  ActuatorInputPayload *payload;

  if ((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
    return (void *)EXIT_FAILURE;

  while (1)
  {
    // rcvid =
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    pthread_mutex_lock(&mutex);

    while (state != NOT_ACQUIRED && state != ABS_STATE)
    {
      pthread_cond_wait(&cond, &mutex);
    }

    switch (pulse_msg.code)
    {
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

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}
