#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
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
  //message_t               message;
  struct _pulse           message;
  name_attach_t 		  *attach;
  int                     rcvid;
  Sensors                 car;
  OutsideObject           other_car;
  Environment             car_env;

  // Create Channel
  if((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL)
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

      // Here we have to define what codes we will treat as what
    default:
      // else if the pulse is something else print the code and value of the pulse
    // Here we fill the Environment values and fill the car and obj structs
      printf("Simulator*** Code: %d, Value: %d\n", message.code, message.value.sival_int);
      car_env.skid       = ((Environment *)message.value.sival_ptr)->skid;
      car_env.distance   = ((Environment *)message.value.sival_ptr)->distance;
      car_env.car_speed  = ((Environment *)message.value.sival_ptr)->car_speed;
      car_env.brakeLevel = ((Environment *)message.value.sival_ptr)->brake_level;
      car_env.obj_speed  = ((Environment *)message.value.sival_ptr)->obj_speed;
      car_env.object     = ((Environment *)message.value.sival_ptr)->object;

      // Update the values of the car and object
      car.skid        = car_env.skid;
      car.distance    = car_env.distance;
      car.brake_level  = car_env.brake_level;
      car.speed       = car_env.car_speed;

      other_car.distance   = car_env.distance;
      other_car.init_speed  = car_env.obj_speed;
      other_car.object     = car_env.object;
    }
  }

  } // End while
  //remove the name from the namespace and destroy the channel
  name_detach(attach, 0);
  printf("Test\n");

  return(EXIT_SUCCESS);
}


void update_distance( unsigned short value, Sensors* sensors, OutsideObject* obj)
{
  sensors->distance = value;
  obj->distance     = value;
}
void update_speed( unsigned short value, Sensors* sensors)
{
  sensors->speed = value;
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
char getObject( OutsideObject* object )
{
  return object->object;
}

