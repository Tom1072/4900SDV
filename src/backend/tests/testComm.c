#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>

#include "../includes/tests.h"
#include "../includes/commons.h"
#include "../includes/CommListener.h"
#include "../includes/CommDispatcher.h"

#define MAX_STRING_LEN 256

void mockedSimulator();

char testComm()
{
    int dispatcherThread, listenerThread, simulatorThread, actuatorThread;

    printf("Testing comm...\n");
    printf("Creating the CommDispatcher thread...\n");
    dispatcherThread = pthread_create(NULL, NULL, (void *)startDispatcher, NULL);

    printf("Creating the MockedSimulator thread...\n");
    // simulatorThread = pthread_create(NULL, NULL, (void *)mockedSimulator, NULL);

    printf("Created the CommListener thread...\n");
    listenerThread = pthread_create(NULL, NULL, (void *)startListener, NULL);
    
    // pthread_join(simulatorThread, NULL);
    pthread_join(dispatcherThread, NULL);
    pthread_join(listenerThread, NULL);

    return TRUE;
}

void mockedSimulator()
{
    typedef union
    {
        struct _pulse pulse;
        char rmsg[MAX_STRING_LEN + 1];
    } myMessage_t;

    // Declare variables
    int rcid, status;
    myMessage_t message;

    // create a channel
    printf("MockedSimulator: Creating a simulator channel...\n");
    name_attach_t *attach = NULL;
    attach = name_attach(NULL, "simulator", 0);

    if (attach == NULL)
    {
        perror("ChannelCreate()");
    }

    // the server should keep receiving, processing and replying to messages
    for (int i=0; i<10; i++)
    {
        // code to receive msg or pulse from client
        printf("MockedSimulator: Waiting for a message...\n");
        rcid = MsgReceive(attach->chid, &message, sizeof(message), NULL);

        if (rcid == -1)
        {
            perror("MsgReceive()");
        }

        // print the returned value of MsgReceive
        printf("MockedSimulator: MsgReceive() rcid = %d\n", rcid);

        // check if it was a pulse or a message
        if (rcid == 0)
        {
            // It is a pulse
            printf("MockedSimulator: Pulse received\n");
            if (message.pulse.code == _PULSE_CODE_DISCONNECT)
            {
                // check the pulse code to see if the client is gone/disconnected and print (client is gone)
                printf("MockedSimulator: client is gone\n");

                // detach the client
                ConnectDetach(message.pulse.scoid);
            }
            else
            {
                // else if the pulse is something else print the code and value of the pulse
                printf("MockedSimulator: pulse code = %d, pulse value = %d\n", message.pulse.code, message.pulse.value.sival_int);
            }
        }
        else
        {
            // It should never be a message
            printf("MockedSimulator: Message received\n");
            printf("MockedSimulator: message = %s\n", message.rmsg);
            // reply to client with the checksum
            status = MsgReply(rcid, 0, &message.rmsg, sizeof(message.rmsg));

            if (status == -1)
            {
                perror("MsgReply()");
            }
            // print the return value of MsgReply
            printf("MockedSimulator: MsgReply() status = %d\n", status);
        }

    }
    // remove the name from the namespace and destroy the channel
    name_detach(attach, 0);
}
