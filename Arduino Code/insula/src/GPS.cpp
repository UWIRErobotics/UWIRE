#include "GPS.h"


GPS::GPS(void)
{
	GPS(4800);
}

GPS::GPS(long baud)
{
	baudrate = baud;

	Serial3.begin(baud);
}

GPS::~GPS()
{
	// ??? don't think there's anything to do...
}

byte GPS::request_feed(NMEA_types type, byte rate, boolean chksum_enable)
{
//  message template
	char temp[25] = {'$','P','S','R','F','1','0','3',',',
                     '0','0',',',
                     '0','0',',',
                     '0','0',',',
                     '0','0','*',
                     '0','0',13,10};

	byte i = 0x0;
	for(; i < 25; i++)	//fill buffer with template
		buffer_out[i] = temp[i];


//  update for user-defined NMEA type
	if     (GGA == type)	buffer_out[10] = '0';
	else if(GLL == type)	buffer_out[10] = '1';
	else if(GSA == type)	buffer_out[10] = '2';
	else if(GSV == type)	buffer_out[10] = '3';
	else if(RMC == type)	buffer_out[10] = '4';
	else if(VTG == type)	buffer_out[10] = '5';

//	update for user-defined checksum
	if(chksum_enable)
		buffer_out[19] = '1';


//  update for user-defined update rate
	if      (rate <= 9)
		buffer_out[19] = rate + 48;
	else if (rate >= 10 && rate <= 99)
	{
		//update the rate to represent double-digit vals
	}


	calc_checksum();

	return 0xFF;
}

void GPS::calc_checksum()
{
	byte i = 0x0;
	chksum checksum;
	checksum.container = 0x00;

	for(i = 1; i <= 19; i++)
		checksum.container ^= buffer_out[i];


//  convert checksum into ASCII values, put lower byte first
	if(checksum.sigchar.upper <= 0x9)
		buffer_out[22] = checksum.sigchar.upper + 48; //= ASCI code for the number
	else
		buffer_out[22] = checksum.sigchar.upper + 55; //=ASCI code for the letter!

	if(checksum.sigchar.lower <= 0x9)
		buffer_out[21] = checksum.sigchar.lower + 48; //= ASCI ode for the number
	else
		buffer_out[21] = checksum.sigchar.lower + 55; //=ASCI code for the letter!
}
