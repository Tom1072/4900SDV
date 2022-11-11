#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../includes/tests.h"
#include "../includes/commons.h"
#include "../includes/utils.h"
#include "../includes/actuators.h"

void *test_simulator();

char testActuators()
{
  printf("Testing actuators...\n");
  pthread_t ABS_thread, ACC_thread, manual_driver_thread;
  pthread_attr_t ABS_attr, ACC_attr, manual_driver_attr;

  pthread_t test_sim_thread;
  pthread_attr_t test_sim_attr;

  create_thread(&manual_driver_thread, &manual_driver_attr, 2, ManualDriver);
  create_thread(&ACC_thread, &ACC_attr, 1, ACC);
  create_thread(&ABS_thread, &ABS_attr, 3, ABS);
  create_thread(&test_sim_thread, &test_sim_attr, 4, test_simulator);

  pthread_join(manual_driver_thread, NULL);
  pthread_join(ACC_thread, NULL);
  pthread_join(ABS_thread, NULL);
  return TRUE;
}

void *test_simulator()
{
  int manual_coid, ACC_coid, ABS_coid;

  manual_coid = name_open(MANUAL_NAME, 0);
  ACC_coid = name_open(ABS_NAME, 0);
  ABS_coid = name_open(ACC_NAME, 0);

  sleep(2);
  printf("Sending manual pulse\n");
  MsgSendPulse(manual_coid, -1, 0, 1);
  sleep(2);
  printf("Sending ABS pulse\n");
  MsgSendPulse(ABS_coid, -1, 0, 2);
  sleep(2);
  printf("Sending ACC pulse\n");
  MsgSendPulse(ACC_coid, -1, 0, 3);
  return NULL;
}