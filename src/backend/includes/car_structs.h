
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
  unsigned short brake_level;
  unsigned short throttle_level;
  double current_speed;
  double desired_speed;
  double distance;
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
  double         speed;
} ActuatorOutputPayload;

typedef struct
{
  double distance;
  double init_speed;
  double speed;
  char   object; // can be either TRUE or FALSE
} OutsideObject;


