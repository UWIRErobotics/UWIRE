#include "URG04LX.h"
#include <float.h>
#include <math.h>

#define MAX_DIST	4000
#define MIN_DIST    100
#define THRESHOLD   50


URG04LX::URG04LX()
       : HardwareSerial(&rx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, UDRE2)
{// use "MS..." for 2-bit encoding, "MD..." for 3-bit
	char msg[17] = {'M','S','0','1','2','8',	 //start step (128 = -90 degrees)
						    '0','6','4','0',	 //end step   (640 = +90 degrees)
						    '0','2','1',		 //cluster count, scan interval
						    '0','1',0xA,'\n'}; 	 //# of scans
	for(int i = 0; i < 17; i++)
		distance_msg[i] = msg[i];

	for(int i = 0; i < 128; i++)
	{
		objects[i].start = 0;
		objects[i].width = 0;
	}
}


byte32 URG04LX::supertest (void)
{
	laser(1);	//enable ranging

	uint16_t num_msr = 0;
//	uint8_t  num_obj = 0;

	num_msr = distAcq();

/*	num_obj = ObjectFilter(num_msr);

	for(uint8_t i = 0; i <= num_obj; i++)
	{
		Serial0.print("Obj start: ");
		Serial0.print(objects[i].start, DEC);
		Serial0.print("\t Width: ");
		Serial0.print(objects[i].width, DEC);
		Serial0.print("\t distance: ");
		Serial0.println(LidarData[objects[i].start]);
	}	*/

	laser(0);	//disable ranging

	return BruteCalc(num_msr);
}


uint16_t URG04LX::distAcq (void)
{
	uint8_t  lines   = 0x00;	// counts <LF>'s
	uint16_t bytecnt = 0x00;	// gross bytes
	uint16_t meascnt = 0x00;	// parsed bytes

//  turn off regular serial reading
	URG_counter      = 0x0;
	Serialflag.flag2 = 0x3;

	write(distance_msg);

	while(URG_counter < 575);	//	incoming data is always 580 bytes
/*	Serial0.println("done waiting!");	*/


//  header (skip), ALWAYS 48 BYTES
	for(; lines < 7; bytecnt++)
		if( 0xA == URG_buffer[bytecnt] )	lines++;

//  data section
	//for(; bytecnt < URG_counter; bytecnt+= 2)	//URG_counter should be 588!
	for(; lines < 19; bytecnt+= 2)				//should always receive 19 lines
	{
		if( (0xA == URG_buffer[bytecnt]) || (0xA == URG_buffer[bytecnt + 1]) )
			lines++;	//skip <LF> & 'sum' characters

		else
		{
			LidarData[meascnt] = 0;
			LidarData[meascnt] = ( ((URG_buffer[bytecnt] - 0x30) << 6) + (URG_buffer[bytecnt + 1] - 0x30) );

//			filter out bad useless (super small/large) data points
			if( (LidarData[meascnt] <= MIN_DIST) || (MAX_DIST < LidarData[meascnt]) )
				LidarData[meascnt] = 0;

			meascnt++;
		}
	}///for loop

	if(bytecnt < 588)
	{
		Serial0.print("Received total = ");
		Serial0.println(URG_counter, DEC);
		Serial0.print("Total parsed   = ");
		Serial0.println(bytecnt, DEC);
		Serial0.print("Measurment tot = ");
		Serial0.println(meascnt, DEC);
		Serial0.print("# of lines     = ");
		Serial0.println(lines, DEC);
	}

	Serialflag.flag2 = 0x0;	//turn ring-buffer back on

	return meascnt;
}


uint8_t URG04LX::ObjectFilter(uint16_t cnt)
{/*	Serial0.println("Entering Object filtering...");	*/

	uint8_t obj = 0; // object index
	int 	back_diff;

	for(uint16_t i = 1; i <= cnt; i++)	//first and last measurements get special treatment
	{
		back_diff = LidarData[ i ] - LidarData[i-1];

//		this data point is a continuation of previous data point
		if ( abs(back_diff) <= THRESHOLD )
			objects[obj].width++;

//		if not part of previous data point, make sure this data isn't threshold
		else if( (0 != LidarData[i]) && (LidarData[i] < 3000) )
		{
		//  if previous object is only 1 point wide,
		//  overwrite. Otherwise, start a new object
			if(1 != objects[obj].width)		obj++;

			objects[obj].start = i;
			objects[obj].width = 1;
		}
	}//1st pass filter

	return obj;
}


