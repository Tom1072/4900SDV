#pragma once

#include <sys/iomsg.h>
//#include "../includes/car_structs.h"
#include "../includes/commons.h"

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

int init(void);

//void init_env( Sensors* , OutsideObject*, Environment* );
void init_env( Environment* );
void copy_updates( Environment*, Environment* );

// Distance between object and car
void *simulate_distance( void * data);

// Simulate skid stop based on random event and speed
void simulate_skid_stop( void * data);

