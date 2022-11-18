
#pragma once


typedef struct
{
  unsigned short skid;
  double distance;
  double speed;
  unsigned short brake_level;
  unsigned short throttle_level;
} Sensors;

typedef struct
{
  double distance;
  double init_speed;
  double speed;
  char   object; // can be either TRUE or FALSE
} OutsideObject;


