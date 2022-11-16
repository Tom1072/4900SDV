#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "../includes/actuators.h"
#include "../includes/commons.h"
#include "../includes/utils.h"

/**
 * current controller
 */
volatile int state = NOT_ACQUIRED;
volatile int request = ABS_STATE;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

volatile int brake_level = 0;

void *man_processor();
void *acc_processor();
void *abs_processor();

char has_higher_prio(unsigned short curr_state);
void copy_man_input_payload(ManMessageInput *input, ManMessageInput *copied);
void copy_acc_input_payload(AccMessageInput *input, AccMessageInput *copied);
void copy_abs_input_payload(AbsMessageInput *input, AbsMessageInput *copied);

/**
 * Handler of the ManualDriver thread.
 */
void *ManualDriver()
{
  name_attach_t *attach;
  struct _pulse pulse_msg;
  ManMessageInput *input, *processed_input;
  // int sim_coid;
  pthread_t processor_thread;
  pthread_attr_t processor_attr;

  if ((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
    return (void *)EXIT_FAILURE;

  processed_input = malloc(sizeof(ManMessageInput));
  create_thread(&processor_thread, &processor_attr, MANUAL_PRIO, processed_input, man_processor);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      break;
    }

    input = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;

    pthread_mutex_lock(&mutex);

    if (has_higher_prio(MANUAL_DRIVER_STATE)) {
      state = MANUAL_DRIVER_STATE; // change to Manual state
    }
    brake_level = input->brake_level;
    copy_man_input_payload(input, processed_input);

    sleep(2);
    processed_input->brake_level = 10;

    pthread_mutex_unlock(&mutex);

    // printf("MANUAL DRIVER GOT pulse code:\n");
    // printf("  Brake level: %d\n", brake_level);
    free(input);

    // switch (pulse_msg.code)
    // {
    // case _PULSE_CODE_DISCONNECT: // check the pulse code to see if the client is gone/disconnected
    //   ConnectDetach(pulse_msg.scoid);
    //   break;
    // default:
    //   payload = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;
    //   printf("MANUAL DRIVER GOT pulse code:\n");
    //   printf("  Brake level: %d\n", payload->brake_level);
    //   printf("  Gas level: %d\n", payload->gas_level);
    //   free(pulse_msg.value.sival_ptr);
    // }
  }

  free(processed_input);
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

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      break;
    }

    payload = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;
    pthread_mutex_lock(&mutex);

    while (state != NOT_ACQUIRED && state != ACC_STATE)
    {
      pthread_cond_wait(&cond, &mutex);
    }

    // printf("ACC GOT pulse code:\n");
    // printf("  Distance: %d\n", payload->distance);
    // printf("  Desired speed: %d\n", payload->desired_speed);
    // printf("  Current speed: %d\n", payload->current_speed);
    free(pulse_msg.value.sival_ptr);

    // switch (pulse_msg.code)
    // {
    // case _PULSE_CODE_DISCONNECT: // check the pulse code to see if the client is gone/disconnected
    //   ConnectDetach(pulse_msg.scoid);
    //   break;
    // default:
    //   payload = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;
    //   printf("ACC GOT pulse code:\n");
    //   printf("  Distance: %d\n", payload->distance);
    //   printf("  Desired speed: %d\n", payload->desired_speed);
    //   printf("  Current speed: %d\n", payload->current_speed);
    //   free(pulse_msg.value.sival_ptr);
    // }

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
  AbsMessageInput *payload;
  char skidding = FALSE;
  pthread_t processor_thread;
  pthread_attr_t processor_attr;

  if ((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
    return (void *)EXIT_FAILURE;

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      break;
    }

    payload = (AbsMessageInput *)pulse_msg.value.sival_ptr;

    printf("Skidding: %d, payload skidding: %d\n", skidding, payload->skid);
    // TODO: Check situation here
    if (!skidding && payload->skid)
    {
      printf("ABS locked\n");
      pthread_mutex_lock(&mutex);
      state = ABS_STATE;

      skidding = TRUE;
      // brake_level = 0;
      create_thread(&processor_thread, &processor_attr, ABS_PRIO, payload, abs_processor);
    }
    else if (skidding && !payload->skid)
    {
      skidding = FALSE;
      state = NOT_ACQUIRED;

      // pthread_cancel(processor_thread);
      pthread_mutex_unlock(&mutex);
    }

    // printf("ABS GOT pulse code:\n");
    // printf("  Brake level: %d\n", brake_level);
    // printf("  Is skidding: %d\n", skidding);
    free(pulse_msg.value.sival_ptr);

    // switch (pulse_msg.code)
    // {
    // case _PULSE_CODE_DISCONNECT: // check the pulse code to see if the client is gone/disconnected
    //   ConnectDetach(pulse_msg.scoid);
    //   break;
    // default:
    //   payload = (ActuatorInputPayload *)pulse_msg.value.sival_ptr;
    //   printf("ACC GOT pulse code:\n");
    //   printf("  Brake level: %d\n", payload->brake_level);
    //   printf("  Is skidding: %d\n", payload->skidding);
    //   free(pulse_msg.value.sival_ptr);
    // }
  }

  return NULL;
}

void *man_processor(void *args)
{
  ActuatorInputPayload *data = args;
  while (1)
  {
    while (state != MANUAL_DRIVER_STATE)
    {
      pthread_cond_wait(&cond, &mutex);
    }

    printf("Sending speed based on brake level %d\n", data->brake_level);
    usleep(200 * 1000); 
  }
  return NULL;
}

void *acc_processor()
{
  return NULL;
}

void *abs_processor(void *args)
{
  // ActuatorInputPayload *payload = args;
  // char brake_engaged = payload->brake_level;
  int brake_engaged = brake_level;
  while (1)
  {
    while (state != NOT_ACQUIRED && state != ABS_STATE)
    {
      pthread_cond_wait(&cond, &mutex);
    }

    brake_engaged = !brake_engaged;
    printf("Brake set to: %d\n", !brake_engaged);
    usleep(200 * 1000); 
  }
  return NULL;
}

char has_higher_prio(unsigned short curr_state) {
  return curr_state >= state;
}

void copy_man_input_payload(ManMessageInput *input, ManMessageInput *copied) {
  copied->brake_level = input->brake_level; 
  copied->throttle_level = input->throttle_level;
}

void copy_acc_input_payload(AccMessageInput *input, AccMessageInput *copied) {
  copied->brake_level = input->brake_level; 
  copied->throttle_level = input->throttle_level;
  copied->distance = input->distance;
}

void copy_abs_input_payload(AbsMessageInput *input, AbsMessageInput *copied) {
  copied->skid = input->skid; 
}