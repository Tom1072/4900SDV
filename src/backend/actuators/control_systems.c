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

/** 
 * Actuator state machine and helper function
 */

#define SPEED_THRESHOLD_COEFFICIENT 0.6
#define THROTTLE_DISENGAGED_COEFFICIENT 0.6
#define FRICTION_COEFFICIENT 1.0
#define BRAKE_COEFFICIENT 0.05

volatile int state = NOT_ACQUIRED;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

volatile short brake_level = 0;
volatile short throttle_level = 0;
volatile double speed = 0;

/**
 * These 3 is for set_state() function to determine
 * the next important state to be taken over
*/
volatile char abs_processing = FALSE;
volatile char acc_processing = FALSE;
volatile char manual_processing = TRUE;

/**
 * @brief Calculate and assign the throttle and brake level to achive 
 * the given speed change in the duration of TIME_INTERVAL
 * 
 * @param desired_speed_changed (meter) The desired speed change in the duration of TIME_INTERVAL
 */
void calculate_brake_and_throttle_levels(double acceleration) {
  double desired_speed_change_per_interval = acceleration * ((double)TIME_INTERVAL / 1000);
  double desired_speed_change = desired_speed_change_per_interval * 3.6; // km/h
  if (desired_speed_change == 0) return;


  double speed_maintain_threshold = SPEED_THRESHOLD_COEFFICIENT * speed;
  if (desired_speed_change > 0) {
    throttle_level = round(desired_speed_change * 12 + speed_maintain_threshold);
    brake_level = 0;
  } else {
    double speed_change_factor = (FRICTION_COEFFICIENT + THROTTLE_DISENGAGED_COEFFICIENT);
    double brake_threshold = (SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12) * speed_change_factor;
    desired_speed_change = -desired_speed_change;

    if (desired_speed_change < brake_threshold) {
      throttle_level = round((-desired_speed_change + 0.1) / (FRICTION_COEFFICIENT + THROTTLE_DISENGAGED_COEFFICIENT) * 12 + speed_maintain_threshold);
      brake_level = 0;
      if (throttle_level > 0) {
        speed = calculate_speed(speed, brake_level, throttle_level);
        return;
      }
    }
    
    double max_speed_change = SPEED_THRESHOLD_COEFFICIENT * MAX_SPEED / 12;
    brake_level = -round(((-desired_speed_change / max_speed_change) - FRICTION_COEFFICIENT) / BRAKE_COEFFICIENT);
    throttle_level = 0;
  }
  speed = calculate_speed(speed, brake_level, throttle_level);
}

/**
 * @brief Calculate the car speed after the duration TIME_INTERVAL based on the given 
 * brake and throttle level
 * 
 * @param speed The car speed to achive after duration TIME_INTERVAL
 * @param brake_level The given brake level
 * @param throttle_level The given throttle level
 * @return double 
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
 * @brief Set the appropriate state for the actuator
 * 
 */
void set_state()
{
  // There must be at exactly one state engaged because of the speed 
  // update loop must run to send back to the simulator
  assert(abs_processing + acc_processing + manual_processing == 1);

  if (abs_processing)
    state = ABS_STATE;
  else if (manual_processing)
    state = MANUAL_DRIVER_STATE;
  else if (acc_processing)
    state = ACC_STATE;
  else
    assert(FALSE);
  
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

  PRINT_ON_DEBUG("CURRENT STATE: %s\n", state_name);
}
