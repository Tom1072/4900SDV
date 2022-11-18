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

volatile unsigned short brake_level = 0;
volatile unsigned short throttle_level = 0;
volatile double speed = 0;

/**
 * These 3 is for set_state() function to determine
 * the next important state to be taken over
*/
volatile char man_processing = FALSE;
volatile char abs_processing = FALSE;
volatile char acc_processing = FALSE;

void *man_processor(void *args);
void *acc_processor(void *args);
void *abs_processor(void *args);

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

  printf("Manual Driver attached\n");
  if ((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
    pthread_exit(NULL);

  // Create the child processor thread
  processed_input = malloc(sizeof(ManMessageInput));
  pthread_create(&processor_thread, NULL, man_processor, processed_input);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    printf("Manual got input\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    pthread_mutex_lock(&mutex);
    input = (ManMessageInput *)pulse_msg.value.sival_ptr;

    // If input is valid (not both are engaged)
    if (!(input->brake_level > 0 && input->throttle_level > 0))
    {
      copy_man_input_payload(input, processed_input);

      // If both are 0 -> no manual input, no need to process, and vice versa.
      if (input->brake_level == 0 && input->throttle_level == 0)
        man_processing = FALSE;
      else
        man_processing = TRUE;

      // brake_level = input->brake_level;
      set_state(); // IMPORTANT: set determine the next state machine to run
    }

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    free(input);
  }

  free(processed_input);
  return NULL;
}

/**
 * Handler of the ACC thread.
 */
void *ACC()
{
  name_attach_t *attach;
  struct _pulse pulse_msg;
  AccMessageInput *input, *processed_input;
  pthread_t processor_thread;

  printf("ACC attached\n");
  if ((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
    pthread_exit(NULL);

  // Create the child processor thread
  processed_input = malloc(sizeof(AccMessageInput));
  pthread_create(&processor_thread, NULL, acc_processor, processed_input);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    printf("ACC got input\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    pthread_mutex_lock(&mutex);
    input = (AccMessageInput *)pulse_msg.value.sival_ptr;
    copy_acc_input_payload(input, processed_input);

    // If desired_speed == 0, ACC is turned off
    if (input->desired_speed == 0)
      acc_processing = FALSE;
    else
      acc_processing = TRUE;

    set_state(); // IMPORTANT: set determine the next state machine to run

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    free(input);
  }

  free(processed_input);
  return NULL;
}

/**
 * Handler of the ABS thread.
 */
void *ABS()
{
  name_attach_t *attach;
  struct _pulse pulse_msg;
  AbsMessageInput *input, *processed_input;
  char skidding = FALSE;
  pthread_t processor_thread;

  printf("ABS attached\n");
  if ((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
    pthread_exit(NULL);

  // Create the child processor thread
  processed_input = malloc(sizeof(ManMessageInput));
  pthread_create(&processor_thread, NULL, abs_processor, processed_input);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    printf("ABS got input\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    pthread_mutex_lock(&mutex);
    input = (AbsMessageInput *)pulse_msg.value.sival_ptr;
    copy_abs_input_payload(input, processed_input);

    printf("Skidding: %d, input skidding: %d\n", skidding, input->skid);
    if (!skidding && input->skid) // If currently not skidding, and receive skidding input
    {
      skidding = TRUE;
      abs_processing = TRUE;
    }
    else if (skidding && !input->skid) // If currently skidding, and receive not skidding input
    {
      skidding = FALSE;
      abs_processing = FALSE;
    }

    set_state(); // IMPORTANT: set determine the next state machine to run
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    free(input);
  }

  free(processed_input);
  return NULL;
}

/**
 * Handler function for manual driver processor/sender that sends pulse to Simulator
*/
void *man_processor(void *args)
{
  usleep(100000);
  int sim_coid = name_open(SIMULATOR_NAME, 0);
  ManMessageInput *data = args;

  while (1)
  {
    pthread_mutex_lock(&mutex);

    while (state != MANUAL_DRIVER_STATE)
      pthread_cond_wait(&cond, &mutex);
    
    // printf("data throttle %d\n", data->throttle_level);

    // If brake_level == 0, throttle is engaging
    // and if throttle_level == 0, brake is engaging
    if (data->brake_level == 0) {
      speed = min(MAX_SPEED, speed + ACTUATOR_SPEED_CHANGE);
    } else {
      speed = max(0, speed - ACTUATOR_SPEED_CHANGE);
    }

    sendUpdates(sim_coid, MANUAL_DRIVER, brake_level, throttle_level, speed);
    usleep(50 * 1000);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

/**
 * Handler function for ACC processor/sender that sends pulse to Simulator
*/
void *acc_processor(void *args)
{
  usleep(100000);
  int sim_coid = name_open(SIMULATOR_NAME, 0);
  AccMessageInput *data = args;

  while (1)
  {
    pthread_mutex_lock(&mutex);

    while (state != ACC_STATE)
      pthread_cond_wait(&cond, &mutex);

    // When distance > slow threshold, no object ahead
    if (data->distance > ACC_SLOW_THRESHOLD)
    {
      // Current speed is already >= desired speed, nothing needed to be done
      if (data->current_speed == data->desired_speed)
      {
        printf("Current speed == desired speed & no object in front.\nNothing to be done.");
      }
      else
      {
        // Otherwise, engage throttle or slow down if current > desired speed
        printf("ACC: engaging GAS and calculating new speed\n");
      }
    }
    // TODO Physics: handle thresholds dynamically based on distance and rate of dist decrease
    // If distance in the middle of the two thresholds, slowing down by disengage throttle
    else if (data->distance <= ACC_SLOW_THRESHOLD && data->distance > ACC_STOP_THRESHOLD)
    {
      printf("ACC: STOP engaging GAS and calculating REDUCED speed\n");
    }
    else // Otherwise (<= stop threshold), engage the brake until out of situation
    {
      printf("ACC: engaging brake to STOP\n");
    }
    usleep(200 * 1000);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

/**
 * Handler function for ABS processor/sender that sends pulse to Simulator
*/
void *abs_processor(void *args)
{
  usleep(100000);
  int sim_coid = name_open(SIMULATOR_NAME, 0);
  // AbsMessageInput *data = args;
  int brake_engaged = brake_level;

  while (1)
  {
    pthread_mutex_lock(&mutex);

    while (state != ABS_STATE)
      pthread_cond_wait(&cond, &mutex);

    brake_engaged = !brake_engaged; // Pulsing the brake
    printf("ABS: brake set to %d\n", brake_engaged);
    sendUpdates(sim_coid, ABS_CODE, brake_level, throttle_level, speed);
    usleep(200 * 1000);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

/**
 * Get the state with next highest priority which is waiting
*/
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

/**
 * Create a copy of Manual Driver input to avoid original being freed or changed
*/
void copy_man_input_payload(ManMessageInput *input, ManMessageInput *copied)
{
  copied->brake_level = input->brake_level;
  copied->throttle_level = input->throttle_level;

  brake_level = input->brake_level;
  throttle_level = input->throttle_level;
}

/**
 * Create a copy of ACC input to avoid original being freed or changed
*/
void copy_acc_input_payload(AccMessageInput *input, AccMessageInput *copied)
{
  copied->brake_level = input->brake_level;
  copied->throttle_level = input->throttle_level;
  copied->distance = input->distance;
  copied->desired_speed = input->desired_speed;
  copied->current_speed = input->current_speed;

  brake_level = input->brake_level;
  throttle_level = input->brake_level;
  speed = input->current_speed;
}

/**
 * Create a copy of ABS input to avoid original being freed or changed
*/
void copy_abs_input_payload(AbsMessageInput *input, AbsMessageInput *copied)
{
  copied->skid = input->skid;
}