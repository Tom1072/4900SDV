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
  // pthread_t dispatcherThread;
  pthread_t listenerThread;
  pthread_t simulatorThread;
  int status;

  printf("Testing comm...\n");
  printf("Testing parse_message()...\n");

  // printf("Creating the CommDispatcher thread...\n");
  // status = pthread_create(&dispatcherThread, NULL, (void *)startDispatcher, NULL);
  // CHECK_STATUS(status);

  printf("Creating the MockedSimulator thread...\n");
  status = pthread_create(&simulatorThread, NULL, (void *)mockedSimulator, NULL);
  CHECK_STATUS(status);

  printf("Created the CommListener thread...\n");
  status = pthread_create(&listenerThread, NULL, (void *)startListener, NULL);
  CHECK_STATUS(status);

  // pthread_join(simulatorThread, NULL);
  // status = pthread_join(dispatcherThread, NULL);
  status = pthread_join(listenerThread, NULL);
  status = pthread_join(simulatorThread, NULL);
  printf("Comm test finished.\n");

  return TRUE;
}

void mockedSimulator()
{
  // Declare variables
  int rcid;
  struct _pulse pulse;

  // create a channel
  printf("MockedSimulator: Creating a simulator channel...\n");
  name_attach_t *attach = NULL;
  attach = name_attach(NULL, SIMULATOR_NAME, 0);

  if (attach == NULL)
  {
    perror("ChannelCreate()");
  }

  // the server should keep receiving, processing and replying to messages
  while (TRUE)
  {
    // code to receive msg or pulse from client
    printf("MockedSimulator: Waiting for a message...\n");
    rcid = MsgReceive(attach->chid, &pulse, sizeof(pulse), NULL);

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
      if (pulse.code == _PULSE_CODE_DISCONNECT)
      {
        printf("MockedSimulator: Comm is gone\n");
        ConnectDetach(pulse.scoid);
        break;
      }
      else if (pulse.code == COMM)
      {
        // else if the pulse is something else print the code and value of the pulse
        printf("MockedSimulator: Received pulse from Comm module\n");
        CommListenerMessage *commMessage = (CommListenerMessage *)pulse.value.sival_ptr;
        CommandData data = commMessage->data;
        CommandType command = commMessage->command;

        switch (command)
        {
        case SPAWN_CAR:
          printf("MockedSimulator: Received a SPAWN_CAR command\n");
          printf("MockedSimulator:   - command=%d\n", command);
          printf("MockedSimulator:   - data.spawnCarData.distance=%d\n", data.spawnCarData.distance);
          printf("MockedSimulator:   - data.spawnCarData.obj_speed=%d\n", data.spawnCarData.obj_speed);
          break;
        case DESPAWN_CAR:
          printf("MockedSimulator: Received a DESPAWN_CAR command\n");
          printf("MockedSimulator:   - command=%d\n", command);
          break;
        case THROTTLE:
          printf("MockedSimulator: Received a THROTTLE command\n");
          printf("MockedSimulator:   - command=%d\n", command);
          printf("MockedSimulator:   - data.throttleLevel=%d\n", data.throttleLevel);
          break;
        case BRAKE:
          printf("MockedSimulator: Received a BRAKE command\n");
          printf("MockedSimulator:   - command=%d\n", command);
          printf("MockedSimulator:   - data.brakeLevel=%d\n", data.brakeLevel);
          break;
        case SKID:
          printf("MockedSimulator: Received a SKID command\n");
          printf("MockedSimulator:   - command=%d\n", command);
          printf("MockedSimulator:   - data.skidOn=%s\n", data.skidOn ? "TRUE" : "FALSE");
          break;
        default:
          printf("MockedSimulator: Received an unknown command\n");
          break;
        }
        free(commMessage);
      }
      else
      {
        printf("MockedSimulator: Received unknown pulse with code %d\n", pulse.code);
      }
    }
    else
    {
      // It should never be a message
      printf("MockedSimulator: Message received...\n");
    }
  }
  // remove the name from the namespace and destroy the channel
  name_detach(attach, 0);
}
