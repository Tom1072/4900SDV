#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <string.h>

#include "../includes/tests.h"
#include "../includes/commons.h"
#include "../includes/CommListener.h"
#include "../includes/CommDispatcher.h"

static void mocked_simulator_receiver();
static void mocked_simulator_sender();

char test_comm()
{
  pthread_t dispatcher_thread;
  pthread_t listener_thread;
  pthread_t simulator_receiver_thread;
  pthread_t simulator_sender_thread;
  int status;

  printf("Testing comm...\n");
  printf("Testing parse_message()...\n");

  printf("Creating the COMM_DISPATCHER thread...\n");
  status = pthread_create(&dispatcher_thread, NULL, (void *)start_dispatcher, NULL);
  CHECK_STATUS(status);

  printf("Creating the MOCKED_SIMULATOR_SENDER thread...\n");
  status = pthread_create(&simulator_sender_thread, NULL, (void *)mocked_simulator_sender, NULL);
  CHECK_STATUS(status);

  status = pthread_join(dispatcher_thread, NULL);
  CHECK_STATUS(status);
  status = pthread_join(simulator_sender_thread, NULL);
  CHECK_STATUS(status);


  printf("Creating the MOCKED_SIMULATOR_RECEIVER thread...\n");
  status = pthread_create(&simulator_receiver_thread, NULL, (void *)mocked_simulator_receiver, NULL);
  CHECK_STATUS(status);

  printf("Created the COMM_LISTENER thread...\n");
  status = pthread_create(&listener_thread, NULL, (void *)start_listener, NULL);
  CHECK_STATUS(status);

  status = pthread_join(listener_thread, NULL);
  CHECK_STATUS(status);
  status = pthread_join(simulator_receiver_thread, NULL);
  CHECK_STATUS(status);
  printf("Comm test finished.\n");

  return TRUE;
}

static void mocked_simulator_receiver()
{
  // Declare variables
  int rcid;
  struct _pulse pulse;

  // create a channel
  printf("MOCKED_SIMULATOR: Creating a simulator channel...\n");
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
    printf("MOCKED_SIMULATOR: Waiting for a message...\n");
    rcid = MsgReceive(attach->chid, &pulse, sizeof(pulse), NULL);

    if (rcid == -1)
    {
      perror("MsgReceive()");
    }

    // print the returned value of MsgReceive
    printf("MOCKED_SIMULATOR: MsgReceive() rcid = %d\n", rcid);

    // check if it was a pulse or a message
    if (rcid == 0)
    {
      // It is a pulse
      printf("MOCKED_SIMULATOR: Pulse received\n");
      if (pulse.code == _PULSE_CODE_DISCONNECT)
      {
        printf("MOCKED_SIMULATOR: Comm is gone\n");
        ConnectDetach(pulse.scoid);
        break;
      }
      else if (pulse.code == COMM)
      {
        // else if the pulse is something else print the code and value of the pulse
        printf("MOCKED_SIMULATOR: Received pulse from Comm module\n");
        CommListenerMessage *comm_message = (CommListenerMessage *)pulse.value.sival_ptr;
        CommandData data = comm_message->data;
        CommandType command = comm_message->command;

        switch (command)
        {
        case SPAWN_CAR:
          printf("MOCKED_SIMULATOR: Received a SPAWN_CAR command\n");
          printf("MOCKED_SIMULATOR:   - command=%d\n", command);
          printf("MOCKED_SIMULATOR:   - data.spawn_car_data.distance=%d\n", data.spawn_car_data.distance);
          printf("MOCKED_SIMULATOR:   - data.spawn_car_data.obj_speed=%d\n", data.spawn_car_data.obj_speed);
          break;
        case DESPAWN_CAR:
          printf("MOCKED_SIMULATOR: Received a DESPAWN_CAR command\n");
          printf("MOCKED_SIMULATOR:   - command=%d\n", command);
          break;
        case THROTTLE:
          printf("MOCKED_SIMULATOR: Received a THROTTLE command\n");
          printf("MOCKED_SIMULATOR:   - command=%d\n", command);
          printf("MOCKED_SIMULATOR:   - data.throttle_level=%d\n", data.throttle_level);
          break;
        case BRAKE:
          printf("MOCKED_SIMULATOR: Received a BRAKE command\n");
          printf("MOCKED_SIMULATOR:   - command=%d\n", command);
          printf("MOCKED_SIMULATOR:   - data.brake_data.brake_level=%d\n", data.brake_data.brake_level);
          printf("MOCKED_SIMULATOR:   - data.brake_data.skid=%d\n", data.brake_data.skid_on);
          break;
        case ACC_SPEED:
          printf("MOCKED_SIMULATOR: Received a ACC_SPEED command\n");
          printf("MOCKED_SIMULATOR:   - command=%d\n", command);
          printf("MOCKED_SIMULATOR:   - data.acc_speed=%d\n", data.acc_speed);
          break;
        default:
          printf("MOCKED_SIMULATOR: Received an unknown command\n");
          break;
        }
        free(comm_message);
      }
      else
      {
        printf("MOCKED_SIMULATOR: Received unknown pulse with code %d\n", pulse.code);
      }
    }
    else
    {
      // It should never be a message
      printf("MOCKED_SIMULATOR: Message received...\n");
    }
  }
  // remove the name from the namespace and destroy the channel
  name_detach(attach, 0);
}

