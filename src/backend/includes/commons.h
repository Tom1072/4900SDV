#define TRUE 1
#define FALSE 0

typedef struct
{
	unsigned short skid;         // level of skid
	unsigned short distance;     // distance from car to object
	unsigned short car_speed;        // car current speed
	unsigned short brakeLevel;   // current brake position
	unsigned short obj_speed;    // object in front speed
	char           object;       // can be either TRUE or FALSE if not set
} Environment;
