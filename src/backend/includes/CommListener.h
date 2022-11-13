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
  SKID
} CommandType;

typedef struct
{
  int distance;
  int obj_speed;
} SpawnCarData;

typedef union
{
  SpawnCarData spawnCarData; // used for SPAWN_CAR command
  int throttleLevel;         // 0-100, used for GAS command
  int brakeLevel;            // 0-100, used for BRAKE command
  char skidOn;               // TRUE or FALSE
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
void startListener();

// Private API
void parseMessage(char *message, CommListenerMessage **parsedMessage);

char checkAtoiResult(int result, char *token);

char inRange(int value, int min, int max);