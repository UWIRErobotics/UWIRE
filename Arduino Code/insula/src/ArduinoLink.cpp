#include "ArduinoLink.h"

ArduinoLink Brain;


ArduinoLink::ArduinoLink()
{
	Serial1.begin(19200);

}
