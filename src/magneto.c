// magneto.c
#include "magneto.h"

struct magnetometer_data mag;
unsigned char mag_starting_up;
unsigned char first_boot = 1;

void initMagneto(void){
  int i;
  printf("Magnetometer Initializing..........\n");
  if(first_boot){
    pinMode(MAG_LED, OUTPUT);
    if( (mag.fd = wiringPiI2CSetup(HMC5883L_I2C)) < 0 )
      printf ("I2C connection failed: %s\n", strerror(errno));
    
    if( (wiringPiI2CWriteReg8(mag.fd, HMC5883L_REG_A, HIGH_FREQ8)) < 0 )
      printf("Write to HMC5883L_REG_A failed: %s/n", strerror(errno));

    if( (wiringPiI2CWriteReg8(mag.fd, HMC5883L_REG_B, SENSOR_GAIN)) < 0 )
      printf("Write to HMC5883L_REG_B failed: %s/n", strerror(errno));
   
    if( (wiringPiI2CWriteReg8(mag.fd, HMC5883L_MODE_REG, CONTINUOUS)) < 0)
      printf ("Write to HMC5883L_MODE_REG failed: %s/n", strerror(errno));
    first_boot=0;
  }
  mag_starting_up = 1;
  mag.reading = 0;
  mag.average = 0;
  mag.deviation = 0;
  mag.magnitude = 0;
  mag.oldest_sample = 0;
  for(i = 0 ; i < MAX_SAMPLES ; i++){
    readMagneto();
  }
  updateMagneto();
  printf("Magnetometer Initialized...........\n");
  mag_starting_up = 0;
}


void readMagneto(void){
  // Each field mag.reading is stored in two separate bytes,
  // most significan byte and least significant byte, that
  // have to be combined to mean anything.

  unsigned char xmsb, xlsb, ymsb, ylsb,zmsb, zlsb;
  short x, y, z, i;
  int reading = 0;
  // Only concerned about the forward facing field.
  // This should give the greatest sensitivity and
  // awareness to the approaching vehicle.
  for(i = 0 ; i < 8 ; i++){
    xmsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_XMSB);
    xlsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_XLSB); 
    ymsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_YMSB);
    ylsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_YLSB);
    zmsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_ZMSB);
    zlsb = wiringPiI2CReadReg8(mag.fd, HMC5883L_ZLSB);
    //printf("X : %02x%02x    Y : %02x%02x    Z : %02x%02x\n"
    //           ,xmsb,xlsb      ,ymsb,ylsb      ,zmsb,zlsb);

    // This combines the two bytes

    x = ((xmsb << 8) & 0xff00) | (xlsb & 0x00ff);
    x = x;
    // mag.x = (int)x;
    y = ((ymsb << 8) & 0xff00) | (ylsb & 0x00ff);
    y = y;
    // mag.y = (int)y;
    z = ((zmsb << 8) & 0xff00) | (zlsb & 0x00ff);
    z = z;
    // mag.z = (int)z;

    reading += (int)z;
    delay(7);
  }  
  mag.reading = reading/8;
  // If we want more omnipresent awareness, we can let all axes contribute
  // to the detection and pay attention to thier combined magnitude
  
  // mag.magnitude = sqrt( (mag.x * mag.x) + (mag.y * mag.y) + (mag.z * mag.z) );
  
  /* This is the old way of keeping a running average without storing
     any samples. The count could grow forever though and I gave up
     on this method. */
  // mag.average += (mag.magnitude - mag.average)/mag.count++;

  if( !mag_starting_up){
    // printf("X: %4d    Y: %4d    Z: %4d    mag: %4d    old: %4d     ave: %4d     dev: %4d\n", 
    //            mag.x,     mag.y,     mag.z,  mag.magnitude, mag.oldest_sample, mag.average, mag.deviation);
  
    printf("\x1B[K\x1B[3Areading: %4d     old: %4d     ave: %4d     dev: %4d     min: %3d     max: %3d\n",
                mag.reading, mag.oldest_sample, mag.average, mag.deviation, mag.min, mag.max); 
  }
  // Replace the oldest_sample taken with the current
  if( mag_starting_up ){
    // store mag.z for straight ahead mag.readings, or mag.magnitude for omnidirectional
    mag.samples[mag.oldest_sample] = mag.reading;
    mag.oldest_sample++;
    mag.oldest_sample = mag.oldest_sample % MAX_SAMPLES;
  }
  else if( ( mag.low <= mag.reading && mag.reading <= mag.high ) ) {
      mag.samples[mag.oldest_sample] = mag.reading;
      mag.oldest_sample++;
      mag.oldest_sample = mag.oldest_sample % MAX_SAMPLES;
  }
  // mag.samples[mag.oldest_sample] = mag.reading;
  // mag.oldest_sample++;
  // mag.oldest_sample = mag.oldest_sample % MAX_SAMPLES;  
}

void updateMagneto(void) {
  // It shouldn't be necessary to come up with a new average every time
  // a mag.reading is stored. This lets it be done less frequently than mag.readings.
  long sum = 0;
  int difference = 0;
  int i;
  mag.min = 999;
  mag.max = -999;
  for( i = 0 ; i < MAX_SAMPLES ; i++ ){
    sum += mag.samples[i];
    if(mag.samples[i] < mag.min)
      mag.min = mag.samples[i];
    if(mag.samples[i] > mag.max)
      mag.max = mag.samples[i];
  }

  mag.average = sum/MAX_SAMPLES;
  sum = 0;

  for( i = 0 ; i < MAX_SAMPLES ; i++ ){
    difference = (mag.samples[i] - mag.average);
    sum += difference*difference;
  }

  mag.deviation = ceil(sqrt((double)sum/MAX_SAMPLES));
  mag.low  = mag.min - DEVIATION_SENSITIVITY*mag.deviation;
  mag.high = mag.max + DEVIATION_SENSITIVITY*mag.deviation;

  
  // printf("X: %4d    Y: %4d    Z: %4d    mag: %4d    old: %4d     ave: %4d     dev: %4d                     **********************************\n", 
  //            mag.x,     mag.y,     mag.z,  mag.magnitude, mag.oldest_sample, mag.average, mag.deviation);
}

int fieldDisruptionDetected(void){
  readMagneto();
  if( ( mag.low < mag.reading && mag.reading < mag.high ) ){
    digitalWrite(MAG_LED, 0);
    return 0;
  }  
  // else if(( (mag.low - DEVIATION_SENSITIVITY*mag.deviation ) > mag.reading || mag.reading > (mag.high + DEVIATION_SENSITIVITY*mag.deviation ) )){
  //   initMagneto();
  // } 
  else{
    digitalWrite(MAG_LED, 1);
    return 1;
  }
}
