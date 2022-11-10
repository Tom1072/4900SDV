#include <stdio.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <errno.h>
#include "../includes/commons.h"

#define MAX_STRING_LEN    256

/**
 * Engage the brake level.
 * This sends pulse to Simulator.
 * Constraint: 0 <= level <= 100
*/
void engageBrake(int level) {
  // actuatorChanges_t message;
}

/**
 * Engage the throttle level (gas engine power).
 * This sends pulse to Simulator.
 * Constraint: 0 <= level <= 100
*/
void engageThrottle(int level) {
  // actuatorChanges_t message;
}