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
  pthread_t      mocked_comm_thread, test_sim_thread,
  	  	  	     mocked_acc_thread, mocked_abs_thread,
			     mocked_dist_thread, mocked_skid_thread;
  pthread_attr_t mocked_comm_attr, test_sim_attr,
  	  	  	  	 mocked_acc_attr, mocked_abs_attr,
				 mocked_dist_attr, mocked_skid_attr;

//  create_thread(&test_sim_thread, &test_sim_attr, 1, test_sim);
//  create_thread(&mocked_comm_thread, &mocked_comm_attr, 2, mocked_comm);
//  create_thread(&mocked_acc_thread, &mocked_acc_attr, 2, mocked_acc);
//  create_thread(&mocked_abs_thread, &mocked_abs_attr, 2, mocked_abs);
//  create_thread(&mocked_dist_thread, &mocked_dist_attr, 2, distance_test);
  create_thread(&mocked_skid_thread, &mocked_skid_attr, 2, skid_test);

//  pthread_join(test_sim_thread, NULL);
//  pthread_join(mocked_comm_thread, NULL);
//  pthread_join(mocked_acc_thread, NULL);
//  pthread_join(mocked_abs_thread, NULL);
//  pthread_join(mocked_dist_thread, NULL);
  pthread_join(mocked_skid_thread, NULL);

  return TRUE;
}

void *mocked_comm()
{

  printf("Testing communication...\n");
  int sim_coid;
  name_attach_t 		  *attach;
  struct _pulse           message;
  int                     coid_sim;
  int                     rcvid;
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
  int reply_comm = MsgSendPulsePtr(sim_coid, -1, COMM, (void *)msg);
  printf("comm reply: %d\n", reply_comm);

//---------------------------------------------------------------
  //------------- Server side ------------------------------------
  rcvid = MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL);
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
  int sim_coid;
  printf("Testing acc replies...\n");
  name_attach_t 		  *attach;
  struct _pulse           message;
  int                     coid_acc;
  int                     rcvid;
  int                     reply_throttle;
  // Create Channel
//  if((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
//  {
//    //if there was an error creating the channel
//    perror("ACC name_attach():");
//    exit(EXIT_FAILURE);
//  }

  sleep(1);

  sim_coid = name_open(SIMULATOR_NAME, 0);

  reply_throttle = MsgSendPulse(sim_coid, -1, ACC_CODE, 4);
  printf("acc reply: %d\n", reply_throttle);

  /*
  rcvid = MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL);
  switch(message.code)
  {
  case _PULSE_CODE_DISCONNECT:
    printf("Simulator*** Client is gone\n");
    ConnectDetach(message.scoid);
    break;
  case SIMULATOR:
	  Sensors *data = (Sensors *)message.value.sival_ptr;
	  if( (data->throttle_level > 1) && (data->distance > 50) ){
		  reply_throttle = MsgSendPulse(sim_coid, -1, ACC_CODE, 4);
		  printf("acc reply: %d\n", reply_throttle);
	  } else {
		  reply_throttle = MsgSendPulse(sim_coid, -1, ACC_CODE, -3);
		  printf("acc reply: %d\n", reply_throttle);
	  } free(data);
	  break;
  default:
	  printf("ACC: code value=%d\n", message.code);
  }*/
  printf("ACC: Message code received: %d\n", message.value.sival_ptr);

  name_close(sim_coid);
//  name_detach(attach, 0);

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
  OutsideObject obj;
  init_env( &sen, &obj, &env );
  pthread_t               distance_simulator;
  pthread_mutex_init(&env.mutex, NULL);
  // Create a thread for distance simulation
  pthread_create(&distance_simulator, NULL, simulate_distance, (void *)&env);

  // -----------------------------------------------------------
  // Test Approaching car -------------------------------------

  env.obj_speed = 1;
  env.car_speed = 240;
  env.object    = TRUE;
  env.distance  = 3;
  printf("Distance in meters initial: %d, obj = %d\n", env.distance, env.object);
  while(env.distance > -1)
  {
	  printf("Distance in meters: %lf\n", env.distance);
	  usleep(100000);
	  if(env.distance == 0){
		  env.object = FALSE;
		  break;
	  }

  }

  printf("Thread stopped calculating the distance\n");

  // -----------------------------------------------------------
  // Test disappearing car in front-----------------------------

  int i = 0;
  env.obj_speed = 60;
  env.car_speed = 70;
  env.object    = TRUE;
  env.distance  = 100;

  while(env.distance > -1)
  {
	  printf("Distance in meters: %d\n", env.distance);
	  usleep(100000);
	  if(i > 30){
		  env.object = FALSE;
		  break;
	  }i++;
  }
  printf("Thread stopped calculating the distance\n");

  // -----------------------------------------------------------
  // Test car in front same speed  -----------------------------

  i = 0;
  env.obj_speed = 70;
  env.car_speed = 70;
  env.object    = TRUE;
  env.distance  = 100;

  while(env.distance > -1)
  {
	  printf("Distance in meters: %d\n", env.distance);
	  usleep(100000);
	  if(i > 30){
		  env.object = FALSE;
		  break;
	  }i++;
  }
  printf("Thread stopped calculating the distance\n");

  // -----------------------------------------------------------
   // Test car in front accelerate -----------------------------

   i = 0;
   env.obj_speed = 90;
   env.car_speed = 40;
   env.object    = TRUE;
   env.distance  = 100;

   while(env.distance > -1)
   {
 	  printf("Distance in meters: %d\n", env.distance);
 	  usleep(100000);
 	  if(i > 40){
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
  OutsideObject obj;
  init_env( &sen, &obj, &env );
  simulatorRequest_t abs_request;

  pthread_t  skid_simulator, mocked_abs_server_thread;
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

  printf("Priority of the simulator = %d ; priority of skid_stop = %d\n", param.sched_priority, param.sched_priority + 2);
  pthread_create(&skid_simulator, NULL, simulate_skid_stop, (void *)&abs_request);

  env.skid = 0;

  printf("Car skid before is %u\n", env.skid);

  env.skid = 1;


  printf("Car skid is set to %u by user\n", env.skid);

  usleep(10);
  printf("Car skid should be 0 = %u\n", env.skid);
  env.skid = 1;
  usleep(10);
  printf("Car skid  set to %u by user\n", env.skid);
//  sleep(1);
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
  int sim_coid;
  name_attach_t 		  *attach;
  struct _pulse           message;
  int                     coid_sim;
  int                     rcvid;
  // Create Channel
  if((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
  {
	//if there was an error creating the channel
	perror("ABS name_attach():");
	exit(EXIT_FAILURE);
  }
    while(1)
    {
	  rcvid = MsgReceivePulse(attach->chid, (void *) &message, sizeof(message), NULL);
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
