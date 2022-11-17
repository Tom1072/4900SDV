#pragma once

#include <sys/dispatch.h>

// Constants
#define TRUE 1
#define FALSE 0
#define OK 0
#define NOK -1

#define MAX_STRING_LEN    512

#define MANUAL_PRIO 2
#define ACC_PRIO 1
#define ABS_PRIO 3

// Names of the channels
#define MANUAL_NAME "manual_driver_attach_name"
#define ABS_NAME "ABS_attach_name"
#define ACC_NAME "ACC_attach_name"
#define SIMULATOR_NAME "simulator_attach_name"
#define COMM_NAME "comm_attach_name"

#define CHECK_STATUS(status) \
    if (status != OK) \
    { \
        return FALSE; \
    } \

typedef enum
{
    COMM = 0, // Msg from the communication module
    SIMULATOR,
    MANUAL_DRIVER,
    ACC_CODE,
    ABS_CODE,
    THROTTLE_ACTUATOR,
    BRAKE_ACTUATOR,
} PulseCode;

typedef struct
{
  unsigned short skid;         // level of skid
  unsigned short distance;     // distance from car to object
  unsigned short car_speed;    // car current speed
  unsigned short brake_level;   // current brake position
  unsigned short obj_speed;    // object in front speed
  char           object;       // can be either TRUE or FALSE if not set
} Environment;