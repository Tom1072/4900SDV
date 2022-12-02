#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <errno.h>
#include "../includes/commons.h"
#include "../includes/actuators.h"

/**
 * @brief Send update to the Simulator
 * 
 * @param sim_coid The Simulator connection ID
 * @param brake_level The updated brake level
 * @param throttle_level The updated throttle level
 * @param speed The updated car speed
 */
void sendUpdates(int sim_coid, short brake_level, short throttle_level, double speed)
{
  PRINT_ON_DEBUG("Brake: %3d, throttle: %3d, speed: %6.2f\n", brake_level, throttle_level, speed);
  ActuatorOutputPayload *msg = malloc(sizeof(ActuatorOutputPayload));
  msg->brake_level = brake_level;
  msg->throttle_level = throttle_level;
  msg->speed = speed;
  MsgSendPulsePtr(sim_coid, -1, ACTUATORS, (void *)msg);
}
