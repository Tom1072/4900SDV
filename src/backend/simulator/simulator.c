#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <sys/iomsg.h>
#include <float.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>


#include "../includes/commons.h"
#include "../includes/simulator.h"
#include "../includes/CommListener.h"
#include "../includes/actuators.h"

/**
 * Simulate the environment: other car moving closer, skidding, etc.
 */

/**
 * @brief Starts the simulator
*/
int init(void){

  // Define variables

  struct _pulse           message;
  name_attach_t           *attach;
  int                     coid_acc, coid_abs, coid_driver, coid_comm;
  int                     rcvid;
  Environment             car_env;
  pthread_t               distance_simulator, skid_simulator;
  simulatorRequest_t      abs_request, acc_request;

  // initialize the sensor information to 0

  init_env( &car_env );
  pthread_mutex_init( &car_env.mutex, NULL );

  // Create Channel
  if( ( attach = name_attach( NULL, SIMULATOR_NAME, 0 ) ) == NULL )
  {
    //if there was an error creating the channel
    perror("name_attach():");
    exit( EXIT_FAILURE );
  }

  //PRINT_ON_DEBUG("****Simulator: attach id  = %d\n", );
  // Create a channel to use as client for sending pulses
  usleep(900000); // to allow them to attach to simulator
  coid_acc    = name_open( ACC_NAME, _NTO_CHF_DISCONNECT );
  coid_abs    = name_open( ABS_NAME, _NTO_CHF_DISCONNECT );
  coid_driver = name_open( MANUAL_NAME, _NTO_CHF_DISCONNECT );
  coid_comm   = name_open( COMM_NAME, _NTO_CHF_DISCONNECT );

  // Create a thread for distance simulation
  abs_request.env = acc_request.env = &car_env;
  abs_request.coid = coid_abs;
  acc_request.coid = coid_acc;

  pthread_create( &distance_simulator, NULL, &simulate_distance, (void *)&acc_request );
  pthread_create( &skid_simulator, NULL, (void *)simulate_skid_stop, (void *)&abs_request );
  //the server should keep receiving, processing and replying to messages

  while(1)
  {
    //code to receive message or pulse from client
	PRINT_ON_DEBUG("****Simulator: waiting to receive a pulse...\n");
    rcvid = MsgReceivePulse( attach->chid, (void *) &message, sizeof(message), NULL );
    PRINT_ON_DEBUG("****Simulator: rcvid = %d, scoid = %d\n", rcvid, message.scoid);
    if( rcvid == -1 )
    {
      perror("MsgReceivePulse()");
    }
    else if( rcvid == 0 )
    {
      switch( message.code )
      {
        case _PULSE_CODE_DISCONNECT:
        {
          PRINT_ON_DEBUG("****Simulator Client is gone\n");
          ConnectDetach( message.scoid );
          // Kill the distance and skid simulators
          int abs_return, acc_return, man_return, com_return;
          // First send disconnect to comm
          com_return = MsgSendPulse( coid_comm, -1, _PULSE_CODE_DISCONNECT, 0 );
          usleep(1);
          pthread_kill( distance_simulator, SIGTERM );
          pthread_kill( skid_simulator, SIGTERM );

          // Send the termination code to actuators
          abs_return = MsgSendPulse( coid_abs, -1, _PULSE_CODE_DISCONNECT, 0 );
          acc_return = MsgSendPulse( coid_acc, -1, _PULSE_CODE_DISCONNECT, 0 );
          man_return = MsgSendPulse( coid_driver, -1, _PULSE_CODE_DISCONNECT, 0 );

          if( ( abs_return != -1) && ( acc_return != -1 ) && ( man_return != -1 ) && ( com_return != -1 ))
          {
            // Terminate simulator itself
            //remove the name from the namespace and destroy the channel
        	printf("****Simulator: exiting\n");
            name_close( coid_acc );
            name_close( coid_abs );
            name_close( coid_driver );
            name_close( coid_comm );
            name_detach( attach, 0 );
            return( EXIT_SUCCESS );
          }
          break;
        }
        case ACTUATORS:
        {
          // Receive the output from any of them
          ActuatorOutputPayload * info = ( ActuatorOutputPayload *) message.value.sival_ptr;
          car_env.brake_level = info->brake_level;
          car_env.car_speed  = info->speed;
          car_env.throttle_level = info->throttle_level;
          free(info);
          // Update display
          Environment* new_env_t = ( Environment * ) malloc(sizeof(Environment) );
          // fill the data
          copy_updates( &car_env, new_env_t );
          // Send updates to display
          if( MsgSendPulsePtr( coid_comm, SIMULATOR_PRIO, SIMULATOR, ( void * )new_env_t ) == -1 )
          {
            perror("***Simulator: MsgSendPulsePtr()");
          }
          break;
        }
        case COMM:
        {
          // Here we fill the Environment values and fill the car and obj structs
          // else if the pulse is something else print the code and value of the pulse
          CommListenerMessage *comm_message = ( CommListenerMessage * )message.value.sival_ptr;
          CommandData data    = comm_message->data;
          CommandType command = comm_message->command;

          switch ( command )
          {
            case SPAWN_CAR:
            {
              // New car in front is set
              car_env.object       = TRUE;
              car_env.obj_speed    = data.spawn_car_data.obj_speed;
              car_env.distance     = data.spawn_car_data.distance;
              // Update display
              Environment* new_env_t = ( Environment * ) malloc(sizeof(Environment) );
              // fill the data
              copy_updates( &car_env, new_env_t );
              // TODO: remove print statement
              PRINT_ON_DEBUG("***Simulator SPAWN: Env vars: dist = %.2f, obj = %d\n", car_env.distance, car_env.object);
              // Send updates to display
              if( MsgSendPulsePtr( coid_comm, SIMULATOR_PRIO, SIMULATOR, ( void * )new_env_t ) == -1 )
              {
                perror("***Simulator: MsgSendPulsePtr()");
              }
              break;
            }
            case DESPAWN_CAR:
            {
              // Car in front is removed from the view
              car_env.object    = FALSE;
              car_env.obj_speed = 0;
              car_env.distance  = DBL_MAX;

              // Update display
              Environment* new_env_t = ( Environment * ) malloc(sizeof(Environment) );

              // fill the data
              copy_updates( &car_env, new_env_t );

              // TODO: remove print statement
              PRINT_ON_DEBUG("***Simulator DESPAWN: Env vars: dist = %.2f, obj = %d\n", car_env.distance, car_env.object);
              // Send updates to display
              if( MsgSendPulsePtr( coid_comm, SIMULATOR_PRIO, SIMULATOR, ( void * )new_env_t ) == -1 )
              {
                perror("***Simulator: MsgSendPulsePtr()");
                }
              break;
            }
            case THROTTLE:
            {
              // User presses gas pedal
              ManMessageInput *manual = ( ManMessageInput * )malloc( sizeof(ManMessageInput) );
              manual->brake_level = 0;
              manual->throttle_level = data.throttle_level;
              // TODO: remove print statement
              PRINT_ON_DEBUG("***Simulator THROTTLE: Env vars: gas = %d, brake = %d\n", manual->throttle_level, manual->brake_level);
              if( MsgSendPulsePtr(coid_driver, SIMULATOR_PRIO, SIMULATOR, (void *)manual ) == -1 )
              {
                perror("***Simulator: MsgSendPulsePtr()");
              }
              break;
            }
            case BRAKE:
            {
              // User presses brake pedal
              ManMessageInput *manual = ( ManMessageInput * )malloc( sizeof(ManMessageInput) );
              manual->brake_level = data.brake_data.brake_level;
              manual->throttle_level = 0; // This one may be obsolete
              // TODO: remove print statement
              PRINT_ON_DEBUG("***Simulator BRAKE: Env vars: skid = %d, brake = %d\n", data.brake_data.skid_on, manual->brake_level);
              if( MsgSendPulsePtr( coid_driver, SIMULATOR_PRIO, SIMULATOR, (void *)manual ) == -1 )
              {
                perror("***Simulator: MsgSendPulsePtr()");
              }
              usleep( 100 );
              car_env.skid = data.brake_data.skid_on;
              break;
            }

            case ACC_SPEED:
            {
              car_env.set_speed = data.acc_speed;
              AccMessageInput * acc_data = ( AccMessageInput * )malloc( sizeof(AccMessageInput) );
              acc_data->brake_level   = car_env.brake_level;
              acc_data->current_speed = car_env.car_speed;
              acc_data->desired_speed = data.acc_speed;
              acc_data->distance      = car_env.distance;
              // TODO: remove print statement
              if( MsgSendPulsePtr( coid_acc, SIMULATOR_PRIO, SIMULATOR, (void *)acc_data ) == -1 )
              {
                perror("***Simulator: MsgSendPulsePtr()");
              }
              break;
            }
            default:
              PRINT_ON_DEBUG("Simulator*** Unknown command\n");
          }
          free( comm_message );
          break;
        }
        default:
        {
          PRINT_ON_DEBUG("***Simulator: code = %d. Unknown type\n", message.code);
        }
      }
    }
  } // End while
  // Destroy mutex
  pthread_join( distance_simulator, NULL );
  pthread_join( skid_simulator, NULL );
  pthread_mutex_destroy( &car_env.mutex );

  //remove the name from the namespace and destroy the channel
  name_close( coid_acc );
  name_close( coid_abs );
  name_close( coid_driver );
  name_close( coid_comm );
  name_detach( attach, 0 );
  return EXIT_SUCCESS;
}

