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

void *test_comm();
void *test_sim();
void *test_acc();
void *test_abs();

char testSimulator() {
  printf("Testing simulator...\n");
  pthread_t test_comm_thread, test_sim_thread, test_acc_thread, test_abs_thread;
  pthread_attr_t test_comm_attr, test_sim_attr, test_acc_attr, test_abs_attr;

  create_thread(&test_sim_thread, &test_sim_attr, 1, test_sim);
  create_thread(&test_comm_thread, &test_comm_attr, 2, test_comm);
  create_thread(&test_acc_thread, &test_acc_attr, 2, test_acc);
  create_thread(&test_abs_thread, &test_abs_attr, 2, test_abs);

  pthread_join(test_sim_thread, NULL);
  pthread_join(test_comm_thread, NULL);
  pthread_join(test_acc_thread, NULL);
  pthread_join(test_abs_thread, NULL);
  return TRUE;
}

void *test_comm()
{

  int sim_coid;
  sleep(1);
  printf("Testing communication...\n");
  Environment* env = (Environment*) malloc(sizeof(Environment));;
  env->skid        = 15;
  env->distance    = 200;
  env->car_speed   = 100;
  env->brake_level = 0;
  env->obj_speed   = 100;
  env->object      = FALSE;

  sim_coid = name_open(SIMULATOR_NAME, 0);
  MsgSendPulsePtr(sim_coid, -1, 100, (void *)env);

  name_close(sim_coid);
  return NULL;
}

void *test_acc()
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
void *test_abs()
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
