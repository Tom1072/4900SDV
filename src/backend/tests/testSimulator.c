#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/dispatch.h>
#include <errno.h>
#include <sys/iomsg.h>
#include <pthread.h>
#include "../includes/tests.h"
#include "../includes/commons.h"
#include "../includes/simulator.h"
#include "../includes/car_structs.h"
#include "../includes/utils.h"
#include "../includes/CommListener.h"


void *mocked_comm();
void *test_sim();
void *mocked_acc();
void *mocked_abs();
void *mocked_abs_server();
void *distance_test();
void *skid_test();

char test_simulator() {
  printf("Testing simulator...\n");
//  pthread_t      mocked_comm_thread, test_sim_thread,
//  	  	  	     mocked_acc_thread, mocked_abs_thread,
//			           mocked_dist_thread, mocked_skid_thread;
//  pthread_attr_t mocked_comm_attr, test_sim_attr,
//  	  	  	  	 mocked_acc_attr, mocked_abs_attr,
//				         mocked_dist_attr, mocked_skid_attr;

<<<<<<< HEAD
//  create_thread(&test_sim_thread, &test_sim_attr, 1, test_sim);
//  create_thread(&mocked_comm_thread, &mocked_comm_attr, 2, mocked_comm);
//  create_thread(&mocked_acc_thread, &mocked_acc_attr, 2, mocked_acc); -- DO NOT USE THIS ON ITS OWN
//  create_thread(&mocked_abs_thread, &mocked_abs_attr, 2, mocked_abs);
//  create_thread(&mocked_dist_thread, &mocked_dist_attr, 2, distance_test);
//  create_thread(&mocked_skid_thread, &mocked_skid_attr, 2, skid_test);

//  pthread_join(test_sim_thread, NULL);
//  pthread_join(mocked_comm_thread, NULL);
//  pthread_join(mocked_acc_thread, NULL); -- DO NOT USE THIS ON ITS OWN
//  pthread_join(mocked_abs_thread, NULL);
//  pthread_join(mocked_dist_thread, NULL);
//  pthread_join(mocked_skid_thread, NULL);

  return TRUE;
}

void *mocked_comm()
{

  printf("Testing communication...\n");
  int sim_coid;
  name_attach_t 		  *attach;
  struct _pulse           message;


  // Create Channel
  if((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("ACC name_attach():");
    exit(EXIT_FAILURE);
  }
  sleep(1);

  CommListenerMessage* msg = (CommListenerMessage*) malloc(sizeof(CommListenerMessage));
  msg->command = SPAWN_CAR;
  msg->data.spawn_car_data.distance = 60;
  msg->data.spawn_car_data.obj_speed = 20;

  sim_coid = name_open(SIMULATOR_NAME, 0);

  if( MsgSendPulsePtr(sim_coid, -1, COMM, (void *)msg) == -1)
  {
    perror("TEST: MessageSendPulsePtr()");
  }

//---------------------------------------------------------------
  //------------- Server side ------------------------------------

  if( MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL) == -1)
  {
    perror("TEST: MessageSendPulsePtr()");
  }
    switch(message.code)
    {
    case _PULSE_CODE_DISCONNECT:
      printf("***DISPLAY Client is gone\n");
      ConnectDetach(message.scoid);
      break;
    case SIMULATOR:
      printf("***DISPLAY: message code = %d\n",message.code);
  	  Environment *data = (Environment *)message.value.sival_ptr;
  	  printf("***DISPLAY: speed = %d, distance = %d, brake_level = %d, skid = %d\n",
  			  data->car_speed, data->distance, data->brake_level, data->skid);
  	  free(data);
  	  break;
    default:
  	  printf("DISPLAY: code value=%d\n", message.code);
    }
// --------------------------------------------


  name_close(sim_coid);
  name_detach(attach, 0);
  return NULL;
}