byte32 URG04LX::ForceCalc(uint16_t num)
{
//  temp variables
	double x_distance, y_distance, raw_distance;
	double x_force,    y_force;

//	angle measurment; 0[deg] is to our right, and we start one step behind that
	double 	incre  = (4.0*M_PI / 1024.0),  // (2)*[2PI/1024] increment
			inital = -incre;

//	reset force vectors
	cumulative_x = 0;
	cumulative_y = 0;

//  go through all objects...
	for(uint8_t i = 0; i <= num; i++)
	{
		uint8_t start = objects[i].start;
		double  angle = inital + incre*(double)start;

/*		Serial0.print("Angle: ");	Serial0.print(angle);
		Serial0.print(" \t");		Serial0.println(objects[i].width, DEC);		*/

//		go through all data points in this object...
		for(uint8_t j = 0; j <= objects[i].width; j++)
		{
			raw_distance = (float) LidarData[start + j];
			x_distance   = raw_distance * cos(angle + (double)j*incre);
			y_distance   = raw_distance * sin(angle + (double)j*incre);

			x_force = ( (LIDAR_FORCE / raw_distance) * (x_distance / raw_distance) );
			y_force = ( (LIDAR_FORCE / raw_distance) * (y_distance / raw_distance) );
		}

		cumulative_x += (signed int)x_force;
		cumulative_y += (signed int)y_force;
	}

/*	Serial0.println();
	Serial0.print("Fx = ");	Serial0.println(cumulative_x, DEC);
	Serial0.print("Fy = ");	Serial0.println(cumulative_y, DEC);	*/

	byte32 cogzilla_info;	cogzilla_info.container = 0;
//	fill x
	if 		(cumulative_x < -255)						cogzilla_info.highest = 255;
	else if	(cumulative_x > -255 && cumulative_x < 0)	cogzilla_info.highest = abs(cumulative_x);
	else if	(cumulative_x > 0 && cumulative_x < 255)	cogzilla_info.high 	  = cumulative_x;
	else /* (cum_force_X > 255) */						cogzilla_info.high 	  = 255;
//	fill y
	if 		(cumulative_y < -255)						cogzilla_info.low 	 = 255;
	else if	(cumulative_y > -255 && cumulative_y < 0)	cogzilla_info.low 	 = abs (cumulative_y);
	else if	(cumulative_y > 0 && cumulative_y < 255)	cogzilla_info.lowest = cumulative_y;
	else /* (cumulative_y > 255) */						cogzilla_info.lowest = 255;

	return cogzilla_info;
}


byte32 URG04LX::BruteCalc(uint16_t num)
{

//  temp variables
	double x_distance, y_distance, raw_distance;
	double x_force,    y_force;

//	angle measurement; 0[deg] is to our right, and we start one step behind that
	double 	incre  = (4.0*M_PI / 1024.0),  // (2)*[2PI/1024] increment
			angle = -incre;

//	reset force vectors
	cumulative_x = 0;
	cumulative_y = 0;

//  go through all objects...
	for(uint16_t i = 0; i <= num; i++, angle += incre)
	{
		if(LidarData[i] != 0)
		{
			raw_distance = (float) LidarData[i];
			x_distance   = raw_distance * cos(angle);
			y_distance   = raw_distance * sin(angle);

			x_force = ( (BRUTE_FORCE / raw_distance) * (x_distance / raw_distance) );
			y_force = ( (BRUTE_FORCE / raw_distance) * (y_distance / raw_distance) );

			cumulative_x += (signed int)x_force;
			cumulative_y += (signed int)y_force;
		}
	}

/*	Serial0.println();
	Serial0.print("Fx = ");	Serial0.println(cumulative_x, DEC);
	Serial0.print("Fy = ");	Serial0.println(cumulative_y, DEC);	*/

	byte32 cogzilla_info;	cogzilla_info.container = 0;
//	fill x
	if 		(cumulative_x < -255)						cogzilla_info.highest = 255;
	else if	(cumulative_x > -255 && cumulative_x < 0)	cogzilla_info.highest = abs(cumulative_x);
	else if	(cumulative_x > 0 && cumulative_x < 255)	cogzilla_info.high 	  = cumulative_x;
	else /* (cum_force_X > 255) */						cogzilla_info.high 	  = 255;
//	fill y
	if 		(cumulative_y < -255)						cogzilla_info.low 	 = 255;
	else if	(cumulative_y > -255 && cumulative_y < 0)	cogzilla_info.low 	 = abs (cumulative_y);
	else if	(cumulative_y > 0 && cumulative_y < 255)	cogzilla_info.lowest = cumulative_y;
	else /* (cumulative_y > 255) */						cogzilla_info.lowest = 255;

	return cogzilla_info;
}






void URG04LX::getInfo(uint8_t mode)
{
	char msg[4] = {'V','V',0x0A,'\n'};

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

	write(msg);
}


//if mode is 0, turn OFF the laser
void URG04LX::laser(uint8_t mode)
{
	char msg[4] = {'B','M',0x0A,'\n'};

	if(!mode)
	{	msg[0] = 'Q';
		msg[1] = 'T';	}

	write(msg);
}

void URG04LX::baudRate(uint32_t baud)
{
   char command[10] = {'S','S','0','0','0','0','0','0',0x0A,'\n'};

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

   write(command);
}


// refer to page (14/19) for meanings
void URG04LX::setMotor(uint16_t speed)
{
   char command[6] = {'C','R','0','0',0x0A,'\n'};

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

   write(command);
}


//just support requesting time, we don't even need that...
void URG04LX::timeInfo (void)
{
	char msg[5] = {'T','M','1',0xA,'\n'};

	write(msg);
}


void URG04LX::reset(void)
{
	char msg[4] = {'R','S',0xA,'\n'};

	write(msg);
}



/** OBJECT DECLARATION **/
	   URG04LX Lidar;
/************************/
