#pragma once
#include "commons.h"

#define SERVER_ADRESS "192.168.128.2"
#define SERVER_PORT 5003
#define BUFFER_SIZE 1000

// Public types
typedef enum
{
  SPAWN_CAR = 0,
  DESPAWN_CAR,
  GAS,
  BRAKE,
  SKID
} CommandType;

typedef struct
{
  CommandType command;
  Environment *environment;
} CommListenerMessage;

// Public API

/**
 * @brief Starts the listener server
 *
 */
void startListener();

// Private API
void parseMessage(char *message, CommListenerMessage *parsedMessage);
