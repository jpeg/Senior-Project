#include "sonar.h"

struct sonar_data sonar;
unsigned char MIDDLE_POS, CLOSER_POS, ALMOST_POS;
unsigned char sonar_starting_up;

void initSonar(void) {
  sonar_starting_up = 1;
  printf("Sonar Initializing.................\n");
  if( (sonar.fd = wiringPiI2CSetup(MB1242_I2C)) < 0 )
    printf ("I2C connection failed: %s\n", strerror(errno));
  if( softPwmCreate(SONAR_LED, 0, 100))
    printf ("pwmCread failed: %s\n", strerror(errno));
  
  sonar.current_range = 0;
  sonar.infinity = 0;
  sonar.last_pos = 0;

  MIDDLE_POS = (FAR_END + NEAR_END)/2;
  CLOSER_POS = (FAR_END + MIDDLE_POS)/2;
  ALMOST_POS = (MIDDLE_POS + NEAR_END)/2;
  updateSonar();
  sonar_starting_up = 0;
   printf("Sonar Initialized..................\n");
}

void readSonar(void) {
  unsigned short hex_range = 0x0;
  // Tells the sensor to take a range reading
  if(wiringPiI2CWrite(sonar.fd, MB1242_RANGE_CMD) < 0)
    printf("\n\n\nWrite to MB1242_I2C failed: %s\n\n\n\n", strerror(errno));  
  
  // Have to wait 10-50ms after giving the cmd to range before data is ready
  delay(60);
  
  // printf("range: %04x    sonar.range: %04x\n", range, sonar.range);
  // printf("range: %4d    sonar.range: %4d\n\n", range, sonar.range);  
  

  // sonar.last_range = sonar.current_range;

  hex_range = wiringPiI2CReadReg16(sonar.fd, MB1242_READ);

  // MSB and LSB are read in reversed. This swaps them.
  sonar.current_range = (( hex_range << 8 ) & 0xff00) | ((hex_range >> 8) & 0x00ff);
  
  // printf("range: %04x    sonar.range: %04x\n", range, sonar.range);
  // printf("range: %4d    sonar.range: %4d\n\n", range, sonar.range);
  if(!sonar_starting_up){
    printf("pos             [ %d ]            [ %d ]            [ %d ]            [ %d ]          last_pos: %d                               current_range:%3d\nNEAR_END: %3d   ALMOST_POS: %3d   MIDDLE_POS: %3d   CLOSER_POS: %3d   FAR_END: %3d                        infinity: %d\n",
            sonar.pos[1], sonar.pos[2], sonar.pos[3], sonar.pos[4], sonar.last_pos, sonar.current_range, NEAR_END, ALMOST_POS, MIDDLE_POS, CLOSER_POS, FAR_END, sonar.infinity );
  }
}


// Performs the decision making
// Returns true after an object has passed through 4 zones dividing the
// forward facing path.
int approachDetected(void) {
  int i;
  readSonar();
  // if the range reading isn't less than the maximum default
  // range measured at start up, can skip over subequent checks
  // I'm thinking this should aslo catch the cases when an object
  // comes part way, but leaves. The else{} will reset the pos
  // array and last_pos to start over next time
  if(sonar.current_range < sonar.infinity){
    // Macros replace the conditionals here to make this more readable. 
    // POS_4 is the end of the driveway. This is the only location
    // we're concerned about checking to begin with.
    if( POS_4 ){
      // Presence at each region is stored in the pos array. This
      // just keeps track to confirm the whole path has been taken
      // from end of drive way. The last_position is used to progress
      // to the next stage. 
      sonar.pos[4] = TRUE;
      sonar.last_pos = 4;
      softPwmWrite(SONAR_LED, 1);
    }
    if( sonar.last_pos == 4 ) {
      // If object moves closer, will reach pos 3. 
      // I don't think anything will need to be done if the object
      // stays in place. If object leaves, condition caught at
      // at beginning if() check. 
      if( POS_3 ){
        sonar.pos[3] = TRUE;
        sonar.last_pos = 3;
        softPwmWrite(SONAR_LED, 5);
      }
    }
    // Can progress only if pos 4 was reached, and
    // the last pos was 3
    if( sonar.pos[4] && (sonar.last_pos == 3) ){
      if( POS_2 ){
        sonar.pos[2] = TRUE;
        sonar.last_pos = 2;
        softPwmWrite(SONAR_LED, 10);
      }
    }
    // If pos 4 and pos 3 both reached and last_pos was 2
    // check to see if now at pos 1
    if( sonar.pos[4] && sonar.pos[3] && (sonar.last_pos == 2) ){
      if( POS_1 ){
        sonar.pos[1] = TRUE;
        // sonar.last_pos = 1;
        softPwmWrite(SONAR_LED, 100);
        // All points passed and final region reached
        // return affirmative. object is approaching
        return 1;
      }
    }
  }
  // To be here, the range reading must be equal to or greater
  // than the default infinite range. At this point nothing is
  // present to keep track of and all tracking data is reset
  else {
    for(i = 0; i < NUM_REGIONS ; i++)
      sonar.pos[i] = 0;
    sonar.last_pos = 0;
    softPwmWrite(SONAR_LED, 0);
  }
  // no confirmed object approaching
  return 0;
}


// Loops for TRAIN_SECONDS time to establish the farthest point in range.
void updateSonar(void){
  int i;
  for(i=0; i < 20*TRAIN_SECONDS; i++){
    readSonar();
    if(sonar.current_range > sonar.infinity)
      sonar.infinity = sonar.current_range;
  }  
}





