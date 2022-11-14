#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "../includes/CommListener.h"
#include "../includes/commons.h"
#include "../includes/View.h"

void start_listener()
{
  int server_socket, coid;
  struct sockaddr_in server_addr, client_addr;
  int status, bytes_received;
  socklen_t addr_size;
  fd_set readfds, writefds;
  char buffer[MAX_STRING_LEN];
  char *response = "OK";

  // Create the server socket
  server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (server_socket < 0)
  {
    perror("COMM_LISTENER ERROR: start_listener: Unable to create socket.\n");
    pthread_exit(NULL);
  }

  // Setup the server address
  memset(&server_addr, 0, sizeof(server_addr)); // zeros the struct serverAddr.sin_family = AF_INET;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_ADRESS);
  server_addr.sin_port = htons(SERVER_PORT);

  // Bind the server socket
  printf("Binding to port %d\n", SERVER_PORT);
  status = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (status < 0)
  {
    perror("COMM_LISTENER ERROR: start_listener: Bind failed");
    pthread_exit(NULL);
  }

  // establish a connection to the simulator's channel
  coid = name_open(SIMULATOR_NAME, _NTO_CHF_DISCONNECT);

  // Wait for clients now
  while (TRUE)
  {
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);
    FD_ZERO(&writefds);
    FD_SET(server_socket, &writefds);

    printf("COMM_LISTENER: start_listener: Waiting for message...\n");
    status = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);
    if (status == 0)
    { // Timeout occurred, no client ready
      perror("COMM_LISTENER ERROR: Timeout occurred, no client ready");
    }
    else if (status < 0)
    {
      perror("COMM_LISTENER ERROR: Could not select socket");
      break;
    }
    else
    {
      addr_size = sizeof(client_addr);
      bytes_received = recvfrom(server_socket, buffer, sizeof(buffer),
                                0, (struct sockaddr *)&client_addr, &addr_size);
      if (bytes_received > 0)
      {
        buffer[bytes_received] = '\0';
        printf("COMM_LISTENER: Received client request: %s\n", buffer);
      }
      // If the client said to stop, then I'll stop myself
      if (strcmp(buffer, "stop") == 0)
      {
        // Respond with an "OK" message
        printf("COMM_LISTENER: Closing CommListener server\n");
        response = "OK: Closing CommListener server.";

        printf("COMM_LISTENER: Sending \"%s\" to client\n", response);
        sendto(server_socket, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, addr_size);
        break;
      }
      else
      {
        CommListenerMessage *msg = NULL;
        parse_message(buffer, &msg);
        if (msg != NULL)
        {
          printf("COMM_LISTENER: Command parsed\n");
          response = "OK: Command parsed.";
          status = MsgSendPulsePtr(coid, 0, COMM, (void *)msg);
        }
        else
        {
          printf("COMM_LISTENER: Cannot parse command sent by client.\n");
          response = "NOK: Cannot parse command sent by client.";
        }

        printf("COMM_LISTENER: Sending \"%s\" to client\n", response);
        sendto(server_socket, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, addr_size);
      }
    }
  }
  name_close(coid);
  pthread_exit(NULL);
}

void parse_message(char *message, CommListenerMessage **parsed_message)
{
  if (message == NULL || parsed_message == NULL)
  {
    printf("COMM_LISTENER: parse_message: Invalid arguments\n");
    return;
  }
  // Split string by spaces
  char *token = strtok(message, " ");
  if (token != NULL)
  {
    printf("COMM_LISTENER: parse_message: %s\n", token);
    if (strcmp(token, "spawn") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        int distance = atoi(token);
        char atoi_result = check_atoi(distance, token);
        token = strtok(NULL, " ");
        if (token != NULL && atoi_result && in_range(distance, 0, 100))
        {
          int obj_speed = atoi(token);

          if (check_atoi(obj_speed, token) && in_range(obj_speed, 0, 100))
          {
            *parsed_message = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
            memset(*parsed_message, 0, sizeof(CommListenerMessage));

            (*parsed_message)->command = SPAWN_CAR;
            (*parsed_message)->data.spawn_car_data.distance = distance;
            (*parsed_message)->data.spawn_car_data.obj_speed = obj_speed;
          }
        }
      }
    }
    else if (strcmp(token, "despawn") == 0)
    {
      *parsed_message = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
      memset(*parsed_message, 0, sizeof(CommListenerMessage));

      (*parsed_message)->command = DESPAWN_CAR;
    }
    else if (strcmp(token, "gas") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        int throttle_level = atoi(token);

        if (check_atoi(throttle_level, token) && in_range(throttle_level, 0, 100))
        {
          *parsed_message = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
          memset(*parsed_message, 0, sizeof(CommListenerMessage));

          (*parsed_message)->command = THROTTLE;
          (*parsed_message)->data.throttle_level = throttle_level;
        }
      }
    }
    else if (strcmp(token, "brake") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        int brake_level = atoi(token);

        *parsed_message = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
        memset(*parsed_message, 0, sizeof(CommListenerMessage));

        (*parsed_message)->command = BRAKE;
        (*parsed_message)->data.brake_level = brake_level;
      }
    }
    else if (strcmp(token, "skid") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        if (strcmp(token, "on") == 0)
        {
          *parsed_message = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
          memset(*parsed_message, 0, sizeof(CommListenerMessage));

          (*parsed_message)->command = SKID;
          (*parsed_message)->data.skid_on = TRUE;
        }
        else if (strcmp(token, "off") == 0)
        {
          *parsed_message = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
          memset(*parsed_message, 0, sizeof(CommListenerMessage));

          (*parsed_message)->command = SKID;
          (*parsed_message)->data.skid_on = FALSE;
        }
      }
    }
    else
    {
      // Invalid command
      *parsed_message = NULL;
    }
  }
}

char check_atoi(int result, char *token)
{
  // Because atoi returns 0 if it cannot convert the string to an integer
  return (result == 0 && strcmp(token, "0")) == 0 || result != 0;
}

char in_range(int value, int min, int max)
{
  return value >= min && value <= max;
}