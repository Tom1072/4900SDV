
#ifndef SRC_BACKEND_INCLUDES_CAR_STRUCTS_H_


typedef struct
{
	unsigned short skid;
	unsigned short distance;
	unsigned short speed;
	unsigned short brakeLevel;
} Sensors;

typedef struct
{
	unsigned short distance;
	unsigned short initSpeed;
	unsigned short speed;
	char           object; // can be either TRUE or FALSE
} OutsideObject;

#endif /* SRC_BACKEND_INCLUDES_CAR_STRUCTS_H_ */
