#ifndef ARDUINOLINK_H_
#define ARDUINOLINK_H_

#include <stddef.h>					//for "NULL"
#include "arduino/wiring.h"			//for data types
#include "arduino/HardwareSerial.h"	//for Serial
#include "arduino/globals.h"		//for UWIRE data types

class ArduinoLink : public HardwareSerial
{
public:
	ArduinoLink();
   ~ArduinoLink(){}
};


/** OBJECT 'DECLARATION' **/
  extern ArduinoLink Brain;
/**************************/

#endif /* ARDUINOLINK_H_ */
