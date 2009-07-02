#include "URG04LX.h"


URG04LX::URG04LX()
       : HardwareSerial(&rx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, UDRE2)
{	// use "MS..." for 2-bit encoding, "MD..." for 3-bit
	char msg[17] = {'M','S','0','1','0','0',	 //start step
						    '0','6','6','8',	 //end step
						    '0','1','1',		 //cluster count, scan interval
						    '0','1',0xA,'\n'}; 	 //# of scans

	for(int i = 0; i < 17; i++)
		distance_msg[i] = msg[i];
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

void URG04LX::supertest(void)
{
	distance_msg[1] = 'D';
	distAcq();
	distance_msg[1] = 'S';

}

void URG04LX::distAcq (void)
{
	uint8_t  lines     = 0x00;
	uint8_t  whichbyte = 0x00;
	uint16_t temp      = 0x00;
	uint16_t angle     = 100;

	LidarCount = 0;
	write(distance_msg);

	while(LidarCount < 1024);	//wait for incoming data

	for(uint16_t i = 0; i < LidarCount; i++)
	{
		switch (lines)
		{
			case 0:	//skip all the header stuff
			case 1: //
			case 2: //
			case 3: //
			case 4: //
			case 5: //
			{
				if( 0xA == big_buffer[i] )	lines++;
				whichbyte = 0;
				break;
			}

			case 6:	//6....18? something like that. find out. do it.
			case 7:
			default:
			{
				if(0 == whichbyte)	temp = 0;

				big_buffer[i] -= 0x30;	//decipher that byte
				temp <<= 6;				//shift 'high' byte to the

				temp += big_buffer[i];

				whichbyte++;
				if(2 == whichbyte)
				{
					Serial0.print(angle, DEC);
					Serial0.print(" : ");
					Serial0.println(temp, DEC);
					whichbyte = 0;
					angle++;
				}
			}

		}//switch..case

	}//for loop

	flush();

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

/*
void URG04LX::parse()
{
    for(uint16_t i = 0; i <= 558; i++)
    {
            dist = ( ( input_buffer[i*2] - 0x30 ) << 8 ) + ( input_buffer[i*2+1] - 0x30 );
            if ( dist > MAX_DIST_MM )
                    dist = MAX_DIST_MM;
            if ( ( dist < 20 ) && ( i != 0 ) )
                    dist = distances[i-1];
            distances[i] = dist;
    }
}
*/

/* NOTES ON PARSING:
 *
1x uint16_t variable "count", the FIRST BYTE indicates if incoming data is high or low (0 or 1)
 * while the rest of hte bytes indicate the INDEX (need to shift that value right 1...)
 *
--> Track the PREVIOUS DISTANCE to compare with the CURRENT DISTANCE, if its ABOUT the same
 * then add to another characteristic, "width", and track the "middle" angular reading
 * (that way, we send the angular position of object, distance to object, width of object)
 * and if the measuring is CLOSER, use that as the real distance (closest measurement)
 *
--> Idea; have parsing dynamic, so instead of ALWAYS needing to track 558 angular measurements,
 * the width can be adjustable (and thus, 558 is the MAX amount we'd ever have to parse)
 * depending on the amount of 'clusters' used in the measurement; this also allows for very
 * small readings (if we're suddenly interested in a very small range, be able to parse
 * 'down to' 1 single reading
 */


/** OBJECT DECLARATION **/
	   URG04LX Lidar;
/************************/
