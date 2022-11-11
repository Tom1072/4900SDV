#pragma once

#define TRUE 1
#define FALSE 0

#define MAX_STRING_LEN    256

typedef struct
{
    unsigned short skid;         // level of skid
    unsigned short distance;     // distance from car to object
    unsigned short carSpeed;        // car current speed
    unsigned short brakeLevel;   // current brake position
    unsigned short objSpeed;    // object in front speed
    char           object;       // can be either TRUE or FALSE if not set
} Environment;

typedef enum
{
    COMM = 0,
    ACTUATOR,
} PulseCode;