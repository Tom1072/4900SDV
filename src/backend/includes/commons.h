#include <sys/dispatch.h>

#define TRUE 1
#define FALSE 0
#define BRAKE_ACTUATOR 1
#define THROTTLE_ACTUATOR 2


typedef struct
{
  unsigned short skid;         // level of skid
  unsigned short distance;     // distance from car to object
  unsigned short car_speed;    // car current speed
  unsigned short brakeLevel;   // current brake position
  unsigned short obj_speed;    // object in front speed
  char           object;       // can be either TRUE or FALSE if not set
} Environment;

typedef union {
  struct _pulse pulse;
  int type; // BRAKE_ACTUATOR or THROTTLE_ACTUATOR
  int level; // 0-100
} actuatorChanges_t;

