#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "globals.h"

//#include "ArduinoLink.h"
#include "GPS.h"
#include "I2C.h"


/*****MAIN FUNCTIONS*****/
void setup();
void loop();
int  main();

void CLI(char *);


#endif /* MAIN_H_ */
