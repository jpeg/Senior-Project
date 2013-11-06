#ifndef COMMON_INCLUDES
#define COMMON_INCLUDES
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#endif

#ifndef MB1242_I2C
#define MB1242_I2C 0x70            // device 7 bit I2C address 
#define MB1242_READ 0xE1           // 8 bit i2c address, plus high R/W bit
#define MB1242_WRITE 0xE0
#define MB1242_RANGE_CMD 0x51      
#define MB1242_ADDRESS_CMD1 0xAA   // Address change sent in this sequence
#define MB1242_ADDRESS_CMD2 0xA5   // Address change 


struct sonar_data {
  short fd;
  unsigned short range;
};
 
extern struct sonar_data sonar;

void initSonar(void);
void readSonar(void);

#endif

