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
  int                     coid_acc, coid_abs, coid_driver, coid_comm;
  int                     rcvid, reply_display;
  Sensors                 car;
  OutsideObject           other_car;
  Environment             car_env;
  unsigned short          brake, gas;
  pthread_t               distance_simulator;

  // initialize the sensor information to 0
  init_env(&car, &other_car, &car_env);
  pthread_mutex_init(&car_env.mutex, NULL);

  // Create a thread for distance simulation
  pthread_create(&distance_simulator, NULL, simulate_distance, &car_env);

  // Create Channel
  if((attach = name_attach(NULL, SIMULATOR_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("name_attach():");
    exit(EXIT_FAILURE);
  }

  // Create a channel to use as client for sending pulses
  sleep(1); // to allow them to attach to simulator
  coid_acc = name_open(ACC_NAME, 0);
  coid_abs = name_open(ABS_NAME, 0);
  coid_driver = name_open(MANUAL_NAME, 0);
  coid_comm = name_open(COMM_NAME, 0);

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
    printf("***Simulator: message.code=%d\n", message.code);
    switch(message.code)
    {
    case _PULSE_CODE_DISCONNECT:
      printf("Simulator*** Client is gone\n");
      ConnectDetach(message.scoid);
      break;
    case ACC:
      gas = message.value.sival_int;
      update_speed(gas, &car);
      // Update the display Environment after each message
      Environment* new_env_t = (Environment*) malloc(sizeof(Environment));
      // fill the data
      copy_updates(&car_env, new_env_t);
      //Send the updates to display
      reply_display = MsgSendPulsePtr(coid_acc, 2, SIMULATOR, (void *)new_env_t);

      break;
    case ABS:
      brake = message.value.sival_int;
	  update_brake_level(brake, &car);
	  // Update the display Environment after each message
	  // Update the display Environment after each message
	  Environment* new_env_b = (Environment*) malloc(sizeof(Environment));
	  // fill the data
	  copy_updates(&car_env, new_env_b);
	  //Send the updates to display
	  reply_display = MsgSendPulsePtr(coid_acc, 2, SIMULATOR, (void *)new_env_b);
	  break;
    case MANUAL_DRIVER:
       gas = message.value.sival_int;
       update_speed(gas, &car);
       // Update the display Environment after each message
       Environment* new_env_d = (Environment*) malloc(sizeof(Environment));
       // fill the data
       copy_updates(&car_env, new_env_d);
       //Send the updates to display
       reply_display = MsgSendPulsePtr(coid_acc, 2, SIMULATOR, (void *)new_env_d);
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
      break;
    } default:
    	printf("***Simulator: code = %d. Unknown type\n", message.code);

    }
    printf("Car speed = %u\nCar brake level = %u\n", car.speed, car.brake_level);
    printf("Env vars: car speed = %u, brakes = %u, skid = %u, dist = %u, obj = %d\n",
  		      car.speed, car.brake_level, car.skid, car.distance, other_car.object);


  }
//  break; // Debug statement
  } // End while

  // Destroy mutex
  pthread_join(distance_simulator, NULL);
  pthread_mutex_destroy(&car_env.mutex);

  //remove the name from the namespace and destroy the channel
  name_close(coid_acc);
  name_close(coid_abs);
  name_close(coid_driver);
  name_close(coid_comm);
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

void copy_updates(Environment* old_env, Environment* new_env)
{
	new_env->skid        = old_env->skid;
	new_env->distance    = old_env->distance;
	new_env->car_speed   = old_env->car_speed;
	new_env->brake_level = old_env->brake_level;
	new_env->obj_speed   = old_env->obj_speed;
	new_env->object      = old_env->object;
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
void *simulate_distance(void *data)
{
  Environment *env = (Environment*) data;
  int t = 100; // ms
  float d_obj, d_car, dist;
  dist = d_obj = d_car = 0;
  sleep(1);
  // If object is set/spawned change distance between teh car and object
  while(1)
  {
	  pthread_mutex_lock(&env->mutex);
	  if((env->object == TRUE))
	  {
		d_obj = ((float)(env->obj_speed))*t/3600;
		d_car = ((float)(env->car_speed))*t/3600;
		dist  = (int)(env->distance) + (d_obj - d_car);
		//lock the dist var and update
		env->distance = (int)dist;
	  }
	  pthread_mutex_unlock(&env->mutex);
	  // Sleep 100 ms
	  usleep( t*1000 );
  }
  printf("No car in front OR you have crashed\n");
  return NULL;
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

