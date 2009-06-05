#ifndef I2C_H_
#define I2C_H_

#include <stddef.h>			//for "NULL"
#include "arduino/wiring.h"	//for data types
#include "arduino/Wire.h"	//for I2C


class I2C {
public:
	I2C();
	~I2C();

	byte attach(byte address);	//return index

private:
	//byte num_devices;
};

/** OBJECT 'DECLARATION' **/
      extern I2C Sonar;
/**************************/

#endif /* I2C_H_ */
