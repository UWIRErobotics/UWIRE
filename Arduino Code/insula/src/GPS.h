#ifndef GPS_H_
#define GPS_H_

#include "arduino/WProgram.h"

#define ulong unsigned long

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

// 'raw' commands
inline byte    available    (void)  { return Serial3.available(); }
inline char    read         (void)  { return Serial3.read();      }

// user commands
       byte    fill         (void);
_GPS_package*  parse        (void);
       ulong   get          (data_types);
	   void    send         (char *, byte);
	   void    request      (NMEA_types, byte, byte, boolean);
inline void    stop_feed    (NMEA_types x)                    {request(x,0,0,false);}
inline void    start_feed   (NMEA_types x, byte y)            {request(x,0,y,false);}
inline void    request_feed (NMEA_types x, byte y, boolean z) {request(x,0,y,z);    }
inline void    query        (NMEA_types x)					  {request(x,1,0,false);}


private:
	byte package_data (void);
	void calc_checksum(void);
	void send         (void);

//  GPS buffers
	char          buffer_in [75];
	char          buffer_out[25];
   _GPS_package   GPS_package, *pGPS_package;

static const char seperator = ',';

	typedef union {
		byte container;
		struct {
			byte upper : 4;
			byte lower : 4;
		}sigchar;
	}nybble;
};

/** OBJECT 'DECLARATION' **/
      extern _GPS GPS;
/**************************/

/*************** Notes ***************
 * the structure of the GPS data is as follows:
struct GPS_buffer;{
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
}//28 bytes
 *
 *
*************************************/

#endif /* GPS_H_ */
