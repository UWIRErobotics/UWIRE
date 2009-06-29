#include "URG04LX.h"


URG04LX::URG04LX()
       : HardwareSerial(&rx_buffer3, &UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UDR3, RXEN3, TXEN3, RXCIE3, UDRE3)
{ }


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


void URG04LX::distAcq (uint16_t start, uint16_t end, uint8_t cluster)
{
	char msg[17] = {'M','D','0','0','0','0',	 //start step
						    '0','0','0','0',	 //end step
						    '0','1','1',		 //cluster count, scan interval
						    '0','1',0xA,'\n'}; //# of scans


}


// refer to page (14/19) for meanings
void URG04LX::setMotor(uint8_t speed)
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


void URG04LX::setMotor(uint16_t speed)
{
   switch(speed)
   {
	   case 600:
	   {
		   setMotor((uint8_t) 0);
		   break;
	   }

	   case 594:
	   {
		   setMotor((uint8_t) 1);
		   break;
	   }

	   case 588:
	   {
		   setMotor((uint8_t) 2);
		   break;
	   }

	   case 582:
	   {
		   setMotor((uint8_t) 3);
		   break;
	   }

	   case 576:
	   {
		   setMotor((uint8_t) 4);
		   break;
	   }

	   case 570:
	   {
		   setMotor((uint8_t) 5);
		   break;
	   }

	   case 564:
	   {
		   setMotor((uint8_t) 6);
		   break;
	   }

	   case 558:
	   {
		   setMotor((uint8_t) 7);
		   break;
	   }

	   case 552:
	   {
		   setMotor((uint8_t) 8);
		   break;
	   }

	   case 546:
	   {
		   setMotor((uint8_t) 9);
		   break;
	   }

	   case 540:
	   {
		   setMotor((uint8_t) 10);
		   break;
	   }

	   default:
	   {
		   setMotor((uint8_t) 99);
		   break;
	   }
   }
}

//just support requesting time, we don't even need that...
inline void URG04LX::timeInfo (void)
{
	char msg[5] = {'T','M',0x1,0xA,'\n'};

	write(msg);
}

inline void URG04LX::reset(void)
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
