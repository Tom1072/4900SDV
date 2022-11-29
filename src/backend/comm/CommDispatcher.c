#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <float.h>

#include "../includes/CommDispatcher.h"
#include "../includes/View.h"
#include "../includes/commons.h"

/**
 * @brief Starts the dispatcher server
 *
 */
void start_dispatcher()
{
  struct _pulse pulse;

  // Declare variables
  int rcid;
  int client_socket;
  struct sockaddr_in server_address;
  int status, bytes_rcv;
  socklen_t addr_size;
  char buffer[MAX_STRING_LEN];

  // Create the client socket
  client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  assert(client_socket >= 0);

  // create a channel
  name_attach_t *attach = NULL;
  attach = name_attach(NULL, COMM_NAME, _NTO_CHF_DISCONNECT);
  assert(attach != NULL);

  // Setup address
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(VIEW_SERVER_ADDRESS);
  server_address.sin_port = htons(CLIENT_PORT);

  // char* msg = "Message from COMM_DISPATCHER";
  // sendto(client_socket, msg, strlen(msg), 0, (struct sockaddr *)&server_address, sizeof(server_address));
  // init view
  init_view();

  if (attach == NULL)
  {
    perror("COMM_DISPATCHER: Failed to create COMM dispatcher channel");
    pthread_exit(NULL);
  }

  // the server should keep receiving, processing and replying to messages
  while (TRUE)
  {
    // code to receive msg or pulse from client
    rcid = MsgReceive(attach->chid, &pulse, sizeof(pulse), NULL);

    if (rcid == -1)
    {
      perror("COMM_DISPATCHER: Failed to receive message from Simulator");
    }

    // print the returned value of MsgReceive
    // PRINT_ON_DEBUG("COMM_DISPATCHER: MsgReceive() rcid = %d\n", rcid);

    // check if it was a pulse or a message
    if (rcid == 0)
    {
      if (pulse.code == _PULSE_CODE_DISCONNECT)
      {
        PRINT_ON_DEBUG("COMM_DISPATCHER: Simulator is gone\n");

        // Send stop to ViewListener
        char *msg = "stop";
        sendto(client_socket, msg, strlen(msg), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        ConnectDetach(pulse.scoid);
        break;
      }
      else
      {
        Environment *env = (Environment *)pulse.value.sival_ptr;
        // PRINT_ON_DEBUG("COMM_DISPATCHER: new Evironment received, updating View\n");
        set_environment(env);

        addr_size = sizeof(server_address);

        memset(buffer, 0, sizeof(buffer));
        char *skid_str = env->skid ? "true" : "false";
        char *obj = env->object ? "true" : "false";
        sprintf(buffer, "{ \"throttle\": %d, \"brake\":%d, \"skid\": %s, \"speed\": %lf, \"distance\": %lf, \"obj\": %s , \"distance\": %lf }",
          env->throttle_level,
          env->brake_level, skid_str,
          env->car_speed,
          env->distance,
          obj, env->obj_speed);

        PRINT_ON_DEBUG("COMM_DISPATCHER: send %s to ViewListener\n", buffer);

        sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_address, sizeof(server_address));

        bytes_rcv = recvfrom(client_socket, buffer, MAX_STRING_LEN, 0, (struct sockaddr *)&server_address, &addr_size);
        buffer[bytes_rcv] = '\0';

        PRINT_ON_DEBUG("COMM_DISPATCHER: received %s from ViewListener\n", buffer);

        free(env);
      }
    }
    else
    {
      PRINT_ON_DEBUG("Message received, something's wrong\n");
    }
  }

  name_detach(attach, 0);
  destroy_view();
  close(client_socket);
}
