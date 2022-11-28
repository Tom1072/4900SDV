#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../includes/View.h"
#include "../includes/commons.h"

Environment *prev_environment = NULL;
Environment *environment = NULL;

void init_view()
{
    prev_environment = (Environment *)malloc(sizeof(Environment));
    environment = (Environment *)malloc(sizeof(Environment));
}

void destroy_view()
{
    free(prev_environment);
    free(environment);
    prev_environment = NULL;
    environment = NULL;
}

void set_environment(Environment *env)
{
    if (env == NULL)
    {
        printf("VIEW: Environment is not set. Please initilize View first.\n");
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
 * @brief Update the view
 *
 */
void render_view()
{

    char *skid_str = environment->skid ? "SKID" : "NO SKID";
    char *obj = environment->object ? "TRUE" : "FALSE";
    printf("VIEW: throttle=%d, brake=(%d, %s), speed=%lf, distance=%lf, obj=(%s, %lf)\n", 
    environment->throttle_level,
    environment->brake_level, skid_str,
    environment->car_speed,
    environment->distance,
    obj, environment->obj_speed);
    // if (prev_environment->skid != environment->skid)
    // {
    //     printf(" - skid: %d -> %d\n", prev_environment->skid, environment->skid);
    // }
    // else
    // {
    //     printf(" - skid: %d\n", environment->skid);
    // }

    // if (prev_environment->distance != environment->distance)
    //     {
    //     printf(" - distance: %d -> %d\n", prev_environment->distance, environment->distance);
    // }
    // else
    // {
    //     printf(" - distance: %d\n", environment->distance);
    // }

    // if (prev_environment->car_speed != environment->car_speed)
    // {
    //     printf(" - car_speed: %d -> %d\n", prev_environment->car_speed, environment->car_speed);
    // }
    // else
    // {
    //     printf(" - car_speed: %d\n", environment->car_speed);
    // }

    // if (prev_environment->brake_level != environment->brake_level)
    // {
    //     printf(" - brake_level: %d -> %d\n", prev_environment->brake_level, environment->brake_level);
    // }
    // else
    // {
    //     printf(" - brake_level: %d\n", environment->brake_level);
    // }

    // if (prev_environment->obj_speed != environment->obj_speed)
    // {
    //     printf(" - obj_speed: %d -> %d\n", prev_environment->obj_speed, environment->obj_speed);
    // }
    // else
    // {
    //     printf(" - obj_speed: %d\n", environment->obj_speed);
    // }

    // if (prev_environment->object != environment->object)
    // {
    //     printf(" - object: %s -> %s\n", prev_environment->object ? "TRUE" : "FALSE", environment->object ? "TRUE" : "FALSE");
    // }
    // else
    // {
    //     printf(" - object: %s\n", environment->object ? "TRUE" : "FALSE");
    // }
}
