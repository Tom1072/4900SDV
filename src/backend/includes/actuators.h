#pragma once

void *ManualDriver();
void *ACC();
void *ABS();

#define MANUAL_DRIVER_CODE 0
#define ACC_CODE 1
#define ABS_CODE 2

#define ACC_SLOW_THRESHOLD 4 // meters
#define ACC_STOP_THRESHOLD 2 // meters


/**
 * Payload that can be used for incoming and outgoing communication between Actuators and Simulator
*/
typedef struct {
  unsigned short brake_level;
  unsigned short gas_level;
  unsigned short distance;
  unsigned short desired_speed;
  unsigned short current_speed;
  unsigned short skidding; // only 0 or 1
  // int acceleration; // I just put this here in case it's needed later
} ActuatorInputPayload;

typedef struct {
  unsigned short brake_level;
  unsigned short gas_level;
  unsigned short speed;
} ActuatorOutputPayload;
