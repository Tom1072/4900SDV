#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../includes/View.h"
#include "../includes/commons.h"


Environment* prevEnvironment = NULL;
Environment* environment = NULL;

void initView() {
    prevEnvironment = (Environment*) malloc(sizeof(Environment));
    environment = (Environment*) malloc(sizeof(Environment));
}

void destroyView() {
    free(prevEnvironment);
    free(environment);
    prevEnvironment = NULL;
    environment = NULL;
}


void setEnvironment(Environment* env)
{
    if (env == NULL)
    {
        printf("VIEW: Environment is not set. Please initilize View first.\n");
        return;
    }
    
    memcpy(prevEnvironment, environment, sizeof(Environment));
    memcpy(environment, env, sizeof(Environment));


    if (prevEnvironment->skid != environment->skid ||
        prevEnvironment->distance != environment->distance ||
        prevEnvironment->car_speed != environment->car_speed ||
        prevEnvironment->brake_level != environment->brake_level ||
        prevEnvironment->obj_speed != environment->obj_speed ||
        prevEnvironment->object != environment->object)
    {
        renderView();
    }
}

/**
 * @brief Update the view
 *
 */
void renderView()
{
    printf("VIEW: Environment:\n");
    if (prevEnvironment->skid != environment->skid)
    {
        printf(" - skid: %d -> %d\n", prevEnvironment->skid, environment->skid);
    }
    else
    {
        printf(" - skid: %d\n", environment->skid);
    }

    if (prevEnvironment->distance != environment->distance)
    {
        printf(" - distance: %d -> %d\n", prevEnvironment->distance, environment->distance);
    }
    else
    {
        printf(" - distance: %d\n", environment->distance);
    }

    if (prevEnvironment->car_speed != environment->car_speed)
    {
        printf(" - car_speed: %d -> %d\n", prevEnvironment->car_speed, environment->car_speed);
    }
    else
    {
        printf(" - car_speed: %d\n", environment->car_speed);
    }

    if (prevEnvironment->brake_level != environment->brake_level)
    {
        printf(" - brake_level: %d -> %d\n", prevEnvironment->brake_level, environment->brake_level);
    }
    else
    {
        printf(" - brake_level: %d\n", environment->brake_level);
    }

    if (prevEnvironment->obj_speed != environment->obj_speed)
    {
        printf(" - obj_speed: %d -> %d\n", prevEnvironment->obj_speed, environment->obj_speed);
    }
    else
    {
        printf(" - obj_speed: %d\n", environment->obj_speed);
    }
    
    if (prevEnvironment->object != environment->object)
    {
        printf(" - object: %s -> %s\n", prevEnvironment->object ? "TRUE" : "FALSE", environment->object ? "TRUE" : "FALSE");
    }
    else
    {
        printf(" - object: %s\n", environment->object ? "TRUE" : "FALSE");
    }
}

