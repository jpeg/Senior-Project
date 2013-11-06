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

#ifndef HMC5883L_I2C
#define HMC5883L_I2C 0x1e            // device I2C address
#define HMC5883L_REG_A 0x00          // data output rate & measurement config
#define HMC5883L_REG_B 0x01          // gain
#define HMC5883L_MODE_REG 0x02       // operating mode
#define HMC5883L_XMSB 0x03
#define HMC5883L_XLSB 0x04
#define HMC5883L_YMSB 0x07
#define HMC5883L_YLSB 0x08
#define HMC5883L_ZMSB 0x05
#define HMC5883L_ZLSB 0x06
#define HMC5883L_STATUS_REG 0x09

#define HIGH_FREQ 0x18          // 1 sample averaged, 75Hz
#define HIGH_FREQ8 0x78         // 8 sample average, 75Hz
#define LOW_FREQ 0x00           // 1 sample averaged, 0.75Hz
#define CONTINUOUS 0x00         // Constantly reads and stores measurement

//#define SENSOR_GAIN 0x00  // +/- 0.88 Ga
#define SENSOR_GAIN 0x20  // +/- 1.3 Ga (default)
//#define SENSOR_GAIN 0x40  // +/- 1.9 Ga
//#define SENSOR_GAIN 0x60  // +/- 2.5 Ga
//#define SENSOR_GAIN 0x80  // +/- 4.0 Ga
//#define SENSOR_GAIN 0xA0  // +/- 4.7 Ga
//#define SENSOR_GAIN 0xC0  // +/- 5.6 Ga
//#define SENSOR_GAIN 0xE0  // +/- 8.1 Ga (not recommended)


struct magnetometer_data {
  short fd, gain, i;
  int x, y, z, average, deviation, magnitude;
 // int samples[1000];
  long count;
};
 
extern struct magnetometer_data mag;

void initMagneto(void);
void readMagneto(void);

#endif