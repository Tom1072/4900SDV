#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../includes/CommListener.h"
#include "../includes/commons.h"
#include "../includes/View.h"

void startListener()
{
  int server_socket;
  int port_number = SERVER_PORT;
  int rc;
  ssize_t recv_length;
  struct sockaddr_in server_address;
  unsigned char buffer[BUFFER_SIZE];
  char cstrBuf[BUFFER_SIZE];

  printf("Creating UDP socket on port: %d\n", port_number);
  sleep(1);
  server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (server_socket < 0)
  {
    printf("COMM ERROR: startListener: Unable to create socket.\n");
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(SERVER_ADRESS);
  server_address.sin_port = htons(port_number);

  sleep(1);
  printf("Binding to port %d\n", port_number);
  rc = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
  if (rc < 0)
  {
    printf("COMM ERROR: startListener: Bind failed rc=%d\n", rc);
    return;
  }

  sleep(1);
  while (TRUE)
  {
    printf("COMM: startListener: Waiting for message...\n");
    recv_length = recvfrom(server_socket, buffer, sizeof(buffer), 0, NULL, NULL);
    if (recv_length < 0)
    {
      printf("COMM ERROR: startListener: recvfrom failed recv_length=%d\n”", recv_length);
    }
    else
    {
      rc = 0;
      while (rc < recv_length)
      {
        sprintf(&cstrBuf[rc * 2], "%02x", buffer[rc]);
        rc += 1;
      }
      printf("recvfrom successful! recv data : %s\n", cstrBuf);
    }
  }

  close(server_socket);

  return;
}

void parseMessage(char *message, CommListenerMessage *parsedMessage)
{
  if (message == NULL || parsedMessage == NULL)
  {
    printf("COMM: parseMessage: Invalid arguments\n");
    return;
  }
  // Split string by spaces
  char *token = strtok(message, " ");
  if (token == NULL)
  {
    printf("COMM: parseMessage: Invalid message\n");
    return;
  }

  printf("Token: %s\n", token);
}