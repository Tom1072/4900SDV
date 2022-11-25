#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "../includes/actuators.h"
#include "../includes/commons.h"
#include "../includes/utils.h"

#define DISTANCE_CONTROL 0
#define SPEED_CONTROL 1
#define MIN_DISTANCE 0
#define SPEED_ERROR_TOLERANCE 5
#define DIST_INF 0 // TODO: get this variable from Kate's change

/**
 * current controller
 */
volatile extern int state;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

volatile extern unsigned short brake_level;
volatile extern unsigned short throttle_level;
volatile extern double speed;

volatile extern char abs_processing;
volatile extern char acc_processing;

volatile long last_update_timestamp = 0; // milliseconds
volatile long delta_time = 0; // milliseconds

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

/**
 * Handler of the ACC thread.
 */
void *ACC()
{
  name_attach_t *attach;
  struct _pulse pulse_msg;
  AccMessageInput *input, *processed_input;
  pthread_t processor_thread;
  long curr_timestamp;

  printf("ACC attached\n");
  if ((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
    pthread_exit(NULL);

  // Create the child processor thread
  processed_input = malloc(sizeof(AccMessageInput));
  pthread_create(&processor_thread, NULL, acc_processor, processed_input);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    curr_timestamp = current_timestamp();
    delta_time = last_update_timestamp == 0 ? 100 : curr_timestamp - last_update_timestamp;
    last_update_timestamp = curr_timestamp;
    printf("ACC got input\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    pthread_mutex_lock(&mutex);
    input = (AccMessageInput *)pulse_msg.value.sival_ptr;
    copy_acc_input_payload(input, processed_input);

    // If desired_speed == 0, ACC is turned off
    if (input->desired_speed == 0)
      acc_processing = FALSE;
    else
      acc_processing = TRUE;

    set_state(); // IMPORTANT: set determine the next state machine to run

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    free(input);
  }

  free(processed_input);
  return NULL;
}
/**
 * Handler function for ACC processor/sender that sends pulse to Simulator
*/
void *acc_processor(void *args)
{
  usleep(100000);
  int sim_coid = name_open(SIMULATOR_NAME, 0);
  AccMessageInput *data = args;
  double desired_distance = 20;
  double delta_distance; // distance change since last update
  double prev_distance = 5; // TODO: check this
  double relative_speed, lead_speed, time_to_lead;
  double desired_acceleration=0, desired_speed_change=0;
  int mode;

  while (1)
  {
    pthread_mutex_lock(&mutex);

    while (state != ACC_STATE)
      pthread_cond_wait(&cond, &mutex);

    delta_distance = (prev_distance == DIST_INF) ? 0 : data->distance - prev_distance;
    relative_speed = -delta_distance / delta_time; 
    lead_speed = speed - relative_speed;

    if (data->distance < desired_distance)
      mode = DISTANCE_CONTROL;
    else if (speed > data->desired_speed || data->distance >= desired_distance)
      mode = SPEED_CONTROL;

    if (mode == DISTANCE_CONTROL) {
      if (speed > lead_speed) {
        time_to_lead = (MIN_DISTANCE - data->distance) / relative_speed;
        desired_acceleration = (lead_speed - speed) / time_to_lead;
        desired_speed_change = desired_acceleration; // TODO: Calculate desired speed change
        calculate_brake_and_throttle_levels(desired_speed_change);
        sendUpdates(sim_coid, brake_level, throttle_level, speed);
      }
    } else if (mode == SPEED_CONTROL) {
      if (abs(speed - data->desired_speed) >= SPEED_ERROR_TOLERANCE) {
        desired_speed_change = desired_acceleration; // TODO: Calculate desired speed change
        calculate_brake_and_throttle_levels(desired_speed_change);
        sendUpdates(sim_coid, brake_level, throttle_level, speed);
      }
    }

    prev_distance = data->distance;

    usleep(200 * 1000);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

  // while (1)
  // {
  //   pthread_mutex_lock(&mutex);

  //   while (state != ACC_STATE)
  //     pthread_cond_wait(&cond, &mutex);

  //   // When distance > slow threshold, no object ahead
  //   if (data->distance > ACC_SLOW_THRESHOLD)
  //   {
  //     // Current speed is already >= desired speed, nothing needed to be done
  //     if (data->current_speed == data->desired_speed)
  //     {
  //       printf("Current speed == desired speed & no object in front.\nNothing to be done.");
  //     }
  //     else
  //     {
  //       // Otherwise, engage throttle or slow down if current > desired speed
  //       printf("ACC: engaging GAS and calculating new speed\n");
  //     }
  //   }
  //   // TODO Physics: handle thresholds dynamically based on distance and rate of dist decrease
  //   // If distance in the middle of the two thresholds, slowing down by disengage throttle
  //   else if (data->distance <= ACC_SLOW_THRESHOLD && data->distance > ACC_STOP_THRESHOLD)
  //   {
  //     printf("ACC: STOP engaging GAS and calculating REDUCED speed\n");
  //   }
  //   else // Otherwise (<= stop threshold), engage the brake until out of situation
  //   {
  //     printf("ACC: engaging brake to STOP\n");
  //   }
  //   usleep(200 * 1000);
  //   pthread_mutex_unlock(&mutex);
  // }