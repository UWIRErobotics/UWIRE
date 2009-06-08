#ifndef ARDUINOLINK_H_
#define ARDUINOLINK_H_

#include "arduino/HardwareSerial.h"

class ArduinoLink// : public HardwareSerial
{
public:
	ArduinoLink();
	~ArduinoLink();
};


/** OBJECT 'DECLARATION' **/
  extern ArduinoLink Brain;
/**************************/

#endif /* ARDUINOLINK_H_ */
