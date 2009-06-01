#include "arduino/WProgram.h"

#ifndef GPS_H_
#define GPS_H_

class GPS{
public:
	GPS();
	GPS(long);
	~GPS();

	typedef enum{GGA = 0, GLL, GSA, GSV, RMC, VTG}NMEA_types;

	byte request_feed(NMEA_types, byte, boolean);
	byte stop_feed   (NMEA_types type)
	{
		return request_feed(type, 0, true);
	}

	void calc_checksum(void);

private:
	char buffer_in;
	char buffer_out[25];

	long baudrate;

	typedef union _chksum{
		byte container;
		struct _sigchar{
			byte upper : 4;
			byte lower : 4;
		}sigchar;
	}chksum;
};

#endif /* GPS_H_ */
