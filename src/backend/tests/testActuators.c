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
void mock_manual(int man_coid, int process_time, int throttle_level, int brake_level);
void mock_acc(int acc_coid, int process_time, double distance, double desired_speed, double current_speed);
void mock_abs(int abs_coid, int process_time, char skidding);

char test_actuators()
{
  printf("Testing actuators...\n");
  pthread_t ABS_thread, ACC_thread, manual_driver_thread;
  pthread_attr_t ABS_attr, ACC_attr, manual_driver_attr;

  pthread_t test_sim_thread;
  pthread_attr_t test_sim_attr;

  create_thread(&manual_driver_thread, &manual_driver_attr, 10, NULL, ManualDriver);
  create_thread(&ACC_thread, &ACC_attr, 10, NULL, ACC);
  create_thread(&ABS_thread, &ABS_attr, 10, NULL, ABS);
  create_thread(&test_sim_thread, &test_sim_attr, 10, NULL, mocked_simulator_actuator_test);

  pthread_join(manual_driver_thread, NULL);
  pthread_join(ACC_thread, NULL);
  pthread_join(ABS_thread, NULL);
  pthread_join(test_sim_thread, NULL);
  return TRUE;
}

void *mocked_simulator_actuator_test()
{
  printf("mock sim created\n");
  sleep(1);
  int man_coid = name_open(MANUAL_NAME, 0);
  // int acc_coid = name_open(ACC_NAME, 0);
  int abs_coid = name_open(ABS_NAME, 0);
  sleep(1);

  /** Demo for ACC only:
   *              coid, t, thr,  br   */
  // mock_acc(acc_coid, 5, 100, 100, 60); // current speed < desired & no object ahead

  /** Demo for Manual Driver only:
   *              coid, t, thr,  br   */
  // mock_manual(man_coid, 2, 100,   0);
  // mock_manual(man_coid, 6,   0,   0);
  // mock_manual(man_coid, 2, 100,   0);
  // mock_manual(man_coid, 3,   0,   1);
  // mock_manual(man_coid, 2, 100,   0);
  // mock_manual(man_coid, 2,   0,  20);
  // mock_manual(man_coid, 2, 100,   0);
  // mock_manual(man_coid, 1,   0,  50);
  // mock_manual(man_coid, 2, 100,   0);
  // mock_manual(man_coid, 1,   0,  70);
  // mock_manual(man_coid, 2, 100,   0);
  // mock_manual(man_coid, 0,   0, 100);

  /* Demo for ABS */
  mock_manual(man_coid, 2, 100, 0);
  mock_manual(man_coid, 0, 0, 50);
  mock_abs(abs_coid, 2, TRUE);
  mock_manual(man_coid, 2, 20, 0);
  mock_abs(abs_coid, 0, FALSE);

  return NULL;
}

/**
 * Send a mock message to Manual Driver controller
 * @param man_coid The coid of the Manual Controller (pulse listener)
 * @param process_time The time we want this process to last (in sec)
 * @param throttle_level Throttle level (in range [0, 100])
 * @param brake_level Brake level (in range [0, 100])
*/
void mock_manual(
    int man_coid,
    int process_time,
    int throttle_level,
    int brake_level)
{
  printf("MAN: Throttle = %d | Brake = %d\n", throttle_level, brake_level);
  ManMessageInput *man_input = (ManMessageInput *)malloc(sizeof(ManMessageInput));
  man_input->throttle_level = throttle_level;
  man_input->brake_level = brake_level;
  MsgSendPulsePtr(man_coid, MANUAL_PRIO, SIMULATOR, (void *)man_input);
  sleep(process_time);
}

/**
 * Send a mock message to ACC controller
 * @param acc_coid The coid of the ACC Controller (pulse listener)
 * @param process_time The time we want this process to last (in sec)
 * @param distance Distance from our car to the car ahead
 * @param desired_speed Desired speed of the car for ACC (in range [0, 100])
 * @param current_speed Current speed of the car (in range [0, 100])
*/
void mock_acc(
    int acc_coid,
    int process_time,
    double distance,
    double desired_speed,
    double current_speed)
{
  printf("ACC: Distance = %d | Desired V = %d | Current V = %d\n", distance, desired_speed, current_speed);
  AccMessageInput *acc_input = (AccMessageInput *)malloc(sizeof(AccMessageInput));
  acc_input->distance = distance;
  acc_input->desired_speed = desired_speed;
  acc_input->current_speed = current_speed;
  MsgSendPulsePtr(acc_coid, ACC_PRIO, SIMULATOR, (void *)acc_input);
  sleep(process_time);
}

/**
 * Send a mock message to ABS controller
 * @param acc_coid The coid of the ACC Controller (pulse listener)
 * @param process_time The time we want this process to last (in sec)
 * @param skidding Flag that indicates if car is skidding or not (TRUE or FALSE)
*/
void mock_abs(
    int abs_coid,
    int process_time,
    char skidding)
{
  printf("ABS: Skidding = %s\n", skidding == TRUE ? "TRUE" : "FALSE");
  AbsMessageInput *abs_input = (AbsMessageInput*) malloc(sizeof(AbsMessageInput));
  abs_input->skid = skidding;
  MsgSendPulsePtr(abs_coid, ABS_PRIO, SIMULATOR, (void *)abs_input);
  sleep(process_time);
}