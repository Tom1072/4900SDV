/*
 * data.c
 *
 * Saving the data to file
 */
#include <stdio.h>
#include <float.h>
#include "../includes/commons.h"

 void write_data(FILE *fn, Environment *env)
 {
  fprintf(fn, "%d,%d,%.4f,%d,%.4f,%.4f,%d\n",
		     env->throttle_level,
			 env->brake_level,
			 env->car_speed,
			 env->object ? 1 : 0,
			 env->obj_speed,
			 env->distance == DBL_MAX ? 99999 : env->distance,
			 env->skid ? 1 : 0);
 }

