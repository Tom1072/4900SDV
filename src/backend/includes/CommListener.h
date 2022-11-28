#pragma once
#include "commons.h"

//#define SERVER_ADRESS "192.168.56.101"
#define SERVER_ADRESS "192.168.2.226"
#define SERVER_PORT 5003
#define BUFFER_SIZE 1000

// Public types
typedef enum
{
  SPAWN_CAR = 0,
  DESPAWN_CAR,
  THROTTLE,
  BRAKE,
  ACC_SPEED,
  ACC_ENGAGE,
  SKID // Deprecated, use BRAKE instead
} CommandType;

typedef struct
{
  double distance;
  int obj_speed;
} SpawnCarData;

typedef struct
{
  int brake_level;
  char skid_on;
} BrakeData;

typedef union
{
  SpawnCarData spawn_car_data; // used for SPAWN_CAR command
  int throttle_level;          // 0-100 (%), used for GAS command
  BrakeData brake_data;        // Used for BRAKE command
  int acc_speed;               // 0-100 (km/h), used for ACC_SPEED command
  char acc_engage;                    // TRUE to engage acc or FALSE to disengage acc, used for ACC command
  int brake_level;             // Deprecated, use BrakeData instead
  char skid_on;                // Deprecated, use BrakeData instead
} CommandData;

typedef struct
{
  CommandType command;
  CommandData data;
} CommListenerMessage;

// Public API

/**
 * @brief Starts the listener server
 *
 */
void start_listener();

// Private API
void parse_message(char *message, CommListenerMessage **parsed_message);

char check_atoi(int result, char *token);

char in_range(int value, int min, int max);
