#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "arduino/VirtualWire.h"
#include "globals.h"
#include "ArduinoLink.h"
#include "GPS.h"
#include "URG04LX.h"
#include "I2C.h"



/*****MAIN FUNCTIONS*****/
int  main();
void setup();
void check_msg();
void check_GPS();
void CLI(char *, uint8_t);
void RC_mode();



#endif /* MAIN_H_ */
