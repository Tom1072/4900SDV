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

double calculate_speed(unsigned short brake_level, unsigned short throttle_level) {
  return 0.0;
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