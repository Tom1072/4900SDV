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
  sleep(1);

  env.skid = 20;
  setEnvironment(&env);

  sleep(1);
  env.distance = 21;
  setEnvironment(&env);

  sleep(1);
  env.carSpeed = 22;
  setEnvironment(&env);

  sleep(1);
  env.brakeLevel = 23;
  setEnvironment(&env);

  sleep(1);
  env.objSpeed = 24;
  setEnvironment(&env);

  sleep(1);
  env.object = TRUE;
  setEnvironment(&env);

  sleep(1);
  env.skid = 30;
  env.distance = 31;
  env.carSpeed = 32;
  env.brakeLevel = 33;
  env.objSpeed = 34;
  env.object = FALSE;
  setEnvironment(&env);

  destroyView();

  return TRUE;
}
