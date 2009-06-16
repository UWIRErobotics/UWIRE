#ifndef ANALOG_H_
#define ANALOG_H_

#include <stddef.h>					//for "NULL"
#include "arduino/wiring.h"			//for data types


class Analog {
public:
	Analog();
	~Analog();
};


/** OBJECT 'DECLARATION' **/
   extern Analog Infrared;
/**************************/

#endif /* ANALOG_H_ */
