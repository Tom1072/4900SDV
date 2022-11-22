#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <errno.h>
#include "../includes/commons.h"
#include "../includes/actuators.h"

/**
 * Engage the actuators.
 * This function sends an ActuatorOutputPayload to Simulator
 */
void sendUpdates(int sim_coid, unsigned short brake_level, unsigned short throttle_level, double speed)
{
  printf("Brake: %3d, throttle: %3d, speed: %6.2f\n", brake_level, throttle_level, speed);
  // ActuatorOutputPayload msg = { brake_level, throttle_level, speed };
  // MsgSendPulsePtr(sim_coid, -1, ACTUATORS, (void *)&msg); // TODO: Enable this
}