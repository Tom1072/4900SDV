
#ifndef SRC_BACKEND_INCLUDES_SIMULATOR_H_
#define SRC_BACKEND_INCLUDES_SIMULATOR_H_

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

void updateDistance( unsigned short, Sensors* , OutsideObject* );
void updateSpeed( unsigned short, Sensors* );
void updateSkid( unsigned short, Sensors* );
void updateBrakeLevel( unsigned short, Sensors* );
void setObject( OutsideObject* );
void removeObject( OutsideObject* );

// Getters for sending to display
unsigned short getSpeed( Sensors* );
unsigned short getDistance( Sensors* );
unsigned short getSkid( Sensors* );
unsigned short getBrakeLevel( Sensors* );
char getObject( OutsideObject* );



#endif /* SRC_BACKEND_INCLUDES_SIMULATOR_H_ */
