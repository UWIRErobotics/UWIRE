#ifndef GPS_H_
#define GPS_H_

#include <stddef.h>					//for "NULL"
#include "arduino/wiring.h"			//for data types
#include "arduino/HardwareSerial.h"	//for Serial
#include "globals.h"


class _GPS
{
public:
	typedef enum {GGA = 0, GLL, GSA, GSV, RMC, VTG} NMEA_types;
	typedef enum {time = 0, speed, course, latitude, longitude} data_types;

	_GPS(long);
	_GPS() {_GPS(4800);}
	~_GPS(){}

// 'raw' commands
inline byte    available    (void)  { return Serial3.available(); }
inline char    read         (void)  { return Serial3.read();      }

// receiving & parsing commands
        char*  fill         (void);
_GPS_package*  parse        (void);
unsigned long  get          (data_types);

// sending (and related) commands
       void    set_param    (long, byte, boolean, byte);
	   void    request      (NMEA_types, byte, byte, boolean);
	   void    calc_checksum(char *);
	   void    send         (char *);
inline void    send         (char *ptr, byte length)
{	for(int i = 0; i < length; i++, ptr++)
	{	Serial3.print(*ptr);
		Serial0.print (*ptr);}	//debugging only
}

inline void    query        (NMEA_types x)                     {request(x,1,0,false);}
inline void    stop_feed    (NMEA_types x)                     {request(x,0,0,false);}
inline void    start_feed   (NMEA_types x, byte y, boolean z)  {request(x,0,y,z);}
inline void    send         (void)                             {send(buffer_out, 25);}
inline void    calc_checksum(void)                             {calc_checksum(buffer_out);}


private:
	static const char seperator = ',';

//  GPS buffers
	char          buffer_in [75], *pbuffer_in;
	char          buffer_out[25], *pbuffer_out;
   _GPS_package   GPS_package,    *pGPS_package;

};

/** OBJECT 'DECLARATION' **/
      extern _GPS GPS;
/**************************/

#endif /* GPS_H_ */
