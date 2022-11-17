#pragma once
#include "commons.h"

#define SERVER_ADRESS "192.168.56.101"
#define SERVER_PORT 5003
#define BUFFER_SIZE 1000

// Public types
typedef enum
{
  SPAWN_CAR = 0,
  DESPAWN_CAR,
  THROTTLE,
  BRAKE,
  SKID,
  ACC_SPEED
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
  int throttle_level;          // 0-100, used for GAS command
  int brake_level;             // 0-100, used for BRAKE command
  char skid_on;                // TRUE or FALSE
  int acc_speed;               // Used for ACC speed command
  BrakeData brake_data;        // Used for BRAKE command
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
