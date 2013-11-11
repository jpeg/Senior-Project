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

#ifndef MB1242_I2C
#define MB1242_I2C 0x70            // device 7 bit I2C address 
#define MB1242_READ 0xE1           // 8 bit i2c address, plus high R/W bit
#define MB1242_WRITE 0xE0
#define MB1242_RANGE_CMD 0x51      
#define MB1242_ADDRESS_CMD1 0xAA   // Address change sent in this sequence
#define MB1242_ADDRESS_CMD2 0xA5   // Address change 
#define TRAIN_SECONDS 5            // Seconds to get check for default distance
#define NUM_REGIONS 5
#define FAR_END 200
#define NEAR_END 50
#define POS_4 ((sonar.current_range < FAR_END   ) && (sonar.current_range > CLOSER_POS))
#define POS_3 ((sonar.current_range < CLOSER_POS) && (sonar.current_range > MIDDLE_POS))
#define POS_2 ((sonar.current_range < MIDDLE_POS) && (sonar.current_range > ALMOST_POS))
#define POS_1 ((sonar.current_range < ALMOST_POS) && (sonar.current_range > NEAR_END  ))
#define TRUE 1
#define SONAR_LED 3
/*

           NEAR      ALMOST        MIDDLE      CLOSER       FAR
VASC-->     |     1     |      2     |     3     |     4     |          Road       Infinity

*/


struct sonar_data {
  short fd;
  unsigned short current_range, infinity;
  unsigned char pos[NUM_REGIONS];
  unsigned char last_pos;
};
 
extern struct sonar_data sonar;
extern unsigned char MIDDLE_POS, CLOSER_POS, ALMOST_POS;
extern unsigned char sonar_starting_up;

void initSonar(void);
void readSonar(void);
void updateSonar(void);
int approachDetected();

#endif

