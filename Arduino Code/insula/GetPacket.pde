#include <WString.h>

/* Reading Arduino Packets
 * -----------------------
 *
 *
 * Created June 8, 2009
 *
 */



/*reads the H packet coming out of the CMUcam2 from using the commands:
        PM 1\r
        BM 1\r
        GH\r
        
        *** make sure to initialize the int * resultingHistogram on the heap!!!
*/
boolean read_H_Packet(int *resultingHistogram) {
  
  //int bins[28];
  
  for (int i=0; i<28; i++) {
  
    //read white space
    Serial.read(); 
    
    int value = Serial.read();
    
    //sanity check
    if ( value == ':' ) {
      Serial.println("ERROR: Expecting colon at end of H packet being read.");
      return false; 
    }
    
    resultingHistogram[i] = value;
    
  }//end for
  
  //read colon
  char value = (char)Serial.read();
  
  //another sanity check, expecting colon at end, if no colon found, then something is wrong
  if ( value != ':' ) {
    Serial.println("ERROR: Expecting colon at end of H packet being read.");
    return false;
  }
  
  //if code reaches here then everything should be fine
  
  // FOR DEBUGGING ONLY -- REMOVE LATER ----------------
  Serial.println( "H Packet Detected!\r\n");          //
  char debugResult_str[56];                           //
  int j = 0;                                          //
  for ( int i=0; i<28; i++ ) {                        //
    debugResult_str[j] = resultingHistogram[i];       //
    j++;                                              //
    debugResult_str[j] = ' ';                         //
    j++;                                              //
  }                                                   //
  Serial.println( debugResult_str );                  //
//------------------------------------------------------

  return true;
  
}



/*reads any packet coming out of the CMUcam2, but not finished implementing

so far only reads the H packet
*/
boolean win_readArduinoPacket() {

  boolean isPacketGood = false;
  
  //windows baud rate
  Serial.begin(115200);
  
  boolean isAcknowledged = false;
  
  //reads the ACK or NCK ~ acknowledgement handshake
  char ack[4];    //the 4th character is for the terminating null
  ack[0] = (char)Serial.read();
  ack[1] = (char)Serial.read();
  ack[2] = (char)Serial.read();
  ack[3] = '\0';
  
  String str_ack = ack;
  
  if ( str_ack.equals("ACK") == true ) {
         
    //flag the state variable
    isAcknowledged = true;
    
    //read in type of packet
    char type = (char)Serial.read(); 
          
    //what type of packet is it?  follow the yellow brick of code
    switch ( type ) {
      
      //Histogram packet
      case 'H':             
        // ***** NEED THIS TO BE ON HEAP, BUT GIVES COMPILE ERROR????
        //int *resultingHistogram = new int[28];
        int resultingHistogram[28];
        
        isPacketGood = read_H_Packet( resultingHistogram );
        break;
      
      
/*--------------------------------------    
      NOT FINISHED IMPLEMENTING T PACKET

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
  --------------------------------------      
*/

      default:
        Serial.println("ERROR: unknown packet detected");
        
    }

  
    return isPacketGood;
  
  }
  
}


/* Determines whether or not the CMUcam is seeing a stop sign by 
   using the histogram statistics.
*/
boolean isStopSign( int *bin ) {
  
  char stopSignBinValues[28] = { 0, 1, 22, 56, 51, 37, 15, 5, 2, 2, 1, 1, 1, 1, 2, 5, 5, 6, 6, 5, 5, 4, 4, 3, 2, 0, 0, 0};
  
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
  
//  Serial.
  
  if (Serial.available() > 0 ) {
        
    boolean isSuccess = win_readArduinoPacket();
    
    if ( isSuccess ) {
      Serial.println( "SUCCESS IN READ\r\n" );
      
    } else {
      Serial.println( "FAIL 2 READ PACKET\r\n" );
      
    }//end else
      
  }
  
}
