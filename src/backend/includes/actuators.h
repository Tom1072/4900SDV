#pragma once

#include <pthread.h>

#include "../includes/commons.h"

#define ACC_SLOW_THRESHOLD 4 // meters
#define ACC_STOP_THRESHOLD 2 // meters

typedef enum {
  NOT_ACQUIRED = 0,
  MANUAL_DRIVER_STATE = 2,
  ACC_STATE = 1,
  ABS_STATE = 3,
} ControllerState;

void *ManualDriver();
void *ACC();
void *ABS();
void sendUpdates(int sim_coid, unsigned short brake_level, unsigned short throttle_level, double speed);
void set_state();
void copy_man_input_payload(ManMessageInput *input, ManMessageInput *copied);
void copy_acc_input_payload(AccMessageInput *input, AccMessageInput *copied);
void copy_abs_input_payload(AbsMessageInput *input, AbsMessageInput *copied);
void *man_processor(void *args);
void *acc_processor(void *args);
void *abs_processor(void *args);
double calculate_speed(double speed, int brake_level, int throttle_level);