void *mocked_acc()
{
//  int sim_coid;
  printf("Testing acc replies...\n");
  name_attach_t 		  *attach;
  struct _pulse           message;



  //   Create Channel
  if((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
  {
    //if there was an error creating the channel
    perror("ACC name_attach():");
    exit(EXIT_FAILURE);
  }

//  sleep(1);

//  sim_coid = name_open(SIMULATOR_NAME, 0);
//  reply = MsgSendPulsePtr(sim_coid, -1, ACC_CODE, 4);

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
		printf("Simulator*** Client is gone\n");
		ConnectDetach(message.scoid);
		break;
	  }
	  case SIMULATOR:
	  {
		AccMessageInput *data = (AccMessageInput *)message.value.sival_ptr;
		printf("****ACC: Data received:  brakes=%u, gas=%u, speed=%.2f, set_speed=%.2f, distance=%.2f\n",
			  data->brake_level, data->throttle_level, data->current_speed, data->desired_speed, data->distance);
		free(data);
		break;
	  }
	  default:
		printf("ACC: code value unknown=%d\n", message.code);
	  }
  }

//  name_close(sim_coid);
  name_detach(attach, 0);

  return NULL;
}

void *mocked_abs()
{
  int sim_coid;
  printf("Testing abs replies...\n");
  sleep(1);
  sim_coid = name_open(SIMULATOR_NAME, 0);
//  int reply_brakes = MsgSendPulse(sim_coid, -1, ABS_CODE, 25);
//  printf("abs reply: %d\n", reply_brakes);
//  reply_brakes = MsgSendPulse(sim_coid, -1, ABS_CODE, -20);
//  printf("abs reply: %d\n", reply_brakes);

  name_close(sim_coid);
  return NULL;
}

void *test_sim()
{
  init();
  return NULL;
}

void *distance_test()
{
  Environment    env;
  Sensors        sen;
  OutsideObject  obj;

  int                coid_acc;
  pthread_t          distance_simulator, mocked_acc_thread;
  simulatorRequest_t acc_request;

  init_env( &sen, &obj, &env );

  pthread_mutex_init(&env.mutex, NULL);

  // Create a thread for ACC server simulation
  pthread_create(&mocked_acc_thread, NULL, mocked_acc, NULL);
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
	if(i > 10){
	  env.object = FALSE;
	  break;
	}i++;
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
	  if(i > 10){
		  env.object = FALSE;
		  break;
	  }i++;
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
 	  if(i > 20){
 		  env.object = FALSE;
 		  break;
 	  }i++;
   }
   printf("Thread stopped calculating the distance\n");

  // Destroy mutex
  pthread_join(distance_simulator, NULL);
  pthread_mutex_destroy(&env.mutex);
  return NULL;
}
void *skid_test()
{
  Environment    env;
  Sensors        sen;
  OutsideObject  obj;
  simulatorRequest_t abs_request;
  pthread_t          skid_simulator, mocked_abs_server_thread;

  init_env( &sen, &obj, &env );

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
  pthread_setschedprio( skid_simulator , param.sched_priority + 2 );
  printf("Priority of the simulator = %d ; priority of skid_stop = %d\n",
		  param.sched_priority, param.sched_priority + 2);
  pthread_create(&skid_simulator, NULL, simulate_skid_stop, (void *)&abs_request);

  printf("Car skid before is %u\n", env.skid);

  env.skid = 1;
  printf("Car skid is set to %u by user\n", env.skid);
  printf("Car skid should be 0 = %u\n", env.skid);

  pthread_mutex_lock(&env.mutex);
  while (env.skid != 0)
  {
	pthread_cond_wait(&env.cond, &env.mutex);
  }
  env.skid = 1;
  pthread_cond_broadcast(&env.cond);
  pthread_mutex_unlock(&env.mutex);

  printf("Car skid  set to %u by user\n", env.skid);
  printf("Car skid should be 0 = %u\n", env.skid);

  pthread_mutex_lock(&env.mutex);
  while (env.skid != 0)
  {
	pthread_cond_wait(&env.cond, &env.mutex);
  }
  env.skid = 1;
  pthread_cond_broadcast(&env.cond);
  pthread_mutex_unlock(&env.mutex);

  printf("Car skid  set to %u by user\n", env.skid);
  printf("Car skid should be 0 = %u\n", env.skid);

  name_close(coid_abs);
  pthread_join(skid_simulator, NULL);
  pthread_join(mocked_abs_server_thread, NULL);
  pthread_mutex_destroy(&env.mutex);
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
