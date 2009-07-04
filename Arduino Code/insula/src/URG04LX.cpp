#include "URG04LX.h"

#define MAX_DIST	4096
#define MIN_DIST    20


URG04LX::URG04LX()
       : HardwareSerial(&rx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, UDRE2)
{	// use "MS..." for 2-bit encoding, "MD..." for 3-bit
	char msg[17] = {'M','S','0','1','2','8',	 //start step (128 = -90 degrees)
						    '0','6','4','0',	 //end step   (640 = +90 degrees)
						    '0','2','1',		 //cluster count, scan interval
						    '0','1',0xA,'\n'}; 	 //# of scans
	for(int i = 0; i < 17; i++)
		distance_msg[i] = msg[i];
}


void URG04LX::supertest (void)
{
//  turn off regular serial reading
	Serialflag.flag2   = 0x3;
	URG_counter        = 0;
	write(distance_msg);

	uint8_t   lines      = 0x00;		// counts <LF>'s
	uint16_t  bytecnt    = 0x00;		// gross byte count
	uint16_t  meascnt    = 0x00;
//	float     angle      = -90.703125;	// which angle does the value correspond to?

//	wait for ~(1/2) data
	while(URG_counter < 256);


//  header (skip), ALWAYS 42 BYTES
	for(; lines < 6; bytecnt++)
		if( 0xA == URG_buffer[bytecnt] )	lines++;

	uint8_t bob = bytecnt;

//  data section
	for(; bytecnt <= URG_counter; meascnt++, bytecnt+= 2)
	{
		if( (0xA == URG_buffer[bytecnt]) || (0xA == URG_buffer[bytecnt + 1]) )
			lines++;	//skip <LF> & 'sum' characters

		else
		{
			LidarData[meascnt] = 0;
			LidarData[meascnt] = ( ((URG_buffer[bytecnt] - 0x30) << 6) + (URG_buffer[bytecnt + 1] - 0x30) );

			Serial0.print(meascnt);
			Serial0.print(" : ");
			Serial0.println(LidarData[meascnt], DEC);

			//angle    += 0.703125;	// = 2*[360/1024] (we use cluster size of 2)
		}

	}//for loop

	Serialflag.flag2 = 0x0;		//turn ring-buffer back on
	Serial0.print("HEADER BYTE COUNT = ");
	Serial0.println(bob, DEC);
	Serial0.print("TOTAL BYTE COUNT = ");
	Serial0.println(bytecnt, DEC);
	Serial0.print("MEASURMENT COUNT = ");
	Serial0.println(meascnt, DEC);
	Serial0.print("LINE COUNT = ");
	Serial0.println(lines, DEC);

	ObjectFilter(meascnt);
}


void URG04LX::ObjectFilter(uint16_t cnt)
{
	uint8_t obj = 0;	//object index



	for(int i = 1; i < (cnt - 1); i++)	//first and last measurments get special treatment
	{

	}

}


void URG04LX::distAcq (void)
{
	uint8_t   lines      = 0x00;		// counts <LF>'s
	uint8_t   whichbyte  = 0x00;		// high or low byte?	TODO: GET RID OF THIS, integrate into ANOTHER...
	uint16_t  bytecnt    = 0x00;		// gross byte count
	uint16_t  temp       = 0x00;		// stores the 'deciphered' value
	float     angle      = 1; //-90.703125;	// which angle does the value correspond to?


//  turn off regular serial reading
	URG_counter        = 0;
	Serialflag.flag2  = 0x3;
	write(distance_msg);

//	wait for ~(1/2) data
	while(URG_counter < 256);


//  header (skip), ALWAYS 42 BYTES
	for(; lines < 6; bytecnt++)
		if( 0xA == URG_buffer[bytecnt] )	lines++;

	uint8_t bob = bytecnt;

//  data section
	for(bytecnt = 42; bytecnt <= URG_counter; bytecnt++)
	{
		if( 0xA == URG_buffer[bytecnt] )	//end of a line? skip it
		{
			lines++;
			whichbyte = 0;
		}

		else
		{
			if(0 == whichbyte)	temp = 0;

			URG_buffer[bytecnt] -=  0x30;
			temp               <<=  6;
			temp                +=  URG_buffer[bytecnt];

			whichbyte++;

//          done getting a value!
			if(2 == whichbyte)
			{
				Serial0.print(angle);
				Serial0.print(" : ");
				Serial0.println(temp, DEC);

				whichbyte = 0;
				angle    += 1;	// = 2*[360/1024] (we use cluster size of 2)

			}
		}
	}//for loop

	Serialflag.flag2 = 0x0;		//turn ring-buffer back on
	Serial0.print("HEADER BYTE COUNT = ");
	Serial0.println(bob, DEC);
	Serial0.print("TOTAL BYTE COUNT = ");
	Serial0.println(bytecnt, DEC);
	Serial0.print("LINE COUNT = ");
	Serial0.println(lines, DEC);
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
	{
		msg[0] = 'Q';
		msg[1] = 'T';
	}

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
	   case 0:
	   case 1:
	   case 2:
	   case 3:
	   case 4:
	   case 5:
	   case 6:
	   case 7:
	   case 8:
	   case 9:
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




/* NOTES ON PARSING:
1x uint16_t variable "count", the FIRST BYTE indicates if incoming data is high or low (0 or 1)
 * while the rest of hte bytes indicate the INDEX (need to shift that value right 1...)
 *
--> Track the PREVIOUS DISTANCE to compare with the CURRENT DISTANCE, if its ABOUT the same
 * then add to another characteristic, "width", and track the "middle" angular reading
 * (that way, we send the angular position of object, distance to object, width of object)
 * and if the measuring is CLOSER, use that as the real distance (closest measurement)
 */


/** OBJECT DECLARATION **/
	   URG04LX Lidar;
/************************/
