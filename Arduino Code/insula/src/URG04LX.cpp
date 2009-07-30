#include "URG04LX.h"
#include <float.h>
#include <math.h>

/*********** LIDAR CONSTANTS *********/
#define MAX_DIST	 2500  //measured in [mm]
#define MIN_DIST     66	   //
#define LIDAR_FORCE  5000
/*************************************/


URG04LX::URG04LX()
       : HardwareSerial(&rx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, UDRE2)
{// use "MS..." for 2-bit encoding, "MD..." for 3-bit
	uint8_t msg[17] = {'M','S','0','1','2','8',	 //start step (128 = -90 degrees)
						    '0','6','4','0',	 //end step   (640 = +90 degrees)
						    '0','2','1',		 //cluster count, scan interval
						    '0','1',0xA,'\n'}; 	 //# of scans

	for(int i = 0; i < 17; i++)
		distance_msg[i] = msg[i];
}


void URG04LX::supertest (void)
{
	setSerial(OFF);

//	enable ranging
	laser(ON);

//	get & parse raw data
	uint16_t num_msr = distAcq();

//	calculate potential field
	BruteCalc(num_msr);

//	send force vectors to cogzilla
	send_force();

//	report forces for debugging
	Serial0.println();
	Serial0.print("Fx = ");	Serial0.println(cumulative_x, DEC);
	Serial0.print("Fy = ");	Serial0.println(cumulative_y, DEC);

	setSerial(ON);
}


uint16_t URG04LX::distAcq (void)
{
	uint8_t  header  = 0x00;	// <LF>'s in header
	uint8_t  lines   = 0x00;	// <LF>'s in data
	uint16_t bytecnt = 0x00;	// gross bytes
	uint16_t meascnt = 0x00;	// parsed bytes

//  send the "MD" command
	write(distance_msg, 16);

//  header (skip)
//  if a <LF> is found, check how many characters precede it;
//  5 characters means the time-stamp, and the end of header
	while(URG_counter < 128);

	for(uint8_t line_char = 0x0, flag = 0x0; flag != 0xFF; bytecnt++)
	{
		if		(URG_counter < bytecnt)		break;

		else if	( 0xA == URG_buffer[bytecnt] )
		{
			if(5 == line_char)	flag = 0xFF;

			header++;
			line_char = 0x0;
		}

		else	line_char++;
	}


//  data section
	while(URG_counter < 585);	//	incoming data is always 588 bytes

	for(; meascnt < 257; bytecnt+= 2)
	{
		if( (0xA == URG_buffer[bytecnt]) || (0xA == URG_buffer[bytecnt + 1]) )
			lines++;	//skip <LF> & 'sum' characters

		else
		{
			LidarData[meascnt] = 0;
			LidarData[meascnt] = ( ((URG_buffer[bytecnt] - 0x30) << 6) + (URG_buffer[bytecnt + 1] - 0x30) );

//			filter out bad useless (super small/large) data points
			if( (LidarData[meascnt] < MIN_DIST)||(MAX_DIST < LidarData[meascnt]) )
				LidarData[meascnt] = 0;

			meascnt++;
		}
	}

//  DEBUG only
	if(meascnt != 257)
	{
		Serial0.print("Received total = ");
		Serial0.println(URG_counter, DEC);
		Serial0.print("Total parsed   = ");
		Serial0.println(bytecnt, DEC);
		Serial0.print("Measurment tot = ");
		Serial0.println(meascnt, DEC);
		Serial0.print("# of lines     = ");
		Serial0.println(lines, DEC);

		Serial0.println();
	}

	return meascnt;
}


void URG04LX::BruteCalc(uint16_t num)
{
	double raw_distance = 0, angle   = 0;
	double x_force = 0, 	 y_force = 0;
	double incre =(4.0*M_PI / 1024.0);

//	reset force vectors
	cumulative_x = 0;
	cumulative_y = 0;

//  go through all objects...
	for(uint16_t i = 0; i <= num; i++, angle += incre)
	{
		if(0 != LidarData[i])
		{
			raw_distance = (float)LidarData[i];

			x_force = ( (LIDAR_FORCE / raw_distance) * cos(angle) );
			y_force = ( (LIDAR_FORCE / raw_distance) * sin(angle) );

			cumulative_x += (signed int)x_force;
			cumulative_y += (signed int)y_force;
		}
	}
}


