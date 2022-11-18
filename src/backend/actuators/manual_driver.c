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

volatile extern char man_processing;
volatile extern char abs_processing;
volatile extern char acc_processing;

/**
 * Handler of the ManualDriver thread.
 */
void *ManualDriver()
{
    name_attach_t *attach;
    struct _pulse pulse_msg;
    ManMessageInput *input, *processed_input;
    pthread_t processor_thread;

    printf("Manual Driver attached\n");
    if ((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
        pthread_exit(NULL);

    // Create the child processor thread
    processed_input = malloc(sizeof(ManMessageInput));
    pthread_create(&processor_thread, NULL, man_processor, processed_input);

    while (1)
    {
        MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
        printf("Manual got input\n");

        if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
        {
            ConnectDetach(pulse_msg.scoid);
            continue;
        }

        pthread_mutex_lock(&mutex);
        input = (ManMessageInput *)pulse_msg.value.sival_ptr;

        // If input is valid (not both are engaged)
        if (!(input->brake_level > 0 && input->throttle_level > 0))
        {
            copy_man_input_payload(input, processed_input);

            // If both are 0 -> no manual input, no need to process, and vice versa.
            if (input->brake_level == 0 && input->throttle_level == 0)
                man_processing = FALSE;
            else
                man_processing = TRUE;

            // brake_level = input->brake_level;
            set_state(); // IMPORTANT: set determine the next state machine to run
        }

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        free(input);
    }

    free(processed_input);
    return NULL;
}

/**
 * Handler function for manual driver processor/sender that sends pulse to Simulator
 */
void *man_processor(void *args)
{
    usleep(100000);
    int sim_coid = name_open(SIMULATOR_NAME, 0);
    ManMessageInput *data = args;

    while (1)
    {
        pthread_mutex_lock(&mutex);

        while (state != MANUAL_DRIVER_STATE)
            pthread_cond_wait(&cond, &mutex);

        // printf("data throttle %d\n", data->throttle_level);

        // If brake_level == 0, throttle is engaging
        // and if throttle_level == 0, brake is engaging
        if (data->brake_level == 0)
        {
            speed = min(MAX_SPEED, speed + ACTUATOR_SPEED_CHANGE);
        }
        else
        {
            speed = max(0, speed - ACTUATOR_SPEED_CHANGE);
        }

        sendUpdates(sim_coid, MANUAL_DRIVER, brake_level, throttle_level, speed);
        usleep(50 * 1000);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
