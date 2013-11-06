#include "sonar.h"

struct sonar_data sonar;

void initSonar(void) {
    if( (sonar.fd = wiringPiI2CSetup(MB1242_I2C)) < 0 )
    printf ("I2C connection failed: %s\n", strerror(errno));
  sonar.range = 0xAAAA;
}

void readSonar(void) {
  unsigned short range = 0x1;
  if(wiringPiI2CWrite(sonar.fd, MB1242_RANGE_CMD) < 0)
    printf("Write to MB1242_I2C failed: %s/n", strerror(errno));  
  delay(200);
  printf("range: %04x    sonar.range: %04x\n", range, sonar.range);
  printf("range: %4d    sonar.range: %4d\n\n", range, sonar.range);  
  range = wiringPiI2CReadReg16(sonar.fd, MB1242_READ);
  sonar.range = (( range << 8 ) & 0xff00) | ( (range >> 8) & 0x00ff);
  printf("range: %04x    sonar.range: %04x\n", range, sonar.range);
  printf("range: %4d    sonar.range: %4d\n\n", range, sonar.range);
}
