#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>


#define MAGNETO 0x1e            // device I2C address
#define REG_A 0x00              // data output rate & measurement config
#define REG_B 0x01              // gain
#define MODE_REG 0x02           // operating mode
#define XMSB 0x03
#define XLSB 0x04
#define YMSB 0x07
#define YLSB 0x08
#define ZMSB 0x05
#define ZLSB 0x06
#define STATUS_REG 0x09

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

int magnetoInit(void);
void readMagneto(int fd, short *data);

int magnetoInit(void){
  int fd;
  if( (fd = wiringPiI2CSetup(MAGNETO)) < 0 ){
    printf ("I2C connection failed: %s\n", strerror(errno));
    return -1;
  }
  if( (wiringPiI2CWriteReg8(fd, REG_A, HIGH_FREQ8)) < 0 ){
    printf("Write to REG_A failed: %s/n", strerror(errno));
    return -1;
  }
  if( (wiringPiI2CWriteReg8(fd, REG_B, SENSOR_GAIN)) < 0 ){
    printf("Write to REG_B failed: %s/n", strerror(errno));
    return -1;
  }
  if( (wiringPiI2CWriteReg8(fd, MODE_REG, CONTINUOUS)) < 0){
    printf ("Write to MODE_REG failed: %s/n", strerror(errno));
    return -1;
  }
  return fd;
}


void readMagneto(int fd, short *data) {
  // int error = 0;
  unsigned char xmsb, xlsb, ymsb, ylsb, zmsb, zlsb;
  xmsb = wiringPiI2CReadReg8(fd, XMSB);
  xlsb = wiringPiI2CReadReg8(fd, XLSB); 
  ymsb = wiringPiI2CReadReg8(fd, YMSB);
  ylsb = wiringPiI2CReadReg8(fd, YLSB);
  zmsb = wiringPiI2CReadReg8(fd, ZMSB);
  zlsb = wiringPiI2CReadReg8(fd, ZLSB);
  printf("X : %02x%02x    Y : %02x%02x    Z : %02x%02x\n", xmsb,xlsb,ymsb,ylsb,zmsb,zlsb);

  data[0]     = ((xmsb << 8) & 0xff00) | (xlsb & 0x00ff);
  *(data + 1) = ((ymsb << 8) & 0xff00) | (ylsb & 0x00ff);
  *(data + 2) = ((zmsb << 8) & 0xff00) | (zlsb & 0x00ff);
  printf("Xd: %4d    Yd: %4d    Zd: %4d\n\n", *data, *(data+1), *(data+2));

  // if(error < 0){
  //   printf ("Reading magnetometer failed: %s\n", strerror(errno));
    //return -1;
  // }
}

int main(){
  int fd;
  short data[3];
  int junk;
  // int i = 0;
  //int x, y, z, data;
  if( (junk = wiringPiSetup()) < 0){
    // printf ("wiringPiSetup failed. What a piece: %s\n", strerror(errno));
    return 1;
  }
  fd = magnetoInit();
  while(1==1){
    readMagneto(fd, data);
    //printf("%3d   X: %-3.1f  Y: %-3.1f  Z: %-3.1f\n",i++ ,(float)data[0]*2.5, (float)data[1]*2.5, (float)data[2]*2.5);
    delay(250);
  }

  return 0;
}
