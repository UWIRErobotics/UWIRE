#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "arduino/VirtualWire.h"
#include "arduino/globals.h"
#include "GPS.h"
#include "URG04LX.h"
#include "I2C.h"


/*****MAIN FUNCTIONS*****/
void CLI		(char *, uint8_t);
void check_msg	();
void RC_mode	(){}

void Sonar_calc	();
void GPS_send   ();

void setup		();
int  main		();



#endif /* MAIN_H_ */
