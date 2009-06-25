#include <WString.h>

/* Reading Arduino Packets
 * -----------------------
 *  note RM 1  = raw mode
 *
 * Created June 8, 2009
 *
 */


/*	Need to keep checking for something that is not -1 in case program 
	reads too fast, faster than hardware
*/
char ArduinoReadChar() {

	char value = Serial3.read();
	
	while ( value == -1 ) {

                //delete me later
                Serial.print("-1");	
  
		value = Serial3.read();
	}	
	
        //---------------------delete me later
        Serial.print("-1 endded");
        Serial.print("\nvalue: ");        
        if ( value == 13 ) {
          Serial.println("return carriage");
        } else {
          Serial.println(value);
        }
        //----------------------
        
	return value;
	
}

/*	Need to keep checking for something that is not -1 in case program 
	reads too fast, faster than hardware
*/
int ArduinoReadInt()  {

	int value = Serial3.read();
	
	while ( value == -1 ) {

                //delete me later
                Serial.print("-1");	
  
		value = Serial3.read();
	}	
	
        //---------------------delete me later
        Serial.print("-1 endded");
        Serial.print("\nvalue: ");        
        if ( value == 13 ) {
          Serial.println("return carriage");
        } else {
          Serial.println(value);
        }
        //----------------------
        
	return value;
	
}
 
/*reads the H packet coming out of the CMUcam2 from using the commands:
        PM 1\r
        BM 1\r
        GH\r
        
        *** make sure to initialize the int * resultingHistogram on the heap!!!
*/
boolean read_H_Packet(int *resultingHistogram) {
  
  for (int i=0; i<28; i++) {    		 

    int value = ArduinoReadInt();	  
    
    //stick number in bin
    resultingHistogram[i] = value;
  
    //error checking to see if return carriage appears in middle of data
    if ( value == 13 ) {
      Serial.println("ERROR: return carriage not expected in middle of packet data!");
      return false;
    }
  }
  
  //success
  return true;
  
}


/*	expects a 
				int result[8] 
	as parameter
*/
boolean read_T_Packet( int *result ) {
	
	for(int i=0; i<8; i++) {
	
		result[i] = ArduinoReadInt();
		
		if ( result[i] == 58 || result[i] == 13 ) {
			Serial.println("ERROR: Unexpected colon or return carriage in middle of T packet!");
			return false;	
		}
		
	} 
	
	//--------------------------delete me later
	Serial.print("mx: ");
	Serial.println(result[0]);
	Serial.print("my: ");
	Serial.println(result[1]);
	Serial.print("x1_leftMost: ");
	Serial.println(result[2]);
	Serial.print("y1_leftMost: ");
	Serial.println(result[3]);
	Serial.print("x2_rightMost: ");
	Serial.println(result[4]);
	Serial.print("y2_rightMost: ");
	Serial.println(result[5]);
	Serial.print("numPixels: ");
	Serial.println(result[6]);
	Serial.print("confidence: ");
	Serial.println(result[7]);
	//---------------------------
	
	return true;
	
}



/*reads any packet coming out of the CMUcam2, but not finished implementing

so far only reads the H packet
*/
boolean win_readArduinoPacket(int *result) {

  //delete me later
  Serial.println("win_readArduinoPacket entered");
  
  boolean isPacketGood = false;   
    
  //read packet type
  char type = ArduinoReadChar();        
       
  //what type of packet is it?  follow the yellow brick of code
  switch ( type ) {
      
    //Histogram packet
    case 'H':      
      Serial.println( "H Packet Detected!\r\n");
   
      //int resultingHistogram[28];        - if you put this back into code then take away parameter in method signature
      isPacketGood = read_H_Packet( result );
      break;

      case 'T':
        Serial.println( "T Packet Detected!\r\n");
        
        /* 	---- FOR REFERENCE ---
        	result[0] = mx, 
        	result[1] = my,
        	result[2] = x1,
        	result[3] = y1,
        	result[4] = x2,
        	result[5] = y2,
        	result[6] = numPixels,
        	result[7] = confidence;
        	---------------------
        */
        isPacketGood = read_T_Packet( result );
      
        break;

    default:
      Serial.println("ERROR: unknown packet detected");
        
  }

  
  /*	isPacketGood :
    					true for good packet reading
    					false for being screwed
  */  
  return isPacketGood;
  
}


