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
void update_skid( unsigned short, Sensors* );
void update_brake_level( unsigned short, Sensors* );
void set_object( OutsideObject* );
void remove_object( OutsideObject* );

// Distance between object and car
void *simulate_distance( void * data);

// Simulate skid stop based on random event and speed
void *simulate_skid_stop( void * data);

// Getters for sending to display
unsigned short get_speed( Sensors* );
unsigned short get_distance( Sensors* );
unsigned short get_skid( Sensors* );
unsigned short get_brake_level( Sensors* );
char get_object( OutsideObject* );


