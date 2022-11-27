#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <math.h>
#include <assert.h>
#include "../includes/actuators.h"
#include "../includes/commons.h"
#include "../includes/utils.h"

#define SPEED_THRESHOLD_COEFFICIENT 0.6
#define THROTTLE_DISENGAGED_COEFFICIENT 0.6
#define FRICTION_COEFFICIENT 1.0
#define BRAKE_COEFFICIENT 0.05

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

// 100 km/h | a = -10 m/s^2 | -> -10m/s -> -36 km/h^2 => km/h
/**
 * Assign brake and throttle so that the speed is changed to the new speed in 1s
 * @param desired_acceleration  (m/s^2)
 */
void calculate_brake_and_throttle_levels(double acceleration) {
  double desired_speed_change_per_interval = acceleration * ((double)TIME_INTERVAL / 1000);
  double desired_speed_change = desired_speed_change_per_interval * 3.6; // km/h
  // printf("desired_speed_change_per_interval: %lf, desired_speed_change: %lf\n", desired_speed_change_per_interval, desired_speed_change);
  // printf("a: %lf, desired speed change: %lf\n", acceleration, desired_speed_change);
  // 
  if (desired_speed_change == 0) return;


  double speed_maintain_threshold = SPEED_THRESHOLD_COEFFICIENT * speed;
  if (desired_speed_change > 0) {
    // printf("SPEED MAINTAIN: %lf\n", speed_maintain_threshold);
    // double speed_maintain_threshold = SPEED_THRESHOLD_COEFFICIENT * speed;
    throttle_level = round(desired_speed_change * 12 + speed_maintain_threshold);
    brake_level = 0;
  } else {
    double speed_change_factor = (FRICTION_COEFFICIENT + THROTTLE_DISENGAGED_COEFFICIENT);
    double brake_threshold = (SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12) * speed_change_factor;
    desired_speed_change = -desired_speed_change;

    if (desired_speed_change < brake_threshold) {
      throttle_level = round((-desired_speed_change + 0.1) / (FRICTION_COEFFICIENT + THROTTLE_DISENGAGED_COEFFICIENT) * 12 + speed_maintain_threshold);
      brake_level = 0;
    } else {
      /**
       * max_speed_change = SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12
       * Delta S = max_speed_change * (1 + brake_level * BRAKE_COEFFICIENT)
      */
      double max_speed_change = SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12;
      brake_level = round(((desired_speed_change / max_speed_change) - FRICTION_COEFFICIENT) / BRAKE_COEFFICIENT);
      throttle_level = 0;
    }
  }
  speed = calculate_speed(speed, brake_level, throttle_level);
}

/**
 * Calculate speed based on brake_level or throttle_level
*/
double calculate_speed(double speed, int brake_level, int throttle_level) {
  if (brake_level > 0) {
    double max_speed_change = SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12; // MAX_SPEED with 0 throttle and 0 gas
    return max(0, speed - max_speed_change * (FRICTION_COEFFICIENT + brake_level * BRAKE_COEFFICIENT));
  }

  double speed_maintain_threshold = SPEED_THRESHOLD_COEFFICIENT * speed;
  double speed_change = (throttle_level - speed_maintain_threshold) / 12;

  // smaller: slow down, equals: maintain, larger: speed up
  if (throttle_level == speed_maintain_threshold)
    return speed;
  else if (throttle_level > speed_maintain_threshold)
    return min(MAX_SPEED, speed + speed_change);

  return max(0, speed + speed_change * (FRICTION_COEFFICIENT + THROTTLE_DISENGAGED_COEFFICIENT) - 0.1);
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
  
  char *state_name;

  switch (state)
  {
    case ABS_STATE:
      state_name = "ABS_STATE";
      break;
    case ACC_STATE:
      state_name = "ACC_STATE";
      break;
    case MANUAL_DRIVER_STATE:
      state_name = "MANUAL_DRIVER_STATE";
      break;
    default:
      state_name = "NOT_ACQUIRED";
      break;
  }

  printf("CURRENT STATE: %s\n", state_name);
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