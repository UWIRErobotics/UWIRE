/* Reading Arduino Packets
 * -----------------------
 *
 *
 * Created June 8, 2009
 *
 */



//TODO: create functions to read each type of CMUcam packet  -----------------------------------




boolean read_H_Packet( char *bin ) {
  
  //bin = new char[28];
  
  for (int i=0; i<28; i++) {
    
    char inByte = Serial.read();
    
    //check for return carriage '\r' == ASCII VALUE OF 13
    if ( inByte != 13 ) {    
      bin[i] = inByte;
      
    } else {
      return false; 
    }
    
  }//end for
  
  return true;
  
}


boolean read_T_Packet( char *mx, char *my, char *x1_leftMost, char *y1_leftMost, char *x2_rightMost, char *y2_rightMost, char *numPixels, char *confidence ) {
    
  char inByte = Serial.read();
  
  if ( inByte !=13 ) {
    *mx = inByte; 
  } else {
    return false; 
  }
  
  inByte = Serial.read();
  if ( inByte !=13 ) {
    *my = inByte;    
  } else {
    return false; 
  }
  
  inByte = Serial.read();
  if ( inByte !=13 ) {
    *x1_leftMost = inByte;    
  } else {
    return false; 
  }
  
  inByte = Serial.read();
  if ( inByte !=13 ) {
    *y1_leftMost = inByte;    
  } else {
    return false; 
  }
  
  inByte = Serial.read();
  if ( inByte !=13 ) {
    *x2_rightMost = inByte;    
  } else {
    return false; 
  }
  
  inByte = Serial.read();
  if ( inByte !=13 ) {
    *y2_rightMost = inByte;    
    
  } else {
    return false; 
  }
  
  inByte = Serial.read();
  if ( inByte !=13 ) {
    *numPixels = inByte;    
    
  } else {
    return false; 
  }

  inByte = Serial.read();
  if ( inByte !=13 ) {
    *confidence = inByte;    
    
  } else {
    return false; 
  }  
  
  return true;
}

boolean win_readArduinoPacket( ) {

  boolean isPacketGood = false;
  
  //windows baud rate
  Serial.begin(115200);
  
  //read in type of packet
  char type = (char)Serial.read();
  
  char value = Serial.read();
    
   
  switch ( value ) {
      
    //Histogram packet
    case 'H':      
      Serial.println( "H Packet Detected!\r\n");
      char bins[28];
      isPacketGood = read_H_Packet( bins );
      break;
      
    case 'T':
      Serial.println( "T Packet Detected!\r\n");
      char *mx, *my, *x1, *y1, *x2, *y2, *numPixels, *confidence;
      char buffer[1000];
      isPacketGood = read_T_Packet( mx, my, x1, y1, x2, y2, numPixels, confidence );
      
      //For Debugging only
      strcpy( buffer, mx );
      strcat( buffer, my );
      strcat( buffer, x1 );
      strcat( buffer, y1 );
      strcat( buffer, x2 );
      strcat( buffer, y2 );
      strcat( buffer, numPixels );      
      strcat( buffer, confidence );
      Serial.println( buffer );
      
      break;
      
    default:
      break;
  }

  //TODO: Add code to check integrity of packet 
  //For corruptions return false
  
  return isPacketGood;
  
}


/* Determines whether or not the CMUcam is seeing a stop sign by 
   using the histogram statistics.
*/
boolean isStopSign( int *bin ) {
  
  char stopSignBinValues[28] = { 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50 };
  
  int interval = 10;
  
  /* number of bins that contains a number of pixels of each bin color 
     that is within +/- interval of the stop sign bin values
  */
  int numBinsMatch = 0;
  
  
  for (int i=0; i<28; i++) {
  
    if ( bin[i] < stopSignBinValues[i] + interval && bin[i] > stopSignBinValues[i] - interval ) {
      numBinsMatch++; 
    }
    
  }//end for
  
  //if the bins match more than 50% then claim it is close to being a stop sign
  if ( numBinsMatch / 28 > 0.5 ) {
    return true;
  }
  
  return false;
  
}



void setup()
{
  pinMode(2, INPUT);      // sets the digital pin as output
}

void loop()
{
  
  if (Serial.available() > 0 ) {
        
    boolean isSuccess = win_readArduinoPacket( );
    
    if ( isSuccess ) {
      Serial.println( "SUCCESS IN READ\r\n" );
      
    } else {
      Serial.println( "FAIL 2 READ PACKET\r\n" );
      
    }//end else
      
  }
  
}
