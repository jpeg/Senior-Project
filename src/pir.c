// pir.c

#include "pir.h"


//int motionDetected;

void initPIR(void) {
  pinMode(PIR, INPUT);  
  pinMode(MOTION_LED, OUTPUT);
}

int motionDetected(void){
  unsigned char moved;
  moved = digitalRead(PIR);
  digitalWrite(MOTION_LED, moved);
  return moved;
 }