#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "arduino/VirtualWire.h"
#include "globals.h"
#include "ArduinoLink.h"
#include "GPS.h"
#include "URG04LX.h"
#include "I2C.h"

#define NUM_READINGS 2

double obs_distance[NUM_READINGS];
double obs_angle   [NUM_READINGS] = {2.35619,0.7853981};
double x_distance  [NUM_READINGS];
double y_distance  [NUM_READINGS];
double x_force     [NUM_READINGS];
double y_force     [NUM_READINGS];


/*****MAIN FUNCTIONS*****/
int  main();
void setup();
void check_msg();
void CLI(char *, uint8_t);
void Sonar_calc();
void GPS_print();
void RC_mode();


#endif /* MAIN_H_ */
