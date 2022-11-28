#include <stdio.h>
#include <assert.h>
#include "backend/includes/tests.h"
#include "backend/includes/commons.h"
#include "backend/includes/simulator.h"
#include "backend/includes/CommListener.h"
#include "backend/includes/CommDispatcher.h"
#include "backend/includes/actuators.h"
#include "backend/includes/utils.h"
#include "backend/includes/View.h"

int main()
{
  // printf("Hello, World!\n");
//    char comm_result  = test_comm();
//   char simulator_result = test_simulator();
  // char view_result = test_view();
  char actuator_result = test_actuators();
  
//    assert(comm_result);
//   assert(simulator_result);
  // assert(view_result);
  assert(actuator_result);



  // Start all threads
  pthread_t ABS_thread, ACC_thread, manual_driver_thread, simulator_thread, comm_input_thread, comm_display_thread;
//  pthread_attr_t ABS_attr, ACC_attr, manual_driver_attr, simulator_attr, comm_input_attr, comm_display_attr;

//  create_thread(&simulator_thread, &simulator_attr, SIMULATOR_PRIO, NULL, init);
//  create_thread(&manual_driver_thread, &manual_driver_attr, MANUAL_PRIO, NULL, ManualDriver);
//  create_thread(&ACC_thread, &ACC_attr, ACC_PRIO, NULL, ACC);
//  create_thread(&ABS_thread, &ABS_attr, ABS_PRIO, NULL, ABS);
//  create_thread(&comm_display_thread, &comm_display_attr, COMM_PRIO, NULL, (void *)start_dispatcher);
//  create_thread(&comm_input_thread, &comm_input_attr, COMM_PRIO, NULL, (void *)start_listener);

  pthread_create(&simulator_thread, NULL, (void *)init, NULL);
  pthread_create(&manual_driver_thread, NULL, ManualDriver, NULL);
  pthread_create(&ACC_thread, NULL, ACC, NULL);
  pthread_create(&ABS_thread, NULL, ABS, NULL);
  pthread_create(&comm_display_thread, NULL, (void *)start_dispatcher, NULL);
  pthread_create(&comm_input_thread, NULL, (void *)start_listener, NULL);


  pthread_join(comm_input_thread, NULL);
  pthread_join(comm_display_thread, NULL);
  pthread_join(manual_driver_thread, NULL);
  pthread_join(ACC_thread, NULL);
  pthread_join(ABS_thread, NULL);
  pthread_join(simulator_thread, NULL);


  return 0;
}
