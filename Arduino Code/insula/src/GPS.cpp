#include "GPS.h"

_GPS GPS(4800);	//object we will be using

_GPS::_GPS(void)
{
	_GPS(4800);
}

_GPS::_GPS(long baud)
{//								   1   2   3   4   5   6   7   8   9   20
	char command_template[25]  = {'$','P','S','R','F','1','0','3',	 // output buffer's template
								  ',','0','0',',','0','0',',','0',   // $PSRF103,00,00,00,00*00<CR><LF>
	                              '0',',','0','0','*','0','0', 13,10};
//  initialise or blank all values
	for(int i = 0; i < 25; i++){
		buffer_out[i] = command_template[i];
		buffer_in[i]  = 0x0;}
	for(int i = 25; i < 75; i++)
		buffer_in[i]  = 0x0;

	GPS_package.time       = 0x0;
	GPS_package.speed      = 0x0;
	GPS_package.course     = 0x0;
	GPS_package.latitude   = 0x0;
	GPS_package.longitude  = 0x0;
	pGPS_package           = &GPS_package;

//  begin serial connection
	Serial3.begin(baud);
}

_GPS::~_GPS()	{}

byte _GPS::fill(void)
{
	byte i = 0x00;

	while(Serial3.available() > 0)
	{
		buffer_in[i] = Serial3.read();
		i++;
	}

	return i;	//return # of characters (size of buffer in bytes)
}

_GPS_package* _GPS::parse(void)
{
	byte index = 0x00;
	byte comma = 0x00;

	do
	{
		if(seperator == buffer_in[index])	comma++;
		else if ('.' != buffer_in[index])	//ignore decimals
		{
			switch(comma)
			{
			case 0 :	//message ID
			{
				char header[6] = {'$','G','P','R','M','C'};
				if(buffer_in[index] != header[index])
				{
					Serial.println("Header mismatch!");
					return NULL;
				}
				break;
			}

			case 1:		//time
			{
				GPS_package.time *= 10;
				GPS_package.time += (buffer_in[index] - 48);
				break;
			}

			case 2:		//status
			{
				if('A' != buffer_in[index])
				{
					Serial.println("Status invalid!");
					return NULL;
				}
				break;
			}

			case 3:		//latitude
			{
				GPS_package.latitude *= 10;
				GPS_package.latitude += (buffer_in[index] - 48);
				break;
			}

			case 4:		//latitude indicator
			{
				if('N' != buffer_in[index])
				{
					Serial.println("Latitude heading incorrect!");
					return NULL;
				}
				break;
			}

			case 5:		//longitude
			{
				GPS_package.longitude *= 10;
				GPS_package.longitude += (buffer_in[index] - 48);
				break;
			}

			case 6:		//longitude indicator
			{
				if('W' != buffer_in[index])
				{
					Serial.println("Longitude heading incorrect!");
					return NULL;
				}
				break;
			}

			case 7:		//speed
			{
				GPS_package.speed *= 10;
				GPS_package.speed += (buffer_in[index] - 48);
				break;
			}
			case 8:		//course
			{
				GPS_package.course *= 10;
				GPS_package.course += (buffer_in[index] - 48);
				break;
			}

			case 9:		//date
			case 10:	//magnetic variation
			case 11:    //checksum (disabled)
			default:
			{
				return pGPS_package;
				break;
			}
			}//switch...case

		}//separator != buffer_in[index]

		index++;
	} while(comma <= 8);

	return pGPS_package;
}

unsigned long _GPS::get(data_types type)
{
	switch (type)
	{
	case time:
		return GPS_package.time;
		break;
	case speed:
		return GPS_package.speed;
		break;
	case course:
		return GPS_package.course;
		break;
	case latitude:
		return GPS_package.latitude;
		break;
	case longitude:
		return GPS_package.longitude;
		break;
	}

	return 0;	//should never get here!
}

void _GPS::send(char *ptr, byte length)
{
	for(int i = 0; i < length; i++)
		Serial3.print(*ptr + i);
}

void _GPS::request(NMEA_types type, byte mode, byte rate, boolean chksum)
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
	if(chksum)
		buffer_out[19] = '1';
	else
		buffer_out[19] = '0';

	calc_checksum();

	send();
}

void _GPS::calc_checksum()
{
	nybble checksum;
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

byte _GPS::package_data(void)
{
	return 0;	//TODO: this!
}

void _GPS::send(void)
{
	Serial3.print(buffer_out);

	Serial.print(buffer_out);	//just for debugging
}
