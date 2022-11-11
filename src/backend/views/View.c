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
        prevEnvironment->carSpeed != environment->carSpeed ||
        prevEnvironment->brakeLevel != environment->brakeLevel ||
        prevEnvironment->objSpeed != environment->objSpeed ||
        prevEnvironment->object != environment->object)
    {
        renderView();
    }
}


/**
 * @brief Get the user input from the console
 *
 */
void getUserInput(char* prompt, char** message)
{
    printf("%s", prompt);
    char *input = (char *)malloc(sizeof(char) * MAX_STRING_LEN);
    fgets(input, MAX_STRING_LEN, stdin);
    *message = input;
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

    if (prevEnvironment->carSpeed != environment->carSpeed)
    {
        printf(" - carSpeed: %d -> %d\n", prevEnvironment->carSpeed, environment->carSpeed);
    }
    else
    {
        printf(" - carSpeed: %d\n", environment->carSpeed);
    }

    if (prevEnvironment->brakeLevel != environment->brakeLevel)
    {
        printf(" - brakeLevel: %d -> %d\n", prevEnvironment->brakeLevel, environment->brakeLevel);
    }
    else
    {
        printf(" - brakeLevel: %d\n", environment->brakeLevel);
    }

    if (prevEnvironment->objSpeed != environment->objSpeed)
    {
        printf(" - objSpeed: %d -> %d\n", prevEnvironment->objSpeed, environment->objSpeed);
    }
    else
    {
        printf(" - objSpeed: %d\n", environment->objSpeed);
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

