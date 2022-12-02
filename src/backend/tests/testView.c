#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

#include "../includes/tests.h"
#include "../includes/View.h"
#include "../includes/commons.h"

/**
 * @brief Test the View module
 * 
 * @return char TRUE for success, FALSE for failure
 */
char test_view()
{

  Environment env = {10, 11, 12, 13, 14, FALSE};
  init_view();
  set_environment(&env);

  env.skid = 20;
  set_environment(&env);

  env.distance = 21;
  set_environment(&env);

  env.car_speed = 22;
  set_environment(&env);

  env.brake_level = 23;
  set_environment(&env);

  env.obj_speed = 24;
  set_environment(&env);

  env.object = TRUE;
  set_environment(&env);

  env.skid = 30;
  env.distance = 31;
  env.car_speed = 32;
  env.brake_level = 33;
  env.obj_speed = 34;
  env.object = FALSE;
  set_environment(&env);

  destroy_view();

  return TRUE;
}
