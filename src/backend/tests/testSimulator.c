#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <sys/iomsg.h>
#include <pthread.h>
#include <sched.h>
#include "../includes/tests.h"
#include "../includes/commons.h"
#include "../includes/simulator.h"
#include "../includes/utils.h"
#include "../includes/CommListener.h"
#include "../includes/actuators.h"


void *mocked_comm();
void *test_sim();
void *mocked_acc();
void *mocked_abs();
void *mocked_abs_server();
void *mocked_manual();
void *distance_test();
void *skid_test();
void *test_comm_sim_actuators_sim_display();

char test_simulator() {
  printf("Testing simulator...\n");
  pthread_t      test_comm_sim_actuators_sim_display_thread,
			           mocked_dist_thread, mocked_skid_thread;
  pthread_attr_t test_comm_sim_actuators_sim_display_attr,
				         mocked_dist_attr, mocked_skid_attr;

  create_thread(&test_comm_sim_actuators_sim_display_thread,
		        &test_comm_sim_actuators_sim_display_attr,
				1, NULL, test_comm_sim_actuators_sim_display);
//  create_thread(&mocked_dist_thread, &mocked_dist_attr, 2, NULL, distance_test);
//  create_thread(&mocked_skid_thread, &mocked_skid_attr, 2, NULL, skid_test);

  pthread_join(test_comm_sim_actuators_sim_display_thread, NULL);
//  pthread_join(mocked_dist_thread, NULL);
//  pthread_join(mocked_skid_thread, NULL);
//
  return TRUE;
}

