#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <math.h>
#include "../includes/actuators.h"
#include "../includes/commons.h"
#include "../includes/utils.h"

#define SPEED_THRESHOLD_COEFFICIENT 0.6
#define SPEED_DECREASE_COEFFICIENT 1.6
#define BRAKE_COEFFICIENT 0.04

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
volatile char abs_processing = FALSE;
volatile char acc_processing = FALSE;

double calculate_brake_and_throttle_levels(double desired_speed_change) {
  double brake_threshold = -SPEED_THRESHOLD_COEFFICIENT * speed;

  if (desired_speed_change > brake_threshold) {
    /* Delta S = (throttle_level - SPEED_THRESHOLD_COEFFICIENT * speed) / 12 */
    throttle_level = round(desired_speed_change * 12 + SPEED_THRESHOLD_COEFFICIENT * speed);
    brake_level = 0;
  } else {
    /**
     * max_speed_change = SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12
     * Delta S = max_speed_change * (1 + brake_level * BRAKE_COEFFICIENT)
    */
    double max_speed_change = SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12;
    brake_level = round(((desired_speed_change / max_speed_change) - 1) / BRAKE_COEFFICIENT);
    throttle_level = 0;
  }

  speed = calculate_speed(speed, brake_level, throttle_level);
}

/**
 * Calculate speed based on brake_level or throttle_level
*/
double calculate_speed(double speed, int brake_level, int throttle_level) {
  if (brake_level > 0) {
    double max_speed_change = SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12; // with 0 throttle and 0 gas
    return max(0, speed - max_speed_change * (1 + brake_level * BRAKE_COEFFICIENT));
  }

  double speed_maintain_threshold = SPEED_THRESHOLD_COEFFICIENT * speed;
  double speed_change = (throttle_level - speed_maintain_threshold) / 12;

  // smaller: slow down, equals: maintain, larger: speed up
  if (throttle_level == speed_maintain_threshold)
    return speed;
  else if (throttle_level > speed_maintain_threshold)
    return min(MAX_SPEED, speed + speed_change);

  return max(0, speed + speed_change * SPEED_DECREASE_COEFFICIENT - 0.1);
}

/**
 * Get the state with next highest priority which is waiting
*/
void set_state()
{
  if (abs_processing)
    state = ABS_STATE;
  else if (throttle_level > 0 || brake_level > 0)
    state = MANUAL_DRIVER_STATE;
  else if (acc_processing)
    state = ACC_STATE;
  else
    state = MANUAL_DRIVER_STATE;
  printf("CURRENT STATE: %d\n", state);
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