/**
 * @brief Initialize the simulator environement
 * 
 * @param env The environment to be initialized
 */
void init_env( Environment* env )
{
  env->skid         = 0;
  env->distance     = DBL_MAX;
  env->car_speed    = 0;
  env->throttle_level = 0;
  env->brake_level  = 0;
  env->obj_speed    = 0;
  env->object   = FALSE;
  env->set_speed    = 0;
}

/** 
 * @brief Update environemnts 
 * 
 * @param old_env The environment to be updated
 * @param new_env The new environment
 */
void copy_updates( Environment* old_env, Environment* new_env )
{
  new_env->skid        = old_env->skid;
  new_env->distance    = old_env->distance;
  new_env->car_speed   = old_env->car_speed;
  new_env->throttle_level = old_env->throttle_level;
  new_env->brake_level = old_env->brake_level;
  new_env->obj_speed   = old_env->obj_speed;
  new_env->set_speed   = old_env->set_speed;
  new_env->object      = old_env->object;
}

/**
 * @brief Start the distance simulator
 * 
 * @param data the data to be used by this thread for distance simulation
 */
void *simulate_distance(void *data)
{
  simulatorRequest_t *info = ( simulatorRequest_t* ) data;
  int t = 100; // ms
  double d_obj, d_car, dist;
  dist = d_obj = d_car = 0;

  // If object is set/spawned change distance between the car and object
  while(1)
  {
    if( ( info->env->object == TRUE ) && ( info->env->distance > 0 ) )
    {
      pthread_mutex_lock(&info->env->mutex);
      d_obj = ( info->env->obj_speed ) * t / 3600;
      d_car = ( info->env->car_speed ) * t / 3600;
      dist  = ( info->env->distance ) + ( d_obj - d_car );
      info->env->distance = dist;
      pthread_mutex_unlock( &info->env->mutex );

      // Send pulse to ACC
      AccMessageInput *message = ( AccMessageInput *) malloc( sizeof(AccMessageInput) );
      message->brake_level = info->env->brake_level;
      message->throttle_level = info->env->throttle_level;
      message->current_speed = info->env->car_speed;
      message->desired_speed = info->env->set_speed;
      message->distance = dist;
      if(MsgSendPulsePtr( info->coid, SIMULATOR_PRIO, SIMULATOR, (void *) message) == -1 )
      {
        perror(">>>>>Distance simulator: MsgSendPulsePtr():");
      }
    } // TODO: add the distance updates if no object to be distance_max
    else if( ( info->env->object == FALSE ))
    {
      // Send pulse to ACC
      AccMessageInput *message = ( AccMessageInput *) malloc( sizeof(AccMessageInput) );
      message->brake_level = info->env->brake_level;
      message->throttle_level = info->env->throttle_level;
      message->current_speed = info->env->car_speed;
      message->desired_speed = info->env->set_speed;
      message->distance = DBL_MAX;
      if(MsgSendPulsePtr( info->coid, SIMULATOR_PRIO, SIMULATOR, (void *) message) == -1 )
      {
        perror(">>>>>Distance simulator: MsgSendPulsePtr():");
      }
    }
    // Sleep 100 ms
    usleep( t * 1000 );
  }
  PRINT_ON_DEBUG("No car in front OR you have crashed\n"); // TODO: remove print statement
  return NULL;
}

