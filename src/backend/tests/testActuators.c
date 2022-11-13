#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../includes/tests.h"
#include "../includes/commons.h"
#include "../includes/utils.h"
#include "../includes/actuators.h"

void *mocked_simulator_actuator_test();

char test_actuators()
{
  printf("Testing actuators...\n");
  pthread_t ABS_thread, ACC_thread, manual_driver_thread;
  pthread_attr_t ABS_attr, ACC_attr, manual_driver_attr;

  pthread_t test_sim_thread;
  pthread_attr_t test_sim_attr;

  create_thread(&manual_driver_thread, &manual_driver_attr, 2, ManualDriver);
  create_thread(&ACC_thread, &ACC_attr, 1, ACC);
  create_thread(&ABS_thread, &ABS_attr, 3, ABS);
  create_thread(&test_sim_thread, &test_sim_attr, 4, mocked_simulator_actuator_test);

  pthread_join(manual_driver_thread, NULL);
  pthread_join(ACC_thread, NULL);
  pthread_join(ABS_thread, NULL);
  return TRUE;
}

void *mocked_simulator_actuator_test()
{
  printf("Testing sending messages:\n");
  sleep(2);
  printf("Sending manual pulse\n");
  send_pulse_msg(MANUAL_NAME, "Hello MANUAL DRIVER!", 0);
  sleep(2);
  printf("Sending ABS pulse\n");
  send_pulse_msg(ACC_NAME, "Hello ACC!", 0);
  sleep(2);
  printf("Sending ACC pulse\n");
  send_pulse_msg(ABS_NAME, "Hello ABS!", 0);

  // int manual_coid, ACC_coid, ABS_coid;

  // manual_coid = name_open(MANUAL_NAME, 0);
  // ACC_coid = name_open(ABS_NAME, 0);
  // ABS_coid = name_open(ACC_NAME, 0);
  // char *manual_buffer = "Hello Manual Driver!";
  // char *ACC_buffer = "Hello ACC!";
  // char *ABS_buffer = "Hello ABS!";

  // char *manual_msg = malloc(sizeof(manual_buffer));
  // char *ACC_msg = malloc(sizeof(ACC_buffer));
  // char *ABS_msg = malloc(sizeof(ABS_buffer));

  // strcpy(manual_msg, manual_buffer)

  // sleep(2);
  // printf("Sending manual pulse\n");
  // MsgSendPulsePtr(manual_coid, -1, 0, 2);
  // sleep(2);
  // printf("Sending ABS pulse\n");
  // MsgSendPulsePtr(ABS_coid, -1, 0, 2);
  // sleep(2);
  // printf("Sending ACC pulse\n");
  // MsgSendPulsePtr(ACC_coid, -1, 0, 3);
  return NULL;
}