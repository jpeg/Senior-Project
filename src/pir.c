// pir.c

#include "pir.h"

void initPIR(void) {
  pinMode(PIR, INPUT);  
  pinMode(MOTION_LED, OUTPUT);
}

int motionDetected(void){
  unsigned char moved;
  moved = digitalRead(PIR);
  delay(1);
  digitalWrite(MOTION_LED, moved);
  return moved;
 }