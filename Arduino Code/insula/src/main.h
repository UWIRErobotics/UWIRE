#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "globals.h"

typedef enum{GGA = 0, GLL, GSA, GSV, RMC, VTG}NMEA_types;

typedef union _chksum{
	byte container;
	struct _sigchar{
		byte upper : 4;
		byte lower : 4;
	}sigchar;
}chksum;


/*****MAIN FUNCTIONS*****/
void setup();
void loop();
int  main();


/*****GPS FUNCTIONS*****/
byte request_feed(NMEA_types, byte, boolean);
byte stop_feed   (NMEA_types type)
{
	return request_feed(type, 0, true);
}

#endif /* MAIN_H_ */
