#pragma once
#include "commons.h"

/**
 * Receive new user commands from the python ViewDispatcher client
 */

/** Public APIs */

/**
 * @brief Initialize the communication listener to receiving data from ViewDispatcher python client
 */
void start_listener();

/** Private APIs */

/**
 * @brief Parse the message received from the ViewDispatcher python client
 * 
 * @param message The message received from the ViewDispatcher python client
 * @param parsed_message The parsed message to be sent to the Simulator
 */
void parse_message(char *message, CommListenerMessage **parsed_message);

/**
 * @brief Check if atoi() was successful
 * 
 * @param result the result of atoi()
 * @param token the input to atoi() that produced the result
 * @return char TRUE if atoi() was successful, FALSE otherwise
 */
char check_atoi(int result, char *token);

/**
 * @brief Check if the input is within the specified range
 * 
 * @param value 
 * @param min 
 * @param max 
 * @return char 
 */
char in_range(int value, int min, int max);