void URG04LX::send_force (void)
{
	byte16 cogzilla_info;	cogzilla_info.container = 0;

//	fill x
	if 	 (255 < abs(cumulative_x))		cogzilla_info.high = 255;
	else								cogzilla_info.high = abs(cumulative_x);
//	fill y
	if 	 (255 < abs(cumulative_y))		cogzilla_info.low  = 255;
	else								cogzilla_info.low  = abs(cumulative_y);

//  send x, reverse what it should be (simplest fix)
	if(cumulative_x < 0)	Brain.write(FORCE_X_POS);
	else					Brain.write(FORCE_X_NEG);
	Brain.write(cogzilla_info.high);

//	send y, always (+)
	Brain.write(FORCE_Y);
	Brain.write(cogzilla_info.low);
}


void URG04LX::getInfo (uint8_t mode)
{
	uint8_t msg[4] = {'V','V',0x0A,'\n'};

	if( (1 == mode) || ('P' == mode) )
	{
		msg[0] = 'P';
		msg[1] = 'P';
	}

	else if( (2 == mode) || ('I' == mode) )
	{
		msg[0] = 'I';
		msg[1] = 'I';
	}

	write(msg, 3);
}


//if mode is 0, turn OFF the laser
void URG04LX::laser(uint8_t mode)
{
	uint8_t msg[4] = {'B','M',0x0A,'\n'};

	if(!mode)
	{	msg[0] = 'Q';
		msg[1] = 'T';	}

	write(msg, 3);
}

void URG04LX::baudRate(uint32_t baud)
{
   uint8_t command[10] = {'S','S','0','0','0','0','0','0',0x0A,'\n'};

   switch(baud)
   {
	   case 57600:
	   {
		   command[3] = '5';
		   command[4] = '7';
		   command[5] = '6';
		   break;
	   }

	   case 115200:
	   {
		   command[2] = '1';
		   command[3] = '1';
		   command[4] = '5';
		   command[5] = '2';
		   break;
	   }

	   case 250000:
	   {
		   command[2] = '2';
		   command[3] = '5';
		   break;
	   }

	   case 500000:
	   {
		   command[2] = '5';
		   break;
	   }

	   case 750000:
	   {
		   command[2] = '7';
		   command[3] = '5';
		   break;
	   }

	   default:
	   case 19200:
	   {
		   command[3] = '1';
		   command[4] = '9';
		   command[5] = '2';
		   break;
	   }
   }

   write(command, 9);
}


// refer to page (14/19) for meanings
void URG04LX::setMotor(uint16_t speed)
{
   uint8_t command[6] = {'C','R','0','0',0x0A,'\n'};

   switch(speed)
   {
	   case 0:	   case 1:	   case 2:	   case 3:	   case 4:
	   case 5:	   case 6:	   case 7:	   case 8:	   case 9:
	   {
		   command[3] = speed + 48;
		   break;
	   }

	   case 10:
	   {
		   command[2] = '1';
		   break;
	   }

	   case 99:
	   default:
	   {
		   command[2] = '9';
		   command[3] = '9';
		   break;
	   }
   }

   write(command, 5);
}


//just support requesting time, we don't even need that...
void URG04LX::timeInfo (void)
{
	uint8_t msg[5] = {'T','M','1',0xA,'\n'};

	write(msg, 4);
}


void URG04LX::reset(void)
{
	uint8_t msg[4] = {'R','S',0xA,'\n'};

	write(msg, 3);
}

void URG04LX::setSerial(uint8_t command)
{
//	turn regular serial ON
	if(command) {
		flush();
		Serialflag.flag2 = 0x0;
	}

//  turn regular serial OFF
	else {
		URG_counter      = 0x0;
		Serialflag.flag2 = 0x3;
	}
}

/** OBJECT DECLARATION **/
	   URG04LX Lidar;
/************************/