void *test_comm_sim_actuators_sim_display()
{
  pthread_t      mocked_comm_thread, test_sim_thread,
				 mocked_acc_thread, mocked_abs_thread,
				 mocked_manual_thread;
  pthread_attr_t mocked_comm_attr, test_sim_attr,
  	  	  	  	 mocked_acc_attr, mocked_abs_attr,
				 mocked_manual_attr;

  create_thread(&test_sim_thread, &test_sim_attr, 1, NULL, test_sim);
  create_thread(&mocked_comm_thread, &mocked_comm_attr, 2, NULL, mocked_comm);
  create_thread(&mocked_acc_thread, &mocked_acc_attr, 2, NULL, mocked_acc);
  create_thread(&mocked_abs_thread, &mocked_abs_attr, 2, NULL, mocked_abs);
  create_thread(&mocked_manual_thread, &mocked_manual_attr, 2, NULL, mocked_manual);

  pthread_join(test_sim_thread, NULL);
  pthread_join(mocked_comm_thread, NULL);
  pthread_join(mocked_acc_thread, NULL);
  pthread_join(mocked_abs_thread, NULL);
  pthread_join(mocked_manual_thread, NULL);

  return NULL;
}
void *mocked_comm()
{

  printf("Testing communication...\n");
  int sim_coid;
  name_attach_t           *attach;
  struct _pulse           message;

  // Create Channel
  if((attach = name_attach(NULL, COMM_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("COMM name_attach():");
    exit(EXIT_FAILURE);
  }
  sleep(1);


  /// TEST 1 : SPAWN CAR ---------------------------------
  CommListenerMessage* msg = (CommListenerMessage*) malloc(sizeof(CommListenerMessage));
  msg->command = SPAWN_CAR;
  msg->data.spawn_car_data.distance = 60;
  msg->data.spawn_car_data.obj_speed = 20;

  sim_coid = name_open(SIMULATOR_NAME, 0);

  // Send only ONE message to Simulator and keep receiving update to the screen
  // Test only 5 updates
  int updates = 0;
  if( MsgSendPulsePtr(sim_coid, COMM_PRIO, COMM, (void *)msg) == -1)
  {
    perror("TEST COMM: MessageSendPulsePtr()");
  }

//---------------------------------------------------------------
  //------------- Server side ------------------------------------

  while(1)
  {
	  if( updates == 3)
	  {
		// TEST 2: DESPAWN CAR
	    CommListenerMessage* msg = (CommListenerMessage*) malloc(sizeof(CommListenerMessage));
	    msg->command = DESPAWN_CAR;
	    if( MsgSendPulsePtr(sim_coid, COMM_PRIO, COMM, (void *)msg) == -1)
	    {
	      perror("TEST COMM DESPAWN: MessageSendPulsePtr()");
	    }
	    // TEST 3: SET SKID
	    usleep(100*1000);
	    msg = (CommListenerMessage*) malloc(sizeof(CommListenerMessage));
	    msg->command = BRAKE;
	    msg->data.brake_data.skid_on = TRUE;
	    msg->data.brake_data.brake_level = 22;
	    if( MsgSendPulsePtr(sim_coid, COMM_PRIO, COMM, (void *)msg) == -1)
	    {
	      perror("TEST COMM SKID: MessageSendPulsePtr()");
	    }
	  }
	  if( MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL) == -1)
	  {
		perror("TEST COMM: MessageSendPulsePtr()");
	  }
	  switch(message.code)
	  {
		case _PULSE_CODE_DISCONNECT:
		{
		  printf("***DISPLAY Client is gone\n");
		  ConnectDetach(message.scoid);
		  break;
		}
		case SIMULATOR:
		{
		  Environment *data = (Environment *)message.value.sival_ptr;
		  printf("***DISPLAY: speed = %.2f, distance = %.2f, brake_level = %u, skid = %u, obj = %d\n\n",
				  data->car_speed, data->distance, data->brake_level, data->skid, data->object);
		  free(data);
		  break;
		}
		default:
		{
		  printf("DISPLAY: code value=%d\n", message.code);
		}
	  }
	  updates++;
	  if(updates > 20)
		  MsgSendPulse(sim_coid, COMM_PRIO, _PULSE_CODE_DISCONNECT, 0);
  }
  printf("TEST COMM: no more tests. While loop exited.\n");



// --------------------------------------------
  name_close(sim_coid);
  name_detach(attach, 0);
  return NULL;
}

void *mocked_acc()
{
  int sim_coid;
  printf("Testing acc replies...\n");
  name_attach_t 		  *attach;
  struct _pulse           message;

  //   Create Channel
  if((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("TEST ACC: name_attach():");
    exit(EXIT_FAILURE);
  }

  sleep(1);

  sim_coid = name_open(SIMULATOR_NAME, 0);
  // reply = MsgSendPulsePtr(sim_coid, -1, ACC_CODE, 4);

  while(1)
  {
	  if( MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL) == -1)
	  {
	    perror("TEST: MsgReceivePulse()");
	  }
    //	printf("TEST ACC: message.code = %d\n", message.code);
	  switch(message.code)
	  {
      case _PULSE_CODE_DISCONNECT:
      {
        printf("TEST ACC: Client is gone\n");
        ConnectDetach(message.scoid);
        break;
      }
      case STOP_CODE:
      {
        printf("TEST ACC: Client is exiting\n");
        ConnectDetach(message.scoid);
        name_close(sim_coid);
        name_detach(attach, 0);
        return NULL;
        break;
      }
      case SIMULATOR:
      {
        AccMessageInput *data = (AccMessageInput *)message.value.sival_ptr;
        printf("TEST ACC: Data received:  speed=%.2f, distance=%.2f, "
                        "brake_level=%u, gas=%u, set_speed=%.2f\n",
                        data->current_speed, data->distance,
                        data->brake_level, data->throttle_level,
                        data->desired_speed);
        free(data);
        // Send back data to mocked simulator to display the new data
        ActuatorOutputPayload * info = ( ActuatorOutputPayload *) malloc(sizeof(ActuatorOutputPayload));
        if(data->desired_speed == 0)
        {
          info->brake_level = data->brake_level;
          info->speed = data->current_speed;
          info->throttle_level = data->throttle_level;
        }else{
          info->brake_level = 1;
          info->speed = 1;
          info->throttle_level = 1;
        }
        if( MsgSendPulsePtr(sim_coid, ACC_PRIO, ACTUATORS, (void *) info) == -1)
        {
          perror("TEST: MsgReceivePulse()");
        }
        break;
      }
      default:
      printf("TEST ACC: code value unknown=%d\n", message.code);
	  }
  }

  name_close(sim_coid);
  name_detach(attach, 0);

  return NULL;
}

void *mocked_abs()
{
  int sim_coid;
  printf("Testing abs replies...\n");
  name_attach_t          *attach;
  struct _pulse          message;

  //   Create Channel
  if((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("ABS name_attach():");
    exit(EXIT_FAILURE);
  }

  sleep(1);
  sim_coid = name_open(SIMULATOR_NAME, 0);
  //-------------------------------------------------------
  //------- Receive the pulse from simulator --------------
  while(1)
  {
    if( MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL) == -1)
    {
      perror("TEST: MsgReceivePulse()");
    }
    switch(message.code)
    {
      case _PULSE_CODE_DISCONNECT:
      {
        printf("TEST ACC: Client is gone\n");
        ConnectDetach(message.scoid);
        break;
      }
      case STOP_CODE:
      {
        printf("TEST ABS: Client is exiting\n");
        ConnectDetach(message.scoid);
        name_close(sim_coid);
        name_detach(attach, 0);
        return NULL;
        break;
      }
      case SIMULATOR:
      {
        AbsMessageInput *input = (AbsMessageInput *)message.value.sival_ptr;
        printf("TEST ABS: Data received skid = %d\n", input->skid);
        free(input);
        // Assemble payload
        ActuatorOutputPayload *updates = malloc( sizeof(ActuatorOutputPayload) );
        updates->brake_level = 25;
        updates->throttle_level = 2;
        updates->speed = 10;
        if( MsgSendPulsePtr(sim_coid, ABS_PRIO, ACTUATORS, updates) == -1)
        {
          perror("TEST ABS: MsgSendPulse()");
        }
        break;
      }
      default:
      printf("TEST ABS: code value unknown=%d\n", message.code);
    }
  }

  name_close(sim_coid);
  name_detach(attach, 0);
  return NULL;
}

void *test_sim()
{
  init();
  return NULL;
}


void *mocked_abs_server()
{
  printf("Testing ABS_skid...\n");
  name_attach_t 		  *attach;
  struct _pulse           message;

  // Create Channel
  if((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("ABS name_attach():");
    exit(EXIT_FAILURE);
  }
  while(1)
  {
    printf("Waiting for message...\n");
    if( MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL) == -1)
    {
      perror("TEST: MsgReceivePulse()");
    }
    switch(message.code)
    {
      case _PULSE_CODE_DISCONNECT:
      {
        printf("***Mocked ABS: Client is gone\n");
        ConnectDetach(message.scoid);
        break;
      }
      case SIMULATOR:
      {
        AbsMessageInput *data = (AbsMessageInput *)message.value.sival_ptr;
        printf("***Mocked ABS: skid changed to = %d\n",data->skid);
        free(data);
        break;
      }
      default:
      printf("Mocked ABS: code value=%d\n", message.code);
    }
  }
  name_detach(attach, 0);
  return NULL;
}
void *mocked_manual()
{
  int sim_coid;
  printf("Testing manual replies...\n");
  name_attach_t 		  *attach;
  struct _pulse           message;

  //   Create Channel
  if((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("Manual name_attach():");
    exit(EXIT_FAILURE);
  }
  sleep(1);
  sim_coid = name_open(SIMULATOR_NAME, 0);
  //-------------------------------------------------------
  //------- Receive the pulse from simulator
  while(1)
  {
    if( MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL) == -1)
    {
      perror("TEST: MsgReceivePulse()");
    }
    switch(message.code)
    {
      case _PULSE_CODE_DISCONNECT:
      {
        printf("TEST Man: Client is gone\n");
        ConnectDetach(message.scoid);
        break;
      }
      case STOP_CODE:
      {
        printf("TEST Man: Client is exiting\n");
        ConnectDetach(message.scoid);
        name_close(sim_coid);
        name_detach(attach, 0);
        return NULL;
        break;
      }
      case SIMULATOR:
      {
        ManMessageInput *manual = ( ManMessageInput * )message.value.sival_ptr;
        printf("TEST Manual: received message brake_level = %u, gas = %u\n",
                      manual->brake_level, manual->throttle_level);
        free(manual);
        // Assemble payload
        ActuatorOutputPayload *updates = malloc( sizeof(ActuatorOutputPayload) );
        updates->brake_level = 45;
        updates->throttle_level = 2;
        updates->speed = 33;

        if( MsgSendPulsePtr(sim_coid, MANUAL_PRIO, ACTUATORS, updates) == -1)
        {
          perror("TEST Manual: MsgSendPulse()");
        }
        break;
        }
      default:
      printf("Mocked Man: code value=%d\n", message.code);
    }

  }
  name_close(sim_coid);
  return NULL;
}
void *distance_test()
{
  Environment        env;
  int                coid_acc;
  pthread_t          distance_simulator, mocked_acc_thread;
  simulatorRequest_t acc_request;

  init_env( &env );
  pthread_mutex_init(&env.mutex, NULL);

  // Create a thread for ACC server simulation
  pthread_create(&mocked_acc_thread, NULL, mocked_acc, NULL);
  printf("Distance_test pid = %d\n", getpid());
  sleep(1);
  coid_acc = name_open( ACC_NAME, 0 );
  // Create a thread for distance simulation
  acc_request.env = &env;
  acc_request.coid = coid_acc;//coid_acc;
  // Create a thread for distance simulation
  pthread_create( &distance_simulator, NULL, simulate_distance, (void *)&acc_request );

  // -----------------------------------------------------------
  // Test Approaching car -------------------------------------

  env.obj_speed = 50;
  env.car_speed = 70;
  env.object    = TRUE;
  env.distance  = 50;
  printf("TEST: Distance in meters initial: %.2f, obj = %d\n", env.distance, env.object);
  while(env.distance > 0)
  {
    usleep(100000);
    if(env.distance <= 0){
	  env.object = FALSE;
	  break;
    }
  }

  printf("Thread stopped calculating the distance\n");

  // -----------------------------------------------------------
  // Test disappearing car in front-----------------------------

  sleep(1);
  int i = 0;
  env.obj_speed = 60;
  env.car_speed = 70;
  env.object    = TRUE;
  env.distance  = 100;

  while(env.distance > -1)
  {
    usleep(500000);
    if(i > 10)
    {
      env.object = FALSE;
      break;
    } i++;
  }
  printf("Thread stopped calculating the distance\n");

  // -----------------------------------------------------------
  // Test car in front same speed  -----------------------------

  sleep(1);
  i = 0;
  env.obj_speed = 70;
  env.car_speed = 70;
  env.object    = TRUE;
  env.distance  = 100;

  while(env.distance > -1)
  {
    usleep(500000);
    if(i > 10)
    {
      env.object = FALSE;
      break;
      } i++;
  }
  printf("Thread stopped calculating the distance\n");

  // -----------------------------------------------------------
   // Test car in front accelerate -----------------------------

   i = 0;
   env.obj_speed = 50;
   env.car_speed = 40;
   env.object    = TRUE;
   env.distance  = 100;

   while(env.distance > -1)
   {
    usleep(500000);
    if(i > 20)
    {
      env.object = FALSE;
      break;
    } i++;
   }
   printf("Thread stopped calculating the distance\n");

  // Destroy mutex
  pthread_join(distance_simulator, NULL);
  pthread_mutex_destroy(&env.mutex);
  return NULL;
}

void *skid_test()
{
  Environment        env;
  simulatorRequest_t abs_request;
  pthread_t          skid_simulator_thread, mocked_abs_server_thread;

  init_env( &env );

  // Create a thread for ABS server simulation
  pthread_create(&mocked_abs_server_thread, NULL, mocked_abs_server, NULL);

  sleep(1);
  int coid_abs = name_open(ABS_NAME, 0);

  pthread_mutex_init(&env.mutex, NULL);
  // Create a thread for skid tread
  abs_request.env = &env;
  abs_request.coid = coid_abs;

  // Set the priority of the thread to higher than parent
  pthread_t simulator = pthread_self();
  struct sched_param param;
  int policy;

  pthread_getschedparam( simulator, &policy, &param );
  pthread_setschedprio( skid_simulator_thread , param.sched_priority + 2 );
  pthread_create( &skid_simulator_thread, NULL, simulate_skid_stop, (void *)&abs_request );
  // ----------------------------------------------------------------
  sleep(2); //let the skid run with skid=0

  env.skid = 1;
  printf("Car skid is set to %u by user\n", env.skid);
  while(env.skid != 0){
	  pthread_cond_wait(&env.cond, &env.mutex);
  }
  printf("Car skid should be 0 = %u\n", env.skid);

  sleep(2); //let the skid run with skid=0
  pthread_mutex_lock(&env.mutex);
  while (env.skid != 0)
  {
	pthread_cond_wait(&env.cond, &env.mutex);
  }
  env.skid = 1;
  pthread_cond_broadcast(&env.cond);
  pthread_mutex_unlock(&env.mutex);

  printf("Car skid  set to %u by user\n", env.skid);

  sleep(2); //let the skid run with skid=0
  while(env.skid != 0){
	  pthread_cond_wait(&env.cond, &env.mutex);
  }
  printf("Car skid should be 0 = %u\n", env.skid);

  sleep(2); //let the skid run with skid=0
  pthread_mutex_lock(&env.mutex);
  while (env.skid != 0)
  {
	pthread_cond_wait(&env.cond, &env.mutex);
  }
  env.skid = 1;
  pthread_cond_broadcast(&env.cond);
  pthread_mutex_unlock(&env.mutex);

  printf("Car skid  set to %u by user\n", env.skid);
  sleep(2); //let the skid run with skid=0
  while(env.skid != 0){
	  pthread_cond_wait(&env.cond, &env.mutex);
  }
  printf("Car skid should be 0 = %u\n", env.skid);

  name_close( coid_abs );
  pthread_join( skid_simulator_thread, NULL );
  pthread_join( mocked_abs_server_thread, NULL );
  pthread_mutex_destroy( &env.mutex );
  return NULL;
}
