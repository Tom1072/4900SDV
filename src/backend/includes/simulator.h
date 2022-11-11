
#pragma once


#define MAX_STRING_LEN    512
#define ATTACH_POINT "simulator"

#include <sys/iomsg.h>
#include "../includes/car_structs.h"

int init(void);

typedef union
{
	struct _pulse pulse;
    char rmsg [MAX_STRING_LEN +1];
} message_t;

void update_distance( unsigned short, Sensors* , OutsideObject* );
void update_speed( unsigned short, Sensors* );
void update_skid( unsigned short, Sensors* );
void update_brakeLevel( unsigned short, Sensors* );
void set_object( OutsideObject* );
void remove_object( OutsideObject* );

// Getters for sending to display
unsigned short get_speed( Sensors* );
unsigned short get_distance( Sensors* );
unsigned short get_skid( Sensors* );
unsigned short get_brakeLevel( Sensors* );
char get_object( OutsideObject* );


