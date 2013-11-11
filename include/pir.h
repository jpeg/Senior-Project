#ifndef COMMON_INCLUDES
#define COMMON_INCLUDES
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <softPwm.h>
#endif

#ifndef PIR
#define PIR 0
#define MOTION_LED 4

//extern int motionDetected;

void initPIR(void);
int motionDetected(void);

#endif