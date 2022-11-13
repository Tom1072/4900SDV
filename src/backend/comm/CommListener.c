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

void startListener()
{
  int serverSocket, coid;
  struct sockaddr_in serverAddr, clientAddr;
  int status, bytesReceived;
  socklen_t addrSize;
  fd_set readfds, writefds;
  char buffer[MAX_STRING_LEN];
  char *response = "OK";

  // Create the server socket
  serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (serverSocket < 0)
  {
    perror("COMM ERROR: startListener: Unable to create socket.\n");
    pthread_exit(NULL);
  }

  // Setup the server address
  memset(&serverAddr, 0, sizeof(serverAddr)); // zeros the struct serverAddr.sin_family = AF_INET;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADRESS);
  serverAddr.sin_port = htons(SERVER_PORT);

  // Bind the server socket
  printf("Binding to port %d\n", SERVER_PORT);
  status = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if (status < 0)
  {
    perror("COMM ERROR: startListener: Bind failed");
    pthread_exit(NULL);
  }

	//establish a connection to the simulator's channel
	coid = name_open(SIMULATOR_NAME, _NTO_CHF_DISCONNECT);

  // Wait for clients now
  while (TRUE)
  {
    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);
    FD_ZERO(&writefds);
    FD_SET(serverSocket, &writefds);

    printf("COMM: startListener: Waiting for message...\n");
    status = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);
    if (status == 0)
    { // Timeout occurred, no client ready
      perror("COMM ERROR: Timeout occurred, no client ready");
    }
    else if (status < 0)
    {
      perror("COMM ERROR: Could not select socket");
      break;
    }
    else
    {
      addrSize = sizeof(clientAddr);
      bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer),
                               0, (struct sockaddr *)&clientAddr, &addrSize);
      if (bytesReceived > 0)
      {
        buffer[bytesReceived] = '\0';
        printf("COMM: Received client request: %s\n", buffer);
      }
      // If the client said to stop, then I'll stop myself
      if (strcmp(buffer, "stop") == 0)
      {
        // Respond with an "OK" message
        printf("COMM: Closing CommListener server\n");
        response = "OK: Closing CommListener server.";

        printf("COMM: Sending \"%s\" to client\n", response);
        sendto(serverSocket, response, strlen(response), 0,
               (struct sockaddr *)&clientAddr, addrSize);
        break;
      }
      else
      {
        CommListenerMessage *msg = NULL;
        parseMessage(buffer, &msg);
        if (msg != NULL)
        {
          printf("COMM: Command parsed\n");
          response = "OK: Command parsed.";
          status = MsgSendPulsePtr(coid, 0, COMM, (void*) msg);
        }
        else
        {
          printf("COMM: Cannot parse command sent by client.\n");
          response = "NOK: Cannot parse command sent by client.";
        }

        printf("COMM: Sending \"%s\" to client\n", response);
        sendto(serverSocket, response, strlen(response), 0,
               (struct sockaddr *)&clientAddr, addrSize);
      }
    }
  }
  name_close(coid);
  pthread_exit(NULL);
}

void parseMessage(char *message, CommListenerMessage **parsedMessage)
{
  if (message == NULL || parsedMessage == NULL)
  {
    printf("COMM: parseMessage: Invalid arguments\n");
    return;
  }
  // Split string by spaces
  char *token = strtok(message, " ");
  if (token != NULL)
  {
    printf("COMM: parseMessage: %s\n", token);
    if (strcmp(token, "spawn") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        int distance = atoi(token);
        char atoiResult = checkAtoiResult(distance, token);
        token = strtok(NULL, " ");
        if (token != NULL && atoiResult && inRange(distance, 0, 100))
        {
          int obj_speed = atoi(token);

          if (checkAtoiResult(obj_speed, token) && inRange(obj_speed, 0, 100) )
          {
            *parsedMessage = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
            memset(*parsedMessage, 0, sizeof(CommListenerMessage));

            (*parsedMessage)->command = SPAWN_CAR;
            (*parsedMessage)->data.spawnCarData.distance = distance;
            (*parsedMessage)->data.spawnCarData.obj_speed = obj_speed;
          }
        }
      }
    }
    else if (strcmp(token, "despawn") == 0)
    {
      *parsedMessage = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
      memset(*parsedMessage, 0, sizeof(CommListenerMessage));

      (*parsedMessage)->command = DESPAWN_CAR;
    }
    else if (strcmp(token, "gas") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        int throttleLevel = atoi(token);

        if (checkAtoiResult(throttleLevel, token) && inRange(throttleLevel, 0, 100))
        {
          *parsedMessage = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
          memset(*parsedMessage, 0, sizeof(CommListenerMessage));

          (*parsedMessage)->command = THROTTLE;
          (*parsedMessage)->data.throttleLevel = throttleLevel;
        }
      }
    }
    else if (strcmp(token, "brake") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        int brakeLevel = atoi(token);

        *parsedMessage = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
        memset(*parsedMessage, 0, sizeof(CommListenerMessage));

        (*parsedMessage)->command = BRAKE;
        (*parsedMessage)->data.brakeLevel = brakeLevel;
      }
    }
    else if (strcmp(token, "skid") == 0)
    {
      token = strtok(NULL, " ");
      if (token != NULL)
      {
        if (strcmp(token, "on") == 0)
        {
          *parsedMessage = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
          memset(*parsedMessage, 0, sizeof(CommListenerMessage));

          (*parsedMessage)->command = SKID;
          (*parsedMessage)->data.skidOn = TRUE;
        }
        else if (strcmp(token, "off") == 0)
        {
          *parsedMessage = (CommListenerMessage *)malloc(sizeof(CommListenerMessage));
          memset(*parsedMessage, 0, sizeof(CommListenerMessage));

          (*parsedMessage)->command = SKID;
          (*parsedMessage)->data.skidOn = FALSE;
        }
      }
    }
    else
    {
      // Invalid command
      *parsedMessage = NULL;
    }
  }
}

char checkAtoiResult(int result, char *token)
{
  // Because atoi returns 0 if it cannot convert the string to an integer
  return (result == 0 && strcmp(token, "0")) == 0 || result != 0;
}

char inRange(int value, int min, int max)
{
  return value >= min && value <= max;
}