#include "magneto.h"

struct magnetometer_data mag;


void initMagneto(void){

  if( (mag.fd = wiringPiI2CSetup(HMC5883L_I2C)) < 0 )
    printf ("I2C connection failed: %s\n", strerror(errno));
  
  if( (wiringPiI2CWriteReg8(mag.fd, HMC5883L_REG_A, HIGH_FREQ8)) < 0 )
    printf("Write to HMC5883L_REG_A failed: %s/n", strerror(errno));

  if( (wiringPiI2CWriteReg8(mag.fd, HMC5883L_REG_B, SENSOR_GAIN)) < 0 )
    printf("Write to HMC5883L_REG_B failed: %s/n", strerror(errno));
 
  if( (wiringPiI2CWriteReg8(mag.fd, HMC5883L_MODE_REG, CONTINUOUS)) < 0)
    printf ("Write to HMC5883L_MODE_REG failed: %s/n", strerror(errno));

  mag.x = 0;
  mag.y = 0;
  mag.z = 0;
  mag.average = 0;
  mag.deviation = 0;
  mag.magnitude = 0;
  mag.count = 1;
  // for(mag.i = 0 ; mag.i < 1000 ; mag.i++){
  //   samples[mag.i]=0;
  // }
  // mag.i=0;

}


void readMagneto(void) {
  // int error = 0;
  unsigned char xmsb, xlsb, ymsb, ylsb, zmsb, zlsb;
  short x, y, z;
  // int i;
  xmsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_XMSB);
  xlsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_XLSB); 
  ymsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_YMSB);
  ylsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_YLSB);
  zmsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_ZMSB);
  zlsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_ZLSB);
  //printf("X : %02x%02x    Y : %02x%02x    Z : %02x%02x\n", xmsb,xlsb,ymsb,ylsb,zmsb,zlsb);

  x = ((xmsb << 8) & 0xff00) | (xlsb & 0x00ff);
  y = ((ymsb << 8) & 0xff00) | (ylsb & 0x00ff);
  z = ((zmsb << 8) & 0xff00) | (zlsb & 0x00ff);
  mag.x = (int)x; mag.y = (int)y; mag.z = (int)z;
  mag.magnitude = sqrt( (mag.x * mag.x) + (mag.y * mag.y) + (mag.z * mag.z) );
  mag.average += (mag.magnitude - mag.average)/mag.count++;
  

  printf("Xd: %4d    Yd: %4d    Zd: %4d    mag:%4d    ave: %4d\n\n", 
             mag.x,     mag.y,      mag.z,  mag.magnitude, mag.average);

  // if(error < 0){
  //   printf ("Reading magnetometer failed: %s\n", strerror(errno));
    //return -1;
  // }
}