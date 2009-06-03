#ifndef GPS_H_
#define GPS_H_

#include "arduino/WProgram.h"

#define ulong unsigned long

typedef struct {  //28 bytes, MAYBE delete
	char  messageID[6];	// $GPRMC
	float UTCtime;
	char  status;
	float latitude;
	char  lat_dir;
	float longitude;
	char  lon_dir;
	float speed;
	float course;
	unsigned long  date;
}_GPS_buffer;

typedef struct {   // data [potentially] sent to brain; integers are better!
	unsigned long time;
	unsigned long speed;
	unsigned long course;
	unsigned long latitude;
	unsigned long longitude;
}_GPS_package;

class _GPS{
public:
	typedef enum {GGA = 0, GLL, GSA, GSV, RMC, VTG} NMEA_types;
	typedef enum {time = 0, speed, course, latitude, longitude} data_types;

	_GPS();
	_GPS(long);
	~_GPS();

// user commands
inline byte    available    (void)  { return Serial3.available(); }
inline char    read         (void)  { return Serial3.read();      }

_GPS_package*  get          (void);
       ulong   get          (data_types);
	   void    send         (char *, byte);
	   void    request      (NMEA_types, byte, byte, boolean);
inline void    stop_feed    (NMEA_types x)                    {request(x,0,0,false);}
inline void    start_feed   (NMEA_types x, byte y)            {request(x,0,y,false);}
inline void    request_feed (NMEA_types x, byte y, boolean z) {request(x,0,y,z);    }
inline void    query        (NMEA_types x)					  {request(x,1,0,false);}

private:
	byte filter_input (void);
	byte package_data (void);
	void calc_checksum(void);
	void send         (void);

	char          buffer_in;	  //serial buffer
	char          buffer_out[25]; //serial buffer
	_GPS_buffer   GPS_buffer,  *pGPS_buffer;
	_GPS_package  GPS_package, *pGPS_package;

	typedef union {
		byte container;
		struct {
			byte upper : 4;
			byte lower : 4;
		}sigchar;
	}chksum;
};

extern _GPS GPS;

#endif /* GPS_H_ */