/* Determines whether or not the CMUcam is seeing a stop sign by 
   using the histogram statistics.
*/
boolean isStopSign( int *bin ) {
  
  char stopSignBinValues[28] = { 1, 7, 13, 12, 8, 6, 5, 4, 6, 20, 26, 20, 13, 14, 11, 7, 5, 4, 4, 3, 2, 2, 2, 3, 3, 3, 3, 35};
  
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
  
  /*---------------------delete me later
  Serial.println("bin values: ----------------" );
  for (int i=0; i<28; i++) {
    Serial.print(bin[i]);
    Serial.print(" ");  
  }
  Serial.print("\nnumBinsMatch: ");
  Serial.println(numBinsMatch);
  double product = (double)(numBinsMatch / 28);
  Serial.print("percentage: ");
  Serial.print( product );
  //----------------------*/
  
  //if the bins match more than 50% then claim it is close to being a stop sign
  if ( numBinsMatch > 13 ) {
    return true;
  }
  
  return false;
  
}



void setup()
{
  Serial.begin(9600);
  
  //cmucam baud rate
  Serial3.begin(9600);
}

void loop()
{
  
  //Serial.println("r u ok?");
  
  if (Serial.available() > 0 ) {
        
    Serial.println("--- PROGRAM STARTED ---");

    while (1) {
      Serial3.print("PM 1");
      Serial3.write(0xD);     
	    
      Serial.println("PM 1 cmd sent");

      if ( ArduinoReadChar() == 'A' && ArduinoReadChar() == 'C' && ArduinoReadChar() == 'K' && ArduinoReadChar() == '\r' ) {
	  	break;    
      }      
    }
    
    Serial.println("SUCCESS!");
        
    while (1) {
      Serial3.print("BM 1");
      Serial3.write(0xD);     
	    
      Serial.println("BM 1 cmd sent");

      if ( ArduinoReadChar() == ':' && ArduinoReadChar() == 'A' && ArduinoReadChar() == 'C' && ArduinoReadChar() == 'K' && ArduinoReadChar() == '\r' ) {
	  	break;    
      }
    }
       
    Serial.println("SUCCESS!");   
    
    while (1) {
      Serial3.print("RM 1");
      Serial3.write(0xD);     
	    
      Serial.println("RM 1 cmd sent");

      if ( ArduinoReadChar() == ':' && ArduinoReadChar() == 'A' && ArduinoReadChar() == 'C' && ArduinoReadChar() == 'K' && ArduinoReadChar() == '\r' ) {
	  	break;    
      }
    }
       
    Serial.println("SUCCESS!");     
    
    
    //  GH cmd --------------------------------------------------------------------------
    
    while (1) {
      Serial3.print("GH");
      Serial3.write(0xD);     
	    
      Serial.println("gh cmd sent");

      if ( ArduinoReadChar() == ':' && ArduinoReadChar() == 'A' && ArduinoReadChar() == 'C' && ArduinoReadChar() == 'K' && ArduinoReadChar() == '\r' ) {
	  	break;    
      }
    }
    
    Serial.println("SUCCESS!");
    
    int resultingHistogram[28];  //stores the bin values from the GH command
    boolean isSuccess = win_readArduinoPacket( resultingHistogram );
    
    //check to see if packet read was successful or not
    if ( isSuccess ) {
      Serial.println( "SUCCESS IN PACKET READ\n" );      
      
    } else {
      Serial.println( "FAIL 2 READ PACKET\n" );      
    }    
    
    
    //check to see if it matches stopsign
    if ( isStopSign( resultingHistogram ) ) {
      Serial.println("FOUND STOP SIGN!  Do something!");
    }
    
    
    
    
    //  TC cmd --------------------------------------------------------------------------

    
    while (1) {
      Serial3.print("TC 72 122 24 74 0 41");
      Serial3.write(0xD);     
	    
      Serial.println("TC cmd sent");

      if ( ArduinoReadChar() == ':' && ArduinoReadChar() == 'A' && ArduinoReadChar() == 'C' && ArduinoReadChar() == 'K' && ArduinoReadChar() == '\r' ) {
	  	break;    
      }
    }
    
    Serial.println("SUCCESS!");
    
    int resultTPacket[8];  //stores the bin values from the GH command
    isSuccess = win_readArduinoPacket( resultTPacket );
    
    //check to see if packet read was successful or not
    if ( isSuccess ) {
      Serial.println( "SUCCESS IN PACKET READ\n" );      
      
    } else {
      Serial.println( "FAIL 2 READ PACKET\n" );      
    }  
  
    
    
  	//--------- END ERROR CHECKING  
    //error checking : expected colon at beginning for next command
  	if ( ArduinoReadChar() != 58 ) {
      Serial.println("ERROR: colon expected at end of packet after carriage return!");
  	}
      
  }// end if of Serial.available()
  
}
