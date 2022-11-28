#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
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

/**
 * Handler of the ABS thread.
 */
void *ABS()
{
  name_attach_t *attach;
  struct _pulse pulse_msg;
  AbsMessageInput *input, *processed_input;
  volatile char skidding = FALSE;
  pthread_t processor_thread; 
  if ((attach = name_attach(NULL, ABS_NAME, 0)) == NULL)
    pthread_exit(NULL);

  // Create the child processor thread
  processed_input = malloc(sizeof(ManMessageInput));
  pthread_create(&processor_thread, NULL, abs_processor, processed_input);

  while (1)
  {
    MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
    printf("ABS got input\n");

    if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
    {
      ConnectDetach(pulse_msg.scoid);
      continue;
    }

    pthread_mutex_lock(&mutex);
    input = (AbsMessageInput *)pulse_msg.value.sival_ptr;
    copy_abs_input_payload(input, processed_input);

    printf("Skidding: %d, input skidding: %d\n", skidding, input->skid);
    if (!skidding && input->skid) // If currently not skidding, and receive skidding input
    {
      skidding = TRUE;
      abs_processing = TRUE;
    }
    else if (skidding && !input->skid) // If currently skidding, and receive not skidding input
    {
      skidding = FALSE;
      abs_processing = FALSE;
    }

    set_state(); // IMPORTANT: set determine the next state machine to run
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    free(input);
  }

  free(processed_input);
  return NULL;
}
/**
 * Handler function for ABS processor/sender that sends pulse to Simulator
*/
void *abs_processor(void *args)
{
  usleep(100000);
  int sent_brake_level = 0;
  int sim_coid = name_open(SIMULATOR_NAME, 0);
  // AbsMessageInput *data = args;

  while (1)
  {
    pthread_mutex_lock(&mutex);

    while (state != ABS_STATE)
      pthread_cond_wait(&cond, &mutex);

    sent_brake_level = sent_brake_level == 0 ? brake_level : 0;
    speed = calculate_speed(speed, sent_brake_level, throttle_level);
    sendUpdates(sim_coid, sent_brake_level, throttle_level, speed);
    usleep(100 * 1000);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}
