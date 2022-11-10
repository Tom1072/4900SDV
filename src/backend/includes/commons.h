#include <sys/dispatch.h>

#define TRUE 1
#define FALSE 0
#define BRAKE_ACTUATOR 1
#define THROTTLE_ACTUATOR 2

typedef union {
  struct _pulse pulse;
  int type; // BRAKE_ACTUATOR or THROTTLE_ACTUATOR
  int level; // 0-100
} actuatorChanges_t;