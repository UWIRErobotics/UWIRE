LidarDriver::LidarDriver()
       :port("/dev/ttyUSB0")
       {}

// Initialize serial with 8-E-1 and 9600 Baud.
void LidarDriver::setup()
{
       cout << "Lidar Serial port: ";
       port.Open();
       port.SetBaudRate( SerialPort::BAUD_19200 );
       port.SetCharSize( SerialPort::CHAR_SIZE_8 );
       port.SetParity( SerialPort::PARITY_NONE );
       port.SetNumOfStopBits( SerialPort::STOP_BITS_1 );
       cout << "Initialized" << endl;
       changeBaudRate();
       port.SetBaudRate( SerialPort::BAUD_115200 );
}

void LidarDriver::shutDown()
{
       cout << "Lidar Serial port: ";
       port.Close();
       cout << "Closed " << endl;
}

int LidarDriver::processMsg ( Buffer command, Buffer &header, Buffer &status, Buffer &data )
{
       port.Write( command );

       Buffer inBuf;

       try {port.Read( inBuf, 10000, 100 ); }
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

       return 0;
}

int LidarDriver::changeBaudRate()
{

       //S (53H) Baud Rate (6 Digits) Reserved Area (7 Digits) LF (0aH) or CR (0dH)
       Buffer command;

       command.push_back('S');
       command.push_back('1');
       command.push_back('1');
       command.push_back('5');
       command.push_back('2');
       command.push_back('0');
       command.push_back('0');
       command.push_back('X');
       command.push_back('X');
       command.push_back('X');
       command.push_back('X');
       command.push_back('X');
       command.push_back('X');
       command.push_back('X');
       command.push_back('\n');

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

int LidarDriver::updateDistances()
{
       Buffer command;

       command.push_back('G');
       command.push_back('0');
       command.push_back('0');
       command.push_back('0');
       command.push_back('7');
       command.push_back('6');
       command.push_back('8');
       command.push_back('0');
       command.push_back('0');
       command.push_back('\n');

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