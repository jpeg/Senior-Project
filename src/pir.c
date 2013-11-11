#include "pir.h"

//int motionDetected;

void initPIR(void) {
  pinMode(PIR, INPUT);  
  pinMode(MOTION_LED, OUTPUT);
}

int motionDetected(void){
  unsigned char moved;
  moved = digitalRead(PIR);
  if ( moved ){
    digitalWrite(MOTION_LED, 1);
    printf("MOTION_LED ON  **********************************************************\n");
    return 1;
  }
  else {
    digitalWrite(MOTION_LED, 0);
    printf("MOTION_LED OFF\n");
    return 0;
  }
 }