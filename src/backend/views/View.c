#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <float.h>

#include "../includes/View.h"
#include "../includes/commons.h"

/**
 * Display the current state of the SDV to stdout
 * 
 */

Environment *prev_environment = NULL;
Environment *environment = NULL;

/**
 * @brief Initialize the view
 * This function must be called before any other view functions
 * 
 */
void init_view()
{
    prev_environment = (Environment *)malloc(sizeof(Environment));
    environment = (Environment *)malloc(sizeof(Environment));
}

/**
 * @brief Destroy the view, free the memory
 * This function must be called after the view is no longer needed
 * 
 */
void destroy_view()
{
    free(prev_environment);
    free(environment);
    prev_environment = NULL;
    environment = NULL;
}

/**
 * @brief Set the Environment object
 * This will only set the attributes that are not NULL
 * Ex: If env->skid is NULL, then the current environment->skid will not be changed
 * 
 * @param env The new environment data
 */
void set_environment(Environment *env)
{
    if (env == NULL)
    {
        PRINT_ON_DEBUG("VIEW: Environment is not set. Please initilize View first.\n");
        return;
    }

    memcpy(prev_environment, environment, sizeof(Environment));
    memcpy(environment, env, sizeof(Environment));

    if (prev_environment->skid != environment->skid ||
        prev_environment->distance != environment->distance ||
        prev_environment->car_speed != environment->car_speed ||
        prev_environment->brake_level != environment->brake_level ||
        prev_environment->obj_speed != environment->obj_speed ||
        prev_environment->object != environment->object)
    {
        render_view();
    }
}

/**
 * @brief Print the current environment
 *
 */
void render_view()
{

    char *skid_str = environment->skid ? "SKID" : "NO SKID";
    char *obj = environment->object ? "TRUE" : "FALSE";
    PRINT_ON_INFO("VIEW: throttle=%d, brake=(%d, %s), speed=%lf, distance=%lf, obj=(%s, %lf)\n", 
        environment->throttle_level,
        environment->brake_level, skid_str,
        environment->car_speed,
        environment->distance == DBL_MAX ? 99999 : environment->distance,
        obj, environment->obj_speed);

}
