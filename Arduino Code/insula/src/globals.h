#ifndef GLOBALS_H_
#define GLOBALS_H_


/********** MESSAGE HEADERS **********/
#define ERROR          0x00
#define ArduinoMSG     0xFF
#define ArduinoCMD     0x66

#define RemoteControl  0x55

#define Sonar1         0x71	// all 'Sonar#' def's are
#define Sonar2         0x72	// also their respective
#define Sonar3         0x74	// 7-bit I2C addresses
#define Sonar4         0x78	//

#define Radar1         0x92 //analog IR readings
#define Radar2         0x94
#define Radar3         0x98

#define GPS_time       0xA0
#define GPS_speed      0xA1
#define GPS_course     0xA2
#define GPS_latitude   0xA4
#define GPS_longitude  0xA8
#define GPS_RAW        0xAF	//doubt this one will ever be used

#define FORCE_HEADER_X			0xF1
#define FORCE_HEADER_Y			0xF2

/********** UNIONS & STRUCTS **********/
typedef union
{
	uint8_t container;
	struct {
		uint8_t upper : 4;
		uint8_t lower : 4;
	};
}nybble8;

typedef union
{
	uint16_t container;
	struct{
		uint8_t high;
		uint8_t low;
	};
}byte16;

typedef union
{
	uint32_t container;
	struct{
		uint8_t highest;
		uint8_t high;
		uint8_t low;
		uint8_t lowest;
	};
}byte32;


typedef struct
{
	uint32_t time;		  //max value = 235959000
	uint16_t speed;
	uint16_t course;	  //max value = 35999   **NOTE, comes in 4 OR 5 digits...
	uint32_t latitude;
	uint32_t longitude;
	uint16_t pos_fix;
	uint16_t sats_used;
	uint16_t HDOP;
	uint16_t altitude;
}_GPS_package;


/*******************GLOBALLY USED VARIABLES************/

#define FORCE_CONST 24000

#endif /* GLOBALS_H_ */
