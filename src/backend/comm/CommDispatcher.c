#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

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

  // create a channel
  name_attach_t *attach = NULL;
  attach = name_attach(NULL, COMM_NAME, _NTO_CHF_DISCONNECT);

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
    printf("COMM_DISPATCHER: MsgReceive() rcid = %d\n", rcid);

    // check if it was a pulse or a message
    if (rcid == 0)
    {
      if (pulse.code == _PULSE_CODE_DISCONNECT)
      {
        printf("COMM_DISPATCHER: Simulator is gone\n");
        ConnectDetach(pulse.scoid);
        break;
      }
      else
      {
        Environment *env = (Environment *)pulse.value.sival_ptr;
        printf("COMM_DISPATCHER: new Evironment received, updating View\n");
        set_environment(env);
        free(env);
      }
    }
    else
    {
      printf("Message received, something's wrong\n");
    }
  }

  name_detach(attach, 0);
  destroy_view();
}
