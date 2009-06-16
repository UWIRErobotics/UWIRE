#include "GPS.h"


_GPS::_GPS(void)
: HardwareSerial(&rx_buffer3, &UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UDR3, RXEN3, TXEN3, RXCIE3, UDRE3)
{//                                1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9
	char command_template[25]  = {'$','P','S','R','F','1','0','3',	    // output buffer's template:
								  ',','0','0',',','0','0',',','0',      // $PSRF103,00,00,00,00*00<CR><LF>
	                              '0',',','0','0','*','0','0', 13, 10}; //
//  initialise or blank all values
	for(int i = 0; i < 25; i++){
		buffer_out[i] = command_template[i];
		buffer_in[i]  = 0x0;}
	for(int i = 25; i < 75; i++)
		buffer_in[i]  = 0x0;

	pbuffer_in  = &buffer_in[0];
	pbuffer_out = &buffer_out[0];

	pGPS_package           = &GPS_package;
	GPS_package.time       = 0x0;
	GPS_package.speed      = 0x0;
	GPS_package.course     = 0x0;
	GPS_package.latitude   = 0x0;
	GPS_package.longitude  = 0x0;
}


char * _GPS::fill(void)
{
	byte i = 0x00;

	for(; available() > 0; i++)
		buffer_in[i] = read();

	flush();

	return pbuffer_in;
}


_GPS_package* _GPS::parse(void)
{
	byte index = 0x00;
	byte comma = 0x00;

	GPS_package.time       = 0x0; // reset all values, or else
	GPS_package.speed      = 0x0; // the old data gets multiplied
	GPS_package.course     = 0x0; // by 10 and it becomes garbage
	GPS_package.latitude   = 0x0; //
	GPS_package.longitude  = 0x0; //

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
					Serial0.println("Header mismatch!");
					return NULL;
				}
				break;
			}

			case 1://time
			{
				GPS_package.time *= 10;
				GPS_package.time += (buffer_in[index] - 48);
				break;
			}

			case 2://status
			{
				if('A' != buffer_in[index])
				{
					Serial0.println("Status invalid!");
					return NULL;
				}
				break;
			}

			case 3://latitude
			{
				GPS_package.latitude *= 10;
				GPS_package.latitude += (buffer_in[index] - 48);
				break;
			}

			case 4://latitude indicator
			{
				if('N' != buffer_in[index])
				{
					Serial0.println("Latitude heading incorrect!");
					return NULL;
				}
				break;
			}

			case 5://longitude
			{
				GPS_package.longitude *= 10;
				GPS_package.longitude += (buffer_in[index] - 48);
				break;
			}

			case 6://longitude indicator
			{
				if('W' != buffer_in[index])
				{
					Serial0.println("Longitude heading incorrect!");
					return NULL;
				}
				break;
			}

			case 7://speed
			{
				GPS_package.speed *= 10;
				GPS_package.speed += (buffer_in[index] - 48);
				break;
			}
			case 8://course
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


//$PSRF100,<protocol>,<baud>,<DataBits>,<StopBits>,<Parity>*CKSUM<CR><LF>
void _GPS::set_param(long baud, byte data_bits, boolean stop, byte parity)
{
	byte index        = 15; //set to location of ending comma
//  message header and protocol (only NMEA supported at this time)
	char message[30]  = {'$','P','S','R','F','1','0','0',',','1',','};

//  baud rate
	char baud_rate[5] = {'0','0','0','0','0'};
	switch(baud)
	{
		case 1200:
			baud_rate[0] = '1';	baud_rate[1] = '2';
			break;

		case 2400:
			baud_rate[0] = '2';	baud_rate[1] = '4';
			break;

		case 4800:
			baud_rate[0] = '4';	baud_rate[1] = '8';
			break;

		case 9600:
			baud_rate[0] = '9';	baud_rate[1] = '6';
			break;

		case 19200:
			baud_rate[0] = '1';
			baud_rate[1] = '9';
			baud_rate[2] = '2';
			index        = 16 ;
			break;

		case 38400:
			baud_rate[0] = '3';
			baud_rate[1] = '8';
			baud_rate[2] = '4';
			index        = 16 ;
			break;
	}

	for(int i = 11; i < index; i++)
		message[i] = baud_rate[i - 11];	//fill in numeric value
	message[index++] = ',';

//  update data bits (GPS only accepts 7 or 8 for NMEA)
	message[index++] = data_bits + 48;
	message[index++] = ',';


//  update stop bits
	if(stop)	message[index++] = '1';
	else		message[index++] = '0';
	message[index++] = ',';


//  update parity (0 = none, 1 = odd, 2 = even)
	message[index++] = parity + 48;

	message[index++]  = '*';

	calc_checksum(message);

	send(message);
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

	send(buffer_out, 25);
}


void _GPS::calc_checksum(char *ptr)
{
	nybble8 checksum;
	checksum.container = 0x00;

	if(*ptr == '$')	   ptr++;

	for(; *ptr != '*'; ptr++)
		checksum.container ^= *ptr;

//  convert checksum into ASCII values, put lower byte first
	ptr++;
	if(checksum.lower <= 0x9)
		*ptr = checksum.lower + 48; //= ASCI conversion for numbers
	else
		*ptr = checksum.lower + 55; //= ASCI conversion for letters

	ptr++;
	if(checksum.upper <= 0x9)
		*ptr = checksum.upper + 48;
	else
		*ptr = checksum.upper + 55;
}


/*SEND 'RAW' ARRAY OF CHARACTERS
 * parameters: (ptr) = address of first element in array
 * NOTE: this function assumes the end of the message was '*'
 *       and that the checksum HAS BEEN CALCULATED!!! if not,
 *       this function won't know where to stop!
 */
void _GPS::send(char *ptr)
{
#define GPSout(x)	write(x); Serial0.print(x);	//include 'Serial.print' only for debugging

//	print everything up to the checksum boundary
	for(; *ptr != '*'; ptr++)
	{
		GPSout(*ptr);
	}
	GPSout(*ptr);	//asterisk

//  print checksum
	ptr++; GPSout(*ptr);	//checksum1
	ptr++; GPSout(*ptr);	//checksum2

	GPSout(13);	//<CR>
	GPSout(10);	//<LF>
}

/** OBJECT DECLARATION **/
       _GPS GPS;
/************************/
