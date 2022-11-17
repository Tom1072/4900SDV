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

char test_simulator() {
  printf("Testing simulator...\n");
  pthread_t mocked_comm_thread, test_sim_thread, mocked_acc_thread, mocked_abs_thread;
  pthread_attr_t mocked_comm_attr, test_sim_attr, mocked_acc_attr, mocked_abs_attr;

  create_thread(&test_sim_thread, &test_sim_attr, 1, NULL, test_sim);
  create_thread(&mocked_comm_thread, &mocked_comm_attr, 2, NULL, mocked_comm);
  create_thread(&mocked_acc_thread, &mocked_acc_attr, 2, NULL, mocked_acc);
  create_thread(&mocked_abs_thread, &mocked_abs_attr, 2, NULL, mocked_abs);

  pthread_join(test_sim_thread, NULL);
  pthread_join(mocked_comm_thread, NULL);
  pthread_join(mocked_acc_thread, NULL);
  pthread_join(mocked_abs_thread, NULL);
  return TRUE;
}

void *mocked_comm()
{

  int sim_coid;
  sleep(1);
  printf("Testing communication...\n");
  // Environment* env = (Environment*) malloc(sizeof(Environment));;
  // env->skid        = 15;
  // env->distance    = 200;
  // env->car_speed   = 100;
  // env->brake_level = 0;
  // env->obj_speed   = 100;
  // env->object      = FALSE;
  CommListenerMessage* msg = (CommListenerMessage*) malloc(sizeof(CommListenerMessage));

  msg->command = SPAWN_CAR;
  msg->data.spawn_car_data.distance = 10;
  msg->data.spawn_car_data.obj_speed = 20;

  sim_coid = name_open(SIMULATOR_NAME, 0);
  MsgSendPulsePtr(sim_coid, -1, 100, (void *)msg);

  name_close(sim_coid);
  return NULL;
}

void *mocked_acc()
{
  int sim_coid;
  printf("Testing acc replies...\n");
  sleep(2);
  sim_coid = name_open(SIMULATOR_NAME, 0);
  int reply_throttle = MsgSendPulse(sim_coid, -1, THROTTLE_ACTUATOR, 4);
  printf("acc reply: %d\n", reply_throttle);
  reply_throttle = MsgSendPulse(sim_coid, -1, THROTTLE_ACTUATOR, -3);
  printf("acc reply: %d\n", reply_throttle);

  name_close(sim_coid);
  return NULL;
}
void *mocked_abs()
{
  int sim_coid;
  printf("Testing abs replies...\n");
  sleep(2);
  sim_coid = name_open(SIMULATOR_NAME, 0);
  int reply_brakes = MsgSendPulse(sim_coid, -1, BRAKE_ACTUATOR, 25);
  printf("abs reply: %d\n", reply_brakes);
  reply_brakes = MsgSendPulse(sim_coid, -1, BRAKE_ACTUATOR, -20);
  printf("abs reply: %d\n", reply_brakes);

  name_close(sim_coid);
  return NULL;
}
void *test_sim()
{
  init();
  return NULL;
}
