#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <string.h>
#include "../includes/actuators.h"
#include "../includes/commons.h"
#include "../includes/utils.h"

/**
 * current controller
 */
volatile extern int state;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

volatile extern short brake_level;
volatile extern short throttle_level;
volatile extern double speed;

volatile extern char abs_processing;
volatile extern char acc_processing;
volatile extern char manual_processing;;

/**
 * Handler of the ManualDriver thread.
 */
void *ManualDriver()
{
    name_attach_t *attach;
    struct _pulse pulse_msg;
    ManMessageInput *input;
    ManMessageInput processed_input;
    pthread_t processor_thread;

    memset(&processed_input, 0, sizeof(ManMessageInput));

    // PRINT_ON_DEBUG("Manual Driver attached\n");
    if ((attach = name_attach(NULL, MANUAL_NAME, 0)) == NULL)
        pthread_exit(NULL);

    // Create the child processor thread
    pthread_create(&processor_thread, NULL, man_processor, &processed_input);

    while (1)
    {
        MsgReceivePulse(attach->chid, &pulse_msg, sizeof(pulse_msg), NULL);
        // PRINT_ON_DEBUG("Manual got input\n");

        if (pulse_msg.code == _PULSE_CODE_DISCONNECT)
        {
            ConnectDetach(pulse_msg.scoid);
            continue;
        }

        input = (ManMessageInput *)pulse_msg.value.sival_ptr;

        pthread_mutex_lock(&mutex);
        
        memcpy(&processed_input, input, sizeof(ManMessageInput));

        // If input is invalid (both throttle and brake engaged)
        if (!(input->brake_level > 0 && input->throttle_level > 0))
        {

            // Manual Driver is turned on when user set the throttle or brake and abs is not engaged
            //                  turned off when abs is engaged
            if (!abs_processing && (input->brake_level > 0 || input->throttle_level > 0))
            {
                manual_processing = TRUE;
                acc_processing = FALSE;
            }
            set_state();
        }
        else
        {
            PRINT_ON_DEBUG("Invalid input: both throttle and brake are engaged\n");
        }

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        free(input);
    }

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

        brake_level = min(100, data->brake_level);
        throttle_level = min(100, data->throttle_level);
        speed = calculate_speed(speed, brake_level, throttle_level);

        sendUpdates(sim_coid, brake_level, throttle_level, speed);
        usleep(TIME_INTERVAL * 1000);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
