#include "URG04LX.h"


URG04LX::URG04LX()
       : HardwareSerial(&rx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, UDRE2)
{ }

/*
void URG04LX::processMsg ( Buffer command, Buffer &header, Buffer &status, Buffer &data )
{
       write(command);

       Buffer inBuf;

       try
       {
    	   read( inBuf, 10000, 100 );
       }
       catch (...) {}

       unsigned short lnCnt = 0;

       for ( int i=0; i<inBuf.size(); i++ )
       {

               if ( inBuf[i] == '\n' )
               {
                       lnCnt++;
               }
               else
               {
                       if ( 0 == lnCnt )
                               header.push_back( inBuf[i] );
                       if ( 1 == lnCnt )
                               status.push_back( inBuf[i] );
                       if ( lnCnt > 1 )
                               data.push_back( inBuf[i] );
               }
       }

       cout << "header: ";
       for ( int i = 0; i < header.size(); i++ )
               cout << header[i];
       cout << endl;

       cout << "status: ";
       for ( int i = 0; i < status.size(); i++ )
               cout << status[i];
       cout << endl;

       cout << "data: ";
       for ( int i = 0; i < data.size(); i++ )
               cout << data[i];
       cout << endl;

}
*/
void URG04LX::changeBaudRate(uint32_t baud)
{
       char command[16] = {'S','0','0','0','0','0','0','X','X','X','X','X','X','X',0x0A,'\n'};

       switch(baud)
       {
		   case 57600:
		   {
			   command[2] = '5';
			   command[3] = '7';
			   command[4] = '6';
			   break;
		   }

		   case 115200:
		   {
			   command[1] = '1';
			   command[2] = '1';
			   command[3] = '5';
			   command[4] = '2';
			   break;
		   }

		   case 250000:
		   {
			   command[1] = '2';
			   command[2] = '5';
			   break;
		   }

		   case 500000:
		   {
			   command[1] = '5';
			   break;
		   }

		   case 750000:
		   {
			   command[1] = '7';
			   command[2] = '5';
			   break;
		   }

		   default:
		   case 19200:
		   {
			   command[2] = '1';
			   command[3] = '9';
			   command[4] = '2';
			   break;
		   }
       }


       write(command);

}
/*
void URG04LX::updateDistances()
{
	char command[10] = {'G','0','0','0','7','6','8','0','0','\n'};


       Buffer header, status, data;
       processMsg ( command, header, status, data );

       unsigned short dist;
       if ( data.size() > 768 * 2 )
       {
               for ( int i=0; i<768; i++ )
               {
                       dist = ( ( data[i*2] - 0x30 ) << 8 ) + ( data[i*2+1] - 0x30 );
                       if ( dist > MAX_DIST_MM )
                               dist = MAX_DIST_MM;
                       if ( ( dist < 20 ) && ( i != 0 ) )
                               dist = distances[i-1];
                       distances[i] = dist;
               }
       }
       cout << distances[384] <<endl<<flush;

}
*/


/** OBJECT DECLARATION **/
	   URG04LX Lidar;
/************************/
