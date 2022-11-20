#pragma once

#include <sys/dispatch.h>
#include <pthread.h>

// Constants
#define TRUE 1
#define FALSE 0
#define OK 0
#define NOK -1

#define MAX_STRING_LEN    512
#define MAX_SPEED         100

#define MANUAL_PRIO 2
#define ACC_PRIO 1
#define ABS_PRIO 3

#define TIME_INTERVAL 100 // milliseconds

// Names of the channels
#define MANUAL_NAME     "manual_driver_attach_name"
#define ABS_NAME        "ABS_attach_name"
#define ACC_NAME        "ACC_attach_name"
#define SIMULATOR_NAME  "simulator_attach_name"
#define COMM_NAME       "comm_attach_name"

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
	ACTUATORS
} PulseCode;

typedef struct
{
  volatile unsigned short skid;         // level of skid
  unsigned short    brake_level;        // current brake position
  unsigned short throttle_level;
  volatile double distance;     // distance from car to object
  double         car_speed;    // car current speed
  double         obj_speed;    // object in front speed
  double         set_speed;    // ACC set speed
  char           object;       // can be either TRUE or FALSE if not set
  pthread_mutex_t mutex;       // to lock the data for distance simulation
  pthread_cond_t  cond;
} Environment;

typedef struct
{
  Environment *env;
  int coid;
} simulatorRequest_t;

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
  char skid;
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