static void mocked_simulator_sender()
{
  // Declare variables needed
  int status;
  Environment* env;
  int coid;

  // establish a connection to the server's channel
  coid = name_open(COMM_NAME, _NTO_CHF_DISCONNECT);

  // send the message to the server and get a reply
  printf("MOCKED_SIMULATOR: Sending a message to Comm module...\n");
  env = (Environment*)malloc(sizeof(Environment));
  memset(env, 0, sizeof(Environment));
  status = MsgSendPulsePtr(coid, 0, SIMULATOR, (void *)env);
  if (status != OK)
  {
    perror("MsgSendPulsePtr()");
  }

  sleep(1);

  printf("MOCKED_SIMULATOR: Sending a message to Comm module...\n");
  env = (Environment*)malloc(sizeof(Environment));
  memset(env, 0, sizeof(Environment));
  env->brake_level = 10;
  env->car_speed = 10;
  env->distance = 10;
  env->obj_speed = 10;
  env->skid = 10;
  env->object = TRUE;
  status = MsgSendPulsePtr(coid, 0, SIMULATOR, (void *)env);
  if (status != OK)
  {
    perror("MsgSendPulsePtr()");
  }

  sleep(1);

  printf("MOCKED_SIMULATOR: Sending a message to Comm module...\n");
  env = (Environment*)malloc(sizeof(Environment));
  memset(env, 0, sizeof(Environment));
  env->brake_level = 20;
  env->car_speed = 20;
  env->distance = 20;
  env->obj_speed = 20;
  env->skid = 20;
  env->object = FALSE;
  status = MsgSendPulsePtr(coid, 0, SIMULATOR, (void *)env);
  if (status != OK)
  {
    perror("MsgSendPulsePtr()");
  }
  sleep(1);

  printf("MOCKED_SIMULATOR: Sending a message to Comm module...\n");
  env = (Environment*)malloc(sizeof(Environment));
  memset(env, 0, sizeof(Environment));
  env->brake_level = 30;
  env->car_speed = 30;
  env->distance = 30;
  env->obj_speed = 30;
  env->skid = 30;
  env->object = FALSE;
  status = MsgSendPulsePtr(coid, 0, SIMULATOR, (void *)env);
  if (status != OK)
  {
    perror("MsgSendPulsePtr()");
  }
  sleep(1);

  // Close a server connection that was opened by name_open()
  name_close(coid);
}
