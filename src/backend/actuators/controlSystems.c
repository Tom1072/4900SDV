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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

volatile int brake_level = 0;
volatile char man_processing = FALSE;
volatile char abs_processing = FALSE;
volatile char acc_processing = FALSE;

void *man_processor();
void *acc_processor();
void *abs_processor();

void set_state();
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
  pthread_t processor_thread;
  pthread_attr_t processor_attr;

  printf("MAN attached\n");
  if ((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
    pthread_exit(NULL);
    // return (void *)EXIT_FAILURE;

  
  processed_input = malloc(sizeof(ManMessageInput));
  create_thread(&processor_thread, &processor_attr, 5, processed_input, man_processor);

  while (1)
  {
    printf("Manual waiting for pulse\n");
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    printf("Manual got pulse\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    input = (ManMessageInput *)pulse_msg.value.sival_ptr;

    pthread_mutex_lock(&mutex);
    if (!(input->brake_level > 0 && input->throttle_level > 0))
    {
      if (input->brake_level == 0 && input->throttle_level == 0) // both are 0 -> no manual input
        man_processing = FALSE;
      else // only one is engaging
        man_processing = TRUE;
        
        // if (has_higher_prio(MANUAL_DRIVER_STATE))
        //   state = MANUAL_DRIVER_STATE; // change to Manual state

      brake_level = input->brake_level;
      copy_man_input_payload(input, processed_input);
      set_state(); // IMPORTANT
    }

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    // free(input);
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
  AccMessageInput *input, *processed_input;
  pthread_t processor_thread;
  pthread_attr_t processor_attr;

  printf("ACC attached\n");
  if ((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
    pthread_exit(NULL);
    // return (void *)EXIT_FAILURE;


  processed_input = malloc(sizeof(AccMessageInput));
  create_thread(&processor_thread, &processor_attr, 4, processed_input, acc_processor);

  while (1)
  {
    printf("ACC waiting for pulse\n");
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    printf("ACC got pulse\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    input = (AccMessageInput *)pulse_msg.value.sival_ptr;

    pthread_mutex_lock(&mutex);
    // TODO: Add ON/OFF status for ACC
    acc_processing = TRUE;
    copy_acc_input_payload(input, processed_input);
    set_state(); // IMPORTANT

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    // free(input);
  }

  free(processed_input);
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
  AbsMessageInput *input, *processed_input;
  char skidding = FALSE;
  pthread_t processor_thread;
  pthread_attr_t processor_attr;

  printf("ABS attached\n");
  if ((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
    // return (void *)EXIT_FAILURE;
    pthread_exit(NULL);


  processed_input = malloc(sizeof(ManMessageInput));
  create_thread(&processor_thread, &processor_attr, 6, processed_input, abs_processor);

  while (1)
  {
    printf("ABS waiting for pulse\n");
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    printf("ABS got pulse\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    input = (AbsMessageInput *)pulse_msg.value.sival_ptr;
    copy_abs_input_payload(input, processed_input);

    printf("Skidding: %d, input skidding: %d\n", skidding, input->skid);
    if (!skidding && input->skid)
    {
      pthread_mutex_lock(&mutex);
      skidding = TRUE;
      abs_processing = TRUE;
    }
    else if (skidding && !input->skid)
    {
      skidding = FALSE;
      abs_processing = FALSE;
      pthread_mutex_unlock(&mutex);
    }

    set_state(); // IMPORTANT
    // free(input);
  }

  free(processed_input);
  return NULL;
}

void *man_processor(void *args)
{
  ManMessageInput *data = args;
  while (1)
  {
    while (state != MANUAL_DRIVER_STATE)
      pthread_cond_wait(&cond, &mutex);

    if (data->brake_level == 0) {
      printf("MAN: gas level %d\n", data->throttle_level);
    } else {
      printf("MAN: brake level %d\n", data->brake_level);
    }
    usleep(200 * 1000);
  }
  return NULL;
}

void *acc_processor(void *args)
{
  AccMessageInput *data = args;
  unsigned short brake_engaged = brake_level;
  unsigned short distance = data->distance;
  unsigned short current_speed = data->current_speed;
  unsigned short desired_speed = data->desired_speed;

  while (1)
  {
    while (state != ACC_STATE)
      pthread_cond_wait(&cond, &mutex);
    
    printf("123\n");

    brake_engaged = !brake_engaged;
    // printf("ABS: brake set to %d\n", brake_engaged);
    if (distance > ACC_SLOW_THRESHOLD) {
      if (current_speed >= desired_speed) {
        printf("MAN: gas %d, brake %d\n", data->throttle_level, data->brake_level);
        continue;
      }

      printf("ACC: engaging GAS and calculating new speed\n");
    } else if (distance <= ACC_SLOW_THRESHOLD && distance > ACC_STOP_THRESHOLD) {
      printf("ACC: STOP engaging GAS and calculating REDUCED speed\n");
    } else {
      printf("ACC: engaging brake to STOP\n");
    }
    usleep(200 * 1000);
  }
  return NULL;
}

void *abs_processor(void *args)
{
  // AbsMessageInput *data = args;
  int brake_engaged = brake_level;

  while (1)
  {
    while (state != ABS_STATE)
      pthread_cond_wait(&cond, &mutex);

    brake_engaged = !brake_engaged;
    printf("ABS: brake set to %d\n", brake_engaged);
    usleep(200 * 1000);
  }
  return NULL;
}

// get the state with next highest priority which is waiting
void set_state()
{
  if (abs_processing)
    state = ABS_STATE;
  else if (man_processing)
    state = MANUAL_DRIVER_STATE;
  else if (acc_processing)
    state = ACC_STATE;
  else
    state = NOT_ACQUIRED;
}

void copy_man_input_payload(ManMessageInput *input, ManMessageInput *copied)
{
  copied->brake_level = input->brake_level;
  copied->throttle_level = input->throttle_level;
}

void copy_acc_input_payload(AccMessageInput *input, AccMessageInput *copied)
{
  copied->brake_level = input->brake_level;
  copied->throttle_level = input->throttle_level;
  copied->distance = input->distance;
}

void copy_abs_input_payload(AbsMessageInput *input, AbsMessageInput *copied)
{
  copied->skid = input->skid;
}