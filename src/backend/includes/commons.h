#pragma once

#include <sys/dispatch.h>
#include <pthread.h>

// Socket configurations
#define SERVER_ADRESS "192.168.56.101"   // Tom's IP  
#define VIEW_SERVER_ADDRESS "192.168.56.1"   // Tom's IP  
// #define SERVER_ADRESS "192.168.2.226" // Kate's IP
#define SERVER_PORT 5003
#define CLIENT_PORT 8080
#define BUFFER_SIZE 1000

// Constants
#define TRUE  1
#define FALSE 0
#define OK    0
#define NOK  -1

#define MAX_STRING_LEN    512
#define MAX_SPEED         100

#define MANUAL_PRIO    12
#define ACC_PRIO       11
#define ABS_PRIO       13
#define COMM_PRIO       9
#define SIMULATOR_PRIO 10

#define TIME_INTERVAL 100 // milliseconds

// ACC configurations
#define MIN_DISTANCE 5                          // m, the minimum distance between this car and the lead car
#define MAX_DEACCELERATION -15                  // m/s^2, the maximum deacceleration of the car when braking
#define SPEED_CONTROL_DEFAULT_ACCELERATION 1    // m/s^2, the default acceleration of the car when in speed control mode
#define SPEED_CONTROL_DEFAULT_DEACCELERATION -1  // m/s^2, the default deacceleration of the car when in speed control mode
#define SPEED_ERROR_TOLERANCE 0.5                 // m/s, the error tolerance of the speed control mode
#define DISTANCE_BUFFER 3

#define DEBUG FALSE
#define PRINT_ON_DEBUG(...) if (DEBUG) printf(__VA_ARGS__)

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
  ACTUATORS,
  STOP_CODE  // To stop the vehicle
} PulseCode;

typedef struct
{
  volatile char   skid;         // level of skid
  short  brake_level;  // current brake position
  short  throttle_level;
  volatile double distance;     // distance from car to object
  double          car_speed;    // car current speed
  double          obj_speed;    // object in front speed
  double          set_speed;    // ACC set speed
  char            object;       // can be either TRUE or FALSE if not set
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
  short brake_level;
  short throttle_level;
  double current_speed;
  double desired_speed;
  double distance;
} AccMessageInput;

typedef struct
{
  volatile char skid;
} AbsMessageInput;

typedef struct
{
  short brake_level;
  short throttle_level;
} ManMessageInput;


typedef struct {
  short brake_level;
  short throttle_level;
  double speed;
} ActuatorOutputPayload;
