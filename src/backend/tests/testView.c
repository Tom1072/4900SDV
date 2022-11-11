#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include "../includes/tests.h"
#include "../includes/View.h"
#include "../includes/commons.h"

char testView()
{
  // char *input = NULL;
  // getUserInput("Enter something: ", &input);
  // assert(input != NULL);
  // printf("Input: %s", input);
  // free(input);

  Environment env = {10, 11, 12, 13, 14, FALSE};
  initView();
  setEnvironment(&env);

  env.skid = 20;
  setEnvironment(&env);

  env.distance = 21;
  setEnvironment(&env);

  env.car_speed = 22;
  setEnvironment(&env);

  env.brake_level = 23;
  setEnvironment(&env);

  env.obj_speed = 24;
  setEnvironment(&env);

  env.object = TRUE;
  setEnvironment(&env);

  env.skid = 30;
  env.distance = 31;
  env.car_speed = 32;
  env.brake_level = 33;
  env.obj_speed = 34;
  env.object = FALSE;
  setEnvironment(&env);

  destroyView();

  return TRUE;
}
