
#pragma once


typedef struct
{
  unsigned short skid;
  unsigned short distance;
  unsigned short speed;
  unsigned short brake_level;
  unsigned short throttle_level;
} Sensors;

typedef struct
{
  unsigned short distance;
  unsigned short brake_level;
  unsigned short throttle_level;
} AccMessageInput;

typedef struct
{
  unsigned short skid;
} AbsMessageInput;

typedef struct
{
  unsigned short brake_level;
  unsigned short throttle_level;
} ManMessageInput;


typedef struct {
  unsigned short brake_level;
  unsigned short gas_level;
  unsigned short speed;
} ActuatorOutputPayload;

typedef struct
{
  unsigned short distance;
  unsigned short init_speed;
  unsigned short speed;
  char           object; // can be either TRUE or FALSE
} OutsideObject;


