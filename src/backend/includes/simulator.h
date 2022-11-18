#pragma once

#include <sys/iomsg.h>
#include "../includes/car_structs.h"
#include "../includes/commons.h"

int init(void);

typedef union
{
  struct _pulse pulse;
  char rmsg [MAX_STRING_LEN +1];
} message_t;

void init_env( Sensors* , OutsideObject*, Environment* );
void copy_updates( Environment*, Environment* );
void update_distance( unsigned short, Sensors* , OutsideObject* );
void update_speed( unsigned short, Sensors* );
void set_object( OutsideObject* );
void remove_object( OutsideObject* );

// Distance between object and car
void *simulate_distance( void * data);

// Simulate skid stop based on random event and speed
void *simulate_skid_stop( void * data);

