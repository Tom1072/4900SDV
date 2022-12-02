#pragma once

#include <sys/dispatch.h>
#include <pthread.h>


#define SERVER_PORT 5003
#define CLIENT_PORT 8080
#define BUFFER_SIZE 1000

// Constants
#define TRUE  1
#define FALSE 0
#define OK    0
#define NOK  -1

#define MAX_STRING_LEN    512

/** Log levels */
#define LOG_LEVEL_DEBUG FALSE
#define LOG_LEVEL_INFO TRUE
#define LOG_LEVEL_ERROR TRUE

/** Macros */
#define PRINT_ON_DEBUG(...) if (LOG_LEVEL_DEBUG) printf(__VA_ARGS__)  // Print only if DEBUG is TRUE
#define PRINT_ON_INFO(...) if (LOG_LEVEL_INFO) printf(__VA_ARGS__)    // Print only if INFO is TRUE
#define PRINT_ON_ERROR(...) if (LOG_LEVEL_ERROR) printf(__VA_ARGS__)  // Print only if ERROR is TRUE

// Check status and return if NOK
#define CHECK_STATUS(status) \
    if (status != OK) \
    { \
        return FALSE; \
    } \

/** Thread priorities */
#define MANUAL_PRIO    12
#define ACC_PRIO       11
#define ABS_PRIO       13
#define COMM_PRIO       9
#define SIMULATOR_PRIO 10

/** Configurations */

// Socket configurations: SERVER_ADDRESS is the IP address of the CommListener server
//                        VIEW_SERVER_ADDRESS is the IP address of the ViewDispatcher server
//#define COMM_SERVER_ADDRESS "192.168.56.101"      // Tom's IP
//#define VIEW_SERVER_ADDRESS "192.168.56.1"        // Tom's IP
 #define COMM_SERVER_ADDRESS "192.168.2.226"        // Kate's IP
 #define VIEW_SERVER_ADDRESS "192.168.2.245"        // Kate's IP
// #define COMM_SERVER_ADDRESS "172.16.41.129"      // Thang's IP
// #define VIEW_SERVER_ADDRESS "172.16.41.1"        // Thang's IP

#define COMM_SERVER_PORT 8080 // Port that the CommListener will listen to for ViewDispatcher messages
#define VIEW_SERVER_PORT 8080 // Port that the ViewListener will listen to for CommDispatcher messages
#define BUFFER_SIZE 1000      // Size of the buffer for the socket messages

// General configurations
#define MAX_SPEED         200   // The computer-limited car speed
#define TIME_INTERVAL      50    // (ms) The time interval between each updates. This affects how frequently the the program update speed, distance, etc.

// ACC configurations
#define MIN_DISTANCE 5                            // (m) the minimum distance between this car and the lead car
#define MAX_DEACCELERATION -15                    // (m/s^2) the maximum deacceleration of the car when braking
#define SPEED_CONTROL_DEFAULT_ACCELERATION 1      // (m/s^2) the default acceleration of the car when in speed control mode
#define SPEED_CONTROL_DEFAULT_DEACCELERATION -1   // (m/s^2) the default deacceleration of the car when in speed control mode
#define SPEED_ERROR_TOLERANCE 0                   // (m/s) the error tolerance of the speed control mode
#define DISTANCE_BUFFER 3

/** Channel names for pulse sending */
#define MANUAL_NAME     "manual_driver_attach_name"
#define ABS_NAME        "ABS_attach_name"
#define ACC_NAME        "ACC_attach_name"
#define SIMULATOR_NAME  "simulator_attach_name"
#define COMM_NAME       "comm_attach_name"
#define FILENAME        "data_log.csv"

/** Pulse Code to indicate the pulse sender */
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

/** Simulator public structs */
// The message that Simulator will send to CommDispatcher for environment update
typedef struct
{
  volatile char   skid;         // level of skid
  short  brake_level;           // current brake position
  short  throttle_level;
  volatile double distance;     // distance from car to object
  double          car_speed;    // car current speed
  double          obj_speed;    // object in front speed
  double          set_speed;    // ACC set speed
  char            object;       // can be either TRUE or FALSE if not set
  pthread_mutex_t mutex;        // to lock the data for distance simulation
  pthread_cond_t  cond;
} Environment;

// The message struct that Simulator will send to ACC for distance update and acc speed request
typedef struct
{
  short brake_level;
  short throttle_level;
  double current_speed;
  double desired_speed;
  double distance;
} AccMessageInput;

/** Actuators (ACC + ABS + Manual) Public structs */
// The message struct that Simulator will send to ABS for skid sensor update*/
typedef struct
{
  volatile char skid; } AbsMessageInput;

// The message struct that Simulator will send to Manual for throttle and brake request
typedef struct
{
  short brake_level;
  short throttle_level;
} ManMessageInput;

// The message struct that the Actuators (ACC+ABS+Manual) will send to Simulator for true brake, throttle, and speed update
typedef struct {
  short brake_level;
  short throttle_level;
  double speed;
} ActuatorOutputPayload;

/** CommListener public structs*/
// User input commands
typedef enum
{
  SPAWN_CAR = 0,  // Spawn a car, use spawn_car_data for associated data
  DESPAWN_CAR,    // Despawn a car, no associated data
  THROTTLE,       // Change throttle level, use throttle_level for associated data
  BRAKE,          // Change brake level and indicate skid simulation or not, use brake_data for associated data
  ACC_SPEED,      // Change ACC speed, use acc_speed for associated data
} CommandType;

// Data associated to the SPAWN_CAR command
typedef struct
{
  double distance;
  int obj_speed;
} SpawnCarData;

// Data associated to the BRAKE command
typedef struct
{
  int brake_level;
  char skid_on;
} BrakeData;

// The union of all possible data associated with all commands
typedef union
{
  SpawnCarData spawn_car_data; // associated data for SPAWN_CAR command
  int throttle_level;          // 0-100 (%), associated data for GAS command
  BrakeData brake_data;        // associated data for BRAKE command
  double acc_speed;            // 0-100 (km/h), associated data for ACC_SPEED command
} CommandData;

// The message structure that CommListener will send to Simulator for user commands
typedef struct
{
  CommandType command;
  CommandData data;
} CommListenerMessage;
