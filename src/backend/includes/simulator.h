#pragma once

#include <sys/iomsg.h>
#include "../includes/commons.h"

/**
 * Simulate the environment: other car moving closer, skidding, etc.
 */

/** Private structs */
typedef struct
{
  Environment *env;
  int coid;
} simulatorRequest_t;

typedef union
{
  struct _pulse pulse;
  char rmsg [MAX_STRING_LEN +1];
} message_t;

/** Public APIs */

/**
 * @brief Starts the simulator
*/
int init(void);

/** Private APIs */

/**
 * @brief Initialize the simulator environement
 * 
 * @param env The environment to be initialized
 */
void init_env( Environment* );

/** 
 * @brief Update environemnts 
 * 
 * @param old_env The environment to be updated
 * @param new_env The new environment
 */
void copy_updates(Environment* old_env, Environment* new_env);

/**
 * @brief Start the distance simulator
 * 
 * @param data the data to be used by this thread for distance simulation
 */
void *simulate_distance( void * data);

/**
 * @brief Start the skid-stop simulator
 * 
 * @param data the data to be used by this thread for skid-stop simulation
 */
void simulate_skid_stop( void * data);

