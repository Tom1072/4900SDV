#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <sys/iomsg.h>
#include <limits.h>

#include "../includes/commons.h"
#include "../includes/simulator.h"
#include "../includes/car_structs.h"
#include "../includes/CommListener.h"

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

    case COMM:
    {
      // Here we fill the Environment values and fill the car and obj structs
      // else if the pulse is something else print the code and value of the pulse
      CommListenerMessage *comm_message = (CommListenerMessage *)message.value.sival_ptr;
      CommandData data = comm_message->data;
      CommandType command = comm_message->command;

      switch (command)
      {
        case SPAWN_CAR:
          car_env.object = TRUE;
          car_env.obj_speed = data.spawn_car_data.obj_speed;
          car_env.distance = data.spawn_car_data.distance;

          car.distance = data.spawn_car_data.distance;

          other_car.object = TRUE;
          other_car.distance = data.spawn_car_data.distance;
          other_car.speed = data.spawn_car_data.obj_speed;
          other_car.init_speed = data.spawn_car_data.obj_speed;
          break;
        case DESPAWN_CAR:
          car_env.object = FALSE;
          car_env.obj_speed = 0;
          car_env.distance = 0;

          car.distance = USHRT_MAX;

          other_car.object = FALSE;
          other_car.distance = USHRT_MAX;
          other_car.speed = 0;
          other_car.init_speed = 0;
          break;
        case THROTTLE:
          car.throttle_level = data.throttle_level;
          break;
        case BRAKE:
          car.brake_level = data.brake_level;
          break;
        case SKID:
          car_env.skid = data.skid_on;
          break;
        default:
          printf("Simulator*** Unknown command\n");
      }
      free(comm_message);
    }

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
void update_brake_level( unsigned short level, Sensors* sensors)
{
  sensors->brake_level = level;
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
unsigned short get_brake_level( Sensors* sensors )
{
  return sensors->brake_level;
}
char get_object( OutsideObject* object )
{
  return object->object;
}

