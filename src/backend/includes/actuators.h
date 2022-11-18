#pragma once

void *ManualDriver();
void *ACC();
void *ABS();
void sendUpdates(int sim_coid, int code, unsigned short brake_level, unsigned short throttle_level, double speed);

#define ACC_SLOW_THRESHOLD 4 // meters
#define ACC_STOP_THRESHOLD 2 // meters

#define ACTUATOR_SPEED_CHANGE 1 // km/s

typedef enum {
  NOT_ACQUIRED = 0,
  MANUAL_DRIVER_STATE = 2,
  ACC_STATE = 1,
  ABS_STATE = 3,
} ControllerState;

// TODO: Separate these
/**
 * Payload that can be used for incoming and outgoing communication between Actuators and Simulator
*/
typedef struct {
  unsigned short brake_level;
  unsigned short throttle_level;
  unsigned short distance;
  unsigned short desired_speed;
  unsigned short current_speed;
  unsigned short skidding; // only 0 or 1
  // int acceleration; // I just put this here in case it's needed later
} ActuatorInputPayload;

typedef struct
{
  unsigned short brake_level;
  unsigned short throttle_level;
  double current_speed;
  double desired_speed;
  double distance;
} AccMessageInput;

typedef struct
{
  unsigned short skid;
} AbsMessageInput;

typedef struct
{
  unsigned short brake_level;
  unsigned short throttle_level;
} ManMessageInput;


typedef struct {
  unsigned short brake_level;
  unsigned short throttle_level;
  double speed;
} ActuatorOutputPayload;
