#pragma once

#include <sys/dispatch.h>

// Constants
#define TRUE 1
#define FALSE 0
#define OK 0
#define NOK -1

#define MAX_STRING_LEN    512

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
    BRAKE_ACTUATOR,
    THROTTLE_ACTUATOR
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

typedef union {
  struct _pulse pulse;
  int type; // BRAKE_ACTUATOR or THROTTLE_ACTUATOR
  int level; // 0-100
} actuatorChanges_t;
