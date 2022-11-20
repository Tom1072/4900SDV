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

  printf("ACC attached\n");
  if ((attach = name_attach(NULL, ACC_NAME, 0)) == NULL)
    pthread_exit(NULL);

  // Create the child processor thread
  processed_input = malloc(sizeof(AccMessageInput));
  pthread_create(&processor_thread, NULL, acc_processor, processed_input);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
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

  while (1)
  {
    pthread_mutex_lock(&mutex);

    while (state != ACC_STATE)
      pthread_cond_wait(&cond, &mutex);

    // When distance > slow threshold, no object ahead
    if (data->distance > ACC_SLOW_THRESHOLD)
    {
      // Current speed is already >= desired speed, nothing needed to be done
      if (data->current_speed == data->desired_speed)
      {
        printf("Current speed == desired speed & no object in front.\nNothing to be done.");
      }
      else
      {
        // Otherwise, engage throttle or slow down if current > desired speed
        printf("ACC: engaging GAS and calculating new speed\n");
      }
    }
    // TODO Physics: handle thresholds dynamically based on distance and rate of dist decrease
    // If distance in the middle of the two thresholds, slowing down by disengage throttle
    else if (data->distance <= ACC_SLOW_THRESHOLD && data->distance > ACC_STOP_THRESHOLD)
    {
      printf("ACC: STOP engaging GAS and calculating REDUCED speed\n");
    }
    else // Otherwise (<= stop threshold), engage the brake until out of situation
    {
      printf("ACC: engaging brake to STOP\n");
    }
    usleep(200 * 1000);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}
