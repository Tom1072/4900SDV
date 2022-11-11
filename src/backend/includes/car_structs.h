
#pragma once


typedef struct
{
  unsigned short skid;
  unsigned short distance;
  unsigned short speed;
  unsigned short brake_level;
} Sensors;

typedef struct
{
  unsigned short distance;
  unsigned short initSpeed;
  unsigned short speed;
  char           object; // can be either TRUE or FALSE
} OutsideObject;


