#include "arduino/WProgram.h"

#ifndef GPS_H_
#define GPS_H_

class GPS{
public:
	typedef enum {GGA = 0, GLL, GSA, GSV, RMC, VTG} NMEA_types;

	GPS();
	GPS(long);
	~GPS();

// user commands
   byte request_feed    (NMEA_types, byte, boolean);
#define stop_feed (x)    request_feed(x,0,false)
#define start_feed(x, y) request_feed(x,y,false)

//  data packaging / manipulation
	byte filter_input();

private:
	void calc_checksum(void);

	long baudrate;	// redundant / useless?
	char buffer_in;
	char buffer_out[25];

	typedef union {
		byte container;
		struct {
			byte upper : 4;
			byte lower : 4;
		}sigchar;
	}chksum;

	typedef struct {
		char  messageID[6];	// $GPRMC
		float UTCtime;
		char  status;
		float latitude;
		char  lat_dir;
		float longitude;
		char  lon_dir;
		float speed;
		float course;
		long  date;
	} RMC_buffer;
};


#endif /* GPS_H_ */
