#include "pir.h"

//int motionDetected;

void initPIR(void) {
  pinMode(PIR, INPUT);  
  pinMode(MOTION_LED, OUTPUT);  
}

void readPIR(void){ 
  if (digitalRead(PIR))
    digitalWrite(MOTION_LED, 1);  
  else
    digitalWrite(MOTION_LED, 0);  
 }