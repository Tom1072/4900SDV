#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
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
  int coid, ACC_coid, ABS_coid;
  ActuatorInputPayload *message = (ActuatorInputPayload *)malloc(sizeof(ActuatorInputPayload));
  memset(message, 0, sizeof(ActuatorInputPayload));
  ActuatorInputPayload *ACC_message = (ActuatorInputPayload *)malloc(sizeof(ActuatorInputPayload));
  memset(ACC_message, 0, sizeof(ActuatorInputPayload));
  ActuatorInputPayload *ABS_message = (ActuatorInputPayload *)malloc(sizeof(ActuatorInputPayload));
  memset(ABS_message, 0, sizeof(ActuatorInputPayload));

  message->brake_level = 50;
  message->gas_level = 78;

  ACC_message->distance = 100;
  ACC_message->desired_speed = 60;
  ACC_message->current_speed = 40;

  ABS_message->skidding = 1;
  ABS_message->brake_level = 80;


  printf("Testing sending messages:\n");
  sleep(2);
  printf("Sending manual pulse\n");
  coid = name_open(MANUAL_NAME, 0);
  MsgSendPulsePtr(coid, -1, MANUAL_DRIVER_CODE, (void *)message);
  sleep(2);
  printf("Sending ACC pulse\n");
  ACC_coid = name_open(ACC_NAME, 0);
  MsgSendPulsePtr(ACC_coid, -1, MANUAL_DRIVER_CODE, (void *)ACC_message);
  sleep(2);
  printf("Sending ABS pulse\n");
  ABS_coid = name_open(ABS_NAME, 0);
  MsgSendPulsePtr(ABS_coid, -1, MANUAL_DRIVER_CODE, (void *)ABS_message);

  return NULL;
}