#include "GPS.h"

byte stop_feed(NMEA_types type)
{
	byte i = 0x00;    //counter
	chksum checksum;  //message checksum

	char temp[25] = {'$','P','S','R','F','1','0','3',',',
                     '0','0',',',
                     '0','0',',',
                     '0','0',',',
                     '0','1','*',
                     '0','0',13,10};

	for(i = 0; i < 25; i++)
		buffer_out[i] = temp[i];

	if     (GGA == type)	buffer_out[10] = '0';
	else if(GLL == type)	buffer_out[10] = '1';
	else if(GSA == type)	buffer_out[10] = '2';
	else if(GSV == type)	buffer_out[10] = '3';
	else if(RMC == type)	buffer_out[10] = '4';
	else if(VTG == type)	buffer_out[10] = '5';

//	calculate the checksum
	checksum.container = 0x00;


	for(i = 1; i <= 19; i++)
		checksum.container ^= buffer_out[i];

	if(checksum.sigchar.upper <= 0x9)
		buffer_out[22] = checksum.sigchar.upper + 48; //= ASCI code for the number
	else
		buffer_out[22] = checksum.sigchar.upper + 55; //=ASCI code for the letter!

	if(checksum.sigchar.lower <= 0x9)
		buffer_out[21] = checksum.sigchar.lower + 48; //= ASCI ode for the number
	else
		buffer_out[21] = checksum.sigchar.lower + 55; //=ASCI code for the letter!

	return 0xFF;
}

byte request_feed(NMEA_types type, byte rate, boolean chksum_enable)
{
	return 0;
}

