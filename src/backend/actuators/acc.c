#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <float.h>
#include <assert.h>
#include <string.h>

#include "../includes/actuators.h"
#include "../includes/commons.h"
#include "../includes/utils.h"

typedef enum
{
  DISTANCE_CONTROL = 0,
  SPEED_CONTROL
} ACCMode;

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


// long long current_timestamp()
// {
//   struct timeval te;
//   struct timeval te;
//   struct timeval te;
//   struct timeval te;
//   struct timeval te;
//   gettimeofday(&te, NULL);                                         // get current time
//   long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
//   // printf("milliseconds: %lld\n", milliseconds);
//   return milliseconds;
// }

/**
 * Handler of the ACC thread.
 */
void *ACC()
{
  usleep(1000000);
  name_attach_t *attach;
  struct _pulse pulse_msg;
  AccMessageInput *input;
  AccMessageInput process_input;
  pthread_t processor_thread;
  // long curr_timestamp;

  memset(&process_input, 0, sizeof(AccMessageInput));

  printf("ACC: attached\n");

  pthread_create(&processor_thread, NULL, acc_processor, &process_input);

  if ((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
    pthread_exit(NULL);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);

    printf("ACC: Got input from Simulator\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    pthread_mutex_lock(&mutex);
    input = (AccMessageInput *)pulse_msg.value.sival_ptr;
    // copy_acc_input_payload(input, processed_input);
    memcpy(&process_input, input, sizeof(AccMessageInput));

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

  return NULL;
}

/**
 * Handler function for ACC processor/sender that sends pulse to Simulator
 */

void *acc_processor(void *args)
{
  int sim_coid = name_open(SIMULATOR_NAME, 0);
  AccMessageInput *data = (AccMessageInput *)args;
  double desired_distance;
  double delta_distance; // distance change since last update
  double prev_distance = DBL_MAX;
  double relative_speed, lead_speed, time_to_lead;
  double desired_acceleration = 0;
  double speed_in_mps;
  int mode;

  while (1)
  {
    pthread_mutex_lock(&mutex);

    while (state != ACC_STATE)
      pthread_cond_wait(&cond, &mutex);
    // printf("ACC: Processing simulator message\n");

    speed_in_mps = speed / 3.6;

    // Print out the data
    // printf("ACC: distance: %lf, desired_speed: %lf, current_speed: %lf\n", data->distance, data->desired_speed, data->current_speed);

    // The difference (m) between the current distance recording and the previous one
    // delta_distance < 0 when distance is decreasing since data->distance < prev_distance
    // delta_distance > 0 when distance is increasing
    delta_distance = (prev_distance == DBL_MAX) ? 0 : data->distance - prev_distance;
    // printf("ACC: delta_distance: %lf\n", delta_distance);

    // The relative speed (m/s) between this car and the lead car
    // relative_speed > 0 when this car is moving faster than the lead car since delta_distance < 0
    // relative_speed < 0 when this car is moving slower than the lead car since delta_distance > 0
    relative_speed = -delta_distance / TIME_INTERVAL * 1000;
    // printf("ACC: relative_speed: %lf\n", relative_speed);

    // The speed of the lead car (m/s)
    // if this car speed is 50m/s, and it's moving 10m/s faster than the lead car
    // --> relative_speed > 0
    // --> the lead car speed is 50 - 10 = 40
    lead_speed = speed_in_mps - relative_speed;
    // printf("ACC: lead_speed: %lf\n", lead_speed);

    // Desired distance is the minimum distance that allow the car to stop in time
    desired_distance = ((-speed_in_mps / MAX_DEACCELERATION) * (speed_in_mps / 2)) + MIN_DISTANCE; // (m)
    // printf("ACC: desired_distance: %lf\n", desired_distance);

    // Print out every calculation so far

    if (data->distance < desired_distance)
    {
      // Current distance is less than desired distance, we need to keep the distance
      mode = DISTANCE_CONTROL;
    }
    else if (speed > data->desired_speed || data->distance >= desired_distance)
    {
      // We are in DISTANCE_CONTROL and car in front speed up too much
      // or when we are far enough from the lead car
      mode = SPEED_CONTROL;
    }

    // printf("ACC: mode: %s\n", mode == DISTANCE_CONTROL ? "DISTANCE_CONTROL" : "SPEED_CONTROL");

    if (mode == DISTANCE_CONTROL)
    {
      if (speed_in_mps > lead_speed)
      {
        assert(relative_speed > 0); // We are moving faster than the lead car

        time_to_lead = (data->distance - MIN_DISTANCE) / relative_speed;
        assert(time_to_lead > 0); // data->distance > MIN_DISTANCE

        desired_acceleration = (lead_speed - speed_in_mps) / time_to_lead;
        assert(desired_acceleration < 0); // we are moving faster than the lead car --> we need to slow down

        calculate_brake_and_throttle_levels(desired_acceleration);
        sendUpdates(sim_coid, brake_level, throttle_level, speed);
      }
    }
    else if (mode == SPEED_CONTROL)
    {
      if (abs(speed - data->desired_speed) >= (double)SPEED_ERROR_TOLERANCE)
      {
        if (speed > data->desired_speed)
        {
          // Slow down
          // printf("ACC: Slow down\n");
          desired_acceleration = SPEED_CONTROL_DEFAULT_DEACCELERATION;
        }
        else
        {
          // Speed up
          // printf("ACC: Speed up\n");
          desired_acceleration = SPEED_CONTROL_DEFAULT_ACCELERATION;
        }
        // printf("desired_acceleration: %lf\n", desired_acceleration);
        calculate_brake_and_throttle_levels(desired_acceleration);
        sendUpdates(sim_coid, brake_level, throttle_level, speed);
      }
    }

    prev_distance = data->distance;
    usleep(TIME_INTERVAL * 1000);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}
