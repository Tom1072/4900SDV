void *ManualDriver();
void *ACC();
void *ABS();

#define MANUAL_DRIVER_CODE 0
#define ACC_CODE 1
#define ABS_CODE 2

#define ACC_SLOW_THRESHOLD 4 // meters
#define ACC_SLOW_THRESHOLD 2 // meters


/**
 * Payload that can be used for incoming and outgoing communication between Actuators and Simulator
*/
typedef struct {
  int brake_level;
  int gas_level;
  int distance;
  int desired_speed;
  int current_speed;
  bool skidding;
  // int acceleration; // I just put this here in case it's needed later
} ActuatorPayload;

typedef struct {
  int type; // BRAKE_ACTUATOR or THROTTLE_ACTUATOR
  int level; // 0-100
} Speed;