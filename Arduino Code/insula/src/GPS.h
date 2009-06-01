#ifndef GPS_H_
#define GPS_H_

#include "arduino/WProgram.h"
#include "globals.h"

/***Defines***/
typedef enum{GGA = 0, GLL, GSA, GSV, RMC, VTG}NMEA_types;

typedef union _chksum{
	byte container;
	struct _sigchar{
		byte upper : 4;
		byte lower : 4;
	}sigchar;
}chksum;


/***Command Functions***/
byte stop_feed   (NMEA_types);
byte request_feed(NMEA_types, byte, boolean);


#endif /* GPS_H_ */
