#pragma once

#include <pthread.h>
#include "../includes/commons.h"

/**
 * All actuators definition (ABS, ACC, Manual)
 * 
 */

/**
 * @brief ACC States
 */
typedef enum {
  NOT_ACQUIRED = 0,
  MANUAL_DRIVER_STATE = 2,
  ACC_STATE = 1,
  ABS_STATE = 3,
} ControllerState;

/** Public APIs */
/**
 * @brief Start the ManualDriver actuator
 * 
 * @return void* 
 */
void *ManualDriver();

/**
 * @brief Start the ACC actuator
 * 
 * @return void* 
 */
void *ACC();

/**
 * @brief Start the ABS actuator
 * 
 * @return void* 
 */
void *ABS();

/** Private APIs */
/**
 * @brief Send update to the Simulator
 * 
 * @param sim_coid The Simulator connection ID
 * @param brake_level The updated brake level
 * @param throttle_level The updated throttle level
 * @param speed The updated car speed
 */
void sendUpdates(int sim_coid, short brake_level, short throttle_level, double speed);

/**
 * @brief Set the appropriate state for the actuator
 * 
 */
void set_state();

/**
 * @brief Start the processor for ManualDriver request from Simulator
 * 
 * @param args 
 * @return void* 
 */
void *man_processor(void *args);

/**
 * @brief Start the processor for ACC request from Simulator
 * 
 * @param args 
 * @return void* 
 */
void *acc_processor(void *args);

/**
 * @brief Start the processor for ABS request from Simulator
 * 
 * @param args 
 * @return void* 
 */
void *abs_processor(void *args);

/**
 * @brief Calculate and assign the throttle and brake level to achive 
 * the given speed change in the duration of TIME_INTERVAL
 * 
 * @param desired_speed_changed (meter) The desired speed change in the duration of TIME_INTERVAL
 */
void calculate_brake_and_throttle_levels(double desired_speed_changed);

/**
 * @brief Calculate the car speed after the duration TIME_INTERVAL based on the given 
 * brake and throttle level
 * 
 * @param speed The car speed to achive after duration TIME_INTERVAL
 * @param brake_level The given brake level
 * @param throttle_level The given throttle level
 * @return double 
 */
double calculate_speed(double speed, int brake_level, int throttle_level);
