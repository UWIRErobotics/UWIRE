#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "globals.h"
#include "vehicle_control.h"


/*****MAIN FUNCTIONS*****/
void setup();
void loop();
int  main();

void ArduinoComm();	//messages from Insula
void CLI();			//messages from user console


#endif /* MAIN_H_ */
