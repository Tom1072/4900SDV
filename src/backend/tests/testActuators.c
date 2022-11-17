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

  create_thread(&manual_driver_thread, &manual_driver_attr, 2, NULL, ManualDriver);
  create_thread(&ACC_thread, &ACC_attr, 1, NULL, ACC);
  create_thread(&ABS_thread, &ABS_attr, 3, NULL, ABS);
  create_thread(&test_sim_thread, &test_sim_attr, 4, NULL, mocked_simulator_actuator_test);

  pthread_join(manual_driver_thread, NULL);
  pthread_join(ACC_thread, NULL);
  pthread_join(ABS_thread, NULL);
  pthread_join(test_sim_thread, NULL);
  return TRUE;
}

void *mocked_simulator_actuator_test()
{
  printf("mock sim created\n");
  int man_coid, acc_coid, abs_coid;

  sleep(1);
  man_coid = name_open(MANUAL_NAME, 0);
  acc_coid = name_open(ACC_NAME, 0);
  abs_coid = name_open(ABS_NAME, 0);
  sleep(1);


  /** ABS preemption test */
  ManMessageInput* man_input;
  AbsMessageInput* abs_input;
  AccMessageInput* acc_input;

  // Enable ACC
  // acc_input = (AccMessageInput*) malloc(sizeof(AccMessageInput));
  // printf("Sending pulse to enable ACC\n");
  // MsgSendPulsePtr(acc_coid, ACC_PRIO, SIMULATOR, (void *)acc_input);

  // sleep(2);

  // Enable ACC: case Current speed < desired & no object ahead
  acc_input = (AccMessageInput*) malloc(sizeof(AccMessageInput));
  acc_input->distance = 1000;
  acc_input->desired_speed = 100;
  acc_input->desired_speed = 60;
  printf("Sending pulse to enable ACC\n");
  MsgSendPulsePtr(acc_coid, ACC_PRIO, SIMULATOR, (void *)acc_input);

  // // User step on the brake
  // sleep(2);
  // man_input = (ManMessageInput*) malloc(sizeof(ManMessageInput));
  // man_input->brake_level = 1;
  // printf("Sending pulse to ManualDriver: brake_level = 1\n");
  // MsgSendPulsePtr(man_coid, MANUAL_PRIO, SIMULATOR, (void *)man_input);

  // sleep(2);
  // man_input = (ManMessageInput*) malloc(sizeof(ManMessageInput));
  // man_input->throttle_level = 1;
  // man_input->brake_level = 0;
  // printf("Sending pulse to ManualDriver: brake_level = 1\n");
  // MsgSendPulsePtr(man_coid, MANUAL_PRIO, SIMULATOR, (void *)man_input);

  // sleep(2);
  // man_input = (ManMessageInput*) malloc(sizeof(ManMessageInput));
  // man_input->throttle_level = 0;
  // man_input->brake_level = 1;
  // printf("Sending pulse to ManualDriver: brake_level = 1\n");
  // MsgSendPulsePtr(man_coid, MANUAL_PRIO, SIMULATOR, (void *)man_input);

  // // Skid happens
  // sleep(2);
  // abs_input = (AbsMessageInput*) malloc(sizeof(AbsMessageInput));
  // abs_input->skid = TRUE;
  // printf("Sending pulse to ABS: skid=TRUE\n");
  // MsgSendPulsePtr(abs_coid, ABS_PRIO, SIMULATOR, (void *)abs_input);

  // // skid off
  // sleep(2);
  // abs_input = (AbsMessageInput*) malloc(sizeof(AbsMessageInput));
  // abs_input->skid = FALSE;
  // printf("Sending pulse to ABS: skid=TRUE\n");
  // MsgSendPulsePtr(abs_coid, ABS_PRIO, SIMULATOR, (void *)abs_input);

  return NULL;
}