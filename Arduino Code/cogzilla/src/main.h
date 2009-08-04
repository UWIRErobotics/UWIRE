#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "arduino/MsTimer2.h"
#include "globals.h"
#include "vehicle_control.h"
#include "cmu_cam.h"
#include "cog_globals.h"
#include "matrix_math.cpp"
#include "arduino/VirtualWire.h"

/*****MAIN FUNCTIONS*****/
void setup();
void loop();
int  main();

void CLI();			//messages from user console
void CLI_RF();		//messages from user console

#endif /* MAIN_H_ */
