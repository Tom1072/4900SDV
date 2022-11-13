#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <sys/iomsg.h>
#include "../includes/commons.h"
#include "../includes/simulator.h"
#include "../includes/car_structs.h"


int init(void){

  // Define variables

  struct _pulse           message;
  name_attach_t 		  *attach;
  int                     rcvid;
  Sensors                 car;
  OutsideObject           other_car;
  Environment             car_env;
  unsigned short          brake, gas;

  // initialize the sensor information to 0
  init_env(&car, &other_car, &car_env);

  // Create Channel
  if((attach = name_attach(NULL, SIMULATOR_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("name_attach():");
    exit(EXIT_FAILURE);
  }

  //the server should keep receiving, processing and replying to messages
  while(1)
  {
    //code to receive message or pulse from client
  rcvid = MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL);
  printf("MsgReceivePulse: rcvid: %d\n", rcvid);
  if(rcvid == -1)
  {
    perror("MsgReceivePulse()");
  }
  else if(rcvid == 0)
  {
    switch(message.code)
    {
    case _PULSE_CODE_DISCONNECT:
      printf("Simulator*** Client is gone\n");
      ConnectDetach(message.scoid);
      break;
    case THROTTLE_ACTUATOR:
      gas = message.value.sival_int;
      update_speed(gas, &car);
      break;
    case BRAKE_ACTUATOR:
      brake = message.value.sival_int;
	    update_brake_level(brake, &car);
	    break;

    default:
    // Here we fill the Environment values and fill the car and obj structs
      printf("Simulator*** Code: %d, Value: %d\n", message.code, message.value.sival_int);
      car_env.skid       = ((Environment *)message.value.sival_ptr)->skid;
      car_env.distance   = ((Environment *)message.value.sival_ptr)->distance;
      car_env.car_speed  = ((Environment *)message.value.sival_ptr)->car_speed;
      car_env.brakeLevel = ((Environment *)message.value.sival_ptr)->brakeLevel;
      car_env.obj_speed  = ((Environment *)message.value.sival_ptr)->obj_speed;
      car_env.object     = ((Environment *)message.value.sival_ptr)->object;

      // Update the values of the car and object
      car.skid        = car_env.skid;
      car.distance    = car_env.distance;
      car.brakeLevel  = car_env.brakeLevel;
      car.speed       = car_env.car_speed;

      other_car.distance   = car_env.distance;
      other_car.initSpeed  = car_env.obj_speed;
      other_car.object     = car_env.object;
      other_car.initSpeed  = car_env.obj_speed;
    }
    printf("Car speed = %u\nCar brake level = %u\n", car.speed, car.brake_level);
    printf("Env vars: car speed = %u, brakes = %u, skid = %u, dist = %u, obj = %d\n",
  		      car.speed, car.brake_level, car.skid, car.distance, other_car.object);
  }
//  break; // Debug statement
  } // End while

  //remove the name from the namespace and destroy the channel
  name_detach(attach, 0);
  return(EXIT_SUCCESS);
}
void init_env(Sensors* sens, OutsideObject* obj, Environment* env)
{
  env->skid        = sens->skid                   = 0;
  env->distance    = sens->distance               = 0;
  env->car_speed   = sens->speed                  = 0;
  env->brake_level = sens->brake_level            = 0;
  env->obj_speed   = obj->init_speed = obj->speed = 0;
  env->object      = obj->object              = FALSE;
}

void update_distance( unsigned short value, Sensors* sensors, OutsideObject* obj)
{
  sensors->distance = value;
  obj->distance     = value;
}
void update_speed( unsigned short value, Sensors* sensors)
{
  sensors->speed += value;
}
void update_skid( unsigned short level, Sensors* sensors)
{
  sensors->skid = level;
}
void update_brakeLevel( unsigned short level, Sensors* sensors)
{
  sensors->brakeLevel = level;
}
void set_object( OutsideObject* object)
{
  object->object = TRUE;
}
void remove_object( OutsideObject* object)
{
  object->object = FALSE;
}


unsigned short get_speed( Sensors* sensors )
{
  return sensors->speed;
}
unsigned short get_distance( Sensors* sensors )
{
  return sensors->distance;
}
unsigned short get_skid( Sensors* sensors )
{
  return sensors->skid;
}
unsigned short getBrakeLevel( Sensors* sensors )
{
  return sensors->brakeLevel;
}
char get_object( OutsideObject* object )
{
  return object->object;
}