/**
 * @brief Start the skid-stop simulator
 * 
 * @param data the data to be used by this thread for skid-stop simulation
 */
void simulate_skid_stop( void * data)
{
  simulatorRequest_t *info = ( simulatorRequest_t * ) data;
  int rand_int;

  PRINT_ON_DEBUG(">>>>>Skid simulator: init skid = %d\n", info->env->skid); // TODO: remove print statement
  while(1)
  {
    srand( time(0) );
    if( info->env->skid == 1 )
    {
      // First send the notification to ABS that skid happened
      AbsMessageInput *message_skid_on = ( AbsMessageInput *) malloc( sizeof( AbsMessageInput) );
      message_skid_on->skid = info->env->skid;
      if( MsgSendPulsePtr( info->coid, SIMULATOR_PRIO, SIMULATOR, (void *) message_skid_on) == -1 )
      {
        perror(">>>>>Skid simulator: MsgSendPulsePtr():");
      }

        // Then update skid after random time
        rand_int = (int)( ( 10 * rand() / RAND_MAX) );
        int total_sleep_time = rand_int * TIME_INTERVAL * 1000;
        int min_sleep = 100000;

        AbsMessageInput *message_skid_off;
        // Send updates to ABS
        int num_iterations = 100;

        for (int i=0; i<num_iterations; i++)
        {
          if (info->env->car_speed == 0)
          {
            // Send skid off rightaway
            pthread_mutex_lock( &info->env->mutex );
            info->env->skid = FALSE;
            message_skid_off = ( AbsMessageInput *) malloc( sizeof( AbsMessageInput) );
            message_skid_off->skid = FALSE;
            pthread_cond_broadcast( &info->env->cond) ;
            pthread_mutex_unlock( &info->env->mutex );

            if( MsgSendPulsePtr( info->coid, SIMULATOR_PRIO, SIMULATOR, (void *) message_skid_off) == -1 )
            {
              perror(">>>>>Skid simulator: MsgSendPulsePtr():");
            }
            break;
          }
          usleep(min_sleep / num_iterations);
          usleep( total_sleep_time / num_iterations);  // sleep for 500 ms 
        }

        if (info->env->skid == TRUE)
        {
          // Send skid off now
          pthread_mutex_lock( &info->env->mutex );
          info->env->skid = 0;
          message_skid_off = ( AbsMessageInput *) malloc( sizeof( AbsMessageInput) );
          message_skid_off->skid = 0;
          pthread_cond_broadcast( &info->env->cond) ;
          pthread_mutex_unlock( &info->env->mutex );

          if( MsgSendPulsePtr( info->coid, SIMULATOR_PRIO, SIMULATOR, (void *) message_skid_off) == -1 )
          {
            perror(">>>>>Skid simulator: MsgSendPulsePtr():");
          }
        }

        PRINT_ON_DEBUG(">>>>>Skid simulator: skid = %d\n", info->env->skid); // TODO: remove print statement
    }
  }
}

