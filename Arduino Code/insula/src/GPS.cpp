#include "GPS.h"

_GPS GPS(4800);	//object we will be using

_GPS::_GPS(void)
{
	_GPS(4800);
}

_GPS::_GPS(long baud)
{
	Serial3.begin(baud);

	pGPS_buffer  = &GPS_buffer;
	pGPS_package = &GPS_package;

//fill output buffer with command template '$PSRF103,00,00,00,00*00<CR><LF>'
	char command_template[26]  = {'$','P','S','R','F','1','0','3',',','0','0',',',
	                              '0','0',',','0','0',',','0','0','*','0','0',13,10};
	for(int i = 0; i < 25; i++)
		buffer_out[i] = command_template[i];
}

_GPS::~_GPS()
{
	// ??? don't think there's anything to do...
}

void _GPS::request(NMEA_types type, byte mode, byte rate, boolean chksum_enable)
{
//  update template with user-defined NMEA type
	if     (GGA == type)	buffer_out[10] = '0';
	else if(GLL == type)	buffer_out[10] = '1';
	else if(GSA == type)	buffer_out[10] = '2';
	else if(GSV == type)	buffer_out[10] = '3';
	else if(RMC == type)	buffer_out[10] = '4';
	else if(VTG == type)	buffer_out[10] = '5';

//  update for user-defined mode
	buffer_out[13] = mode + 48;

//  update for user-defined update rate (TODO: modify to allow rates > 10)
	buffer_out[16] = rate + 48;

//	update user-defined checksum status
	if(chksum_enable)
		buffer_out[19] = '1';
	else
		buffer_out[19] = '0';

	calc_checksum();

	send();
}

_GPS_package* _GPS::get(void)
{
	byte status = filter_input();

	if(0 == status)	return NULL;
	else			return pGPS_package;
}

unsigned long _GPS::get(data_types type)
{
	return 0;	//TODO:  this!
}


void _GPS::calc_checksum()
{
	chksum checksum;
	checksum.container = 0x00;

	byte i;
	for(i = 1; i <= 19; i++)
		checksum.container ^= buffer_out[i];

//  convert checksum into ASCII values, put lower byte first
	if(checksum.sigchar.lower <= 0x9)
		buffer_out[21] = checksum.sigchar.lower + 48; //= ASCI conversion for numbers
	else
		buffer_out[21] = checksum.sigchar.lower + 55; //= ASCI conversion for letters

	if(checksum.sigchar.upper <= 0x9)
		buffer_out[22] = checksum.sigchar.upper + 48;
	else
		buffer_out[22] = checksum.sigchar.upper + 55;
}


byte _GPS::filter_input(void)
{
	return 0;	//TODO: this!
}

byte _GPS::package_data(void)
{
	return 0;	//TODO: this!
}

void _GPS::send(void)
{
	Serial3.print(buffer_out);

	Serial.print(buffer_out);	//just for debugging
}
