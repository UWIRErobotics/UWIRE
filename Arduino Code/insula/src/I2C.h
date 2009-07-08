#ifndef I2C_H_
#define I2C_H_
#include <stddef.h>
#include "arduino/wiring.h"
#include "arduino/Wire.h"
#include "arduino/globals.h"
/***** http://www.robot-electronics.co.uk/htm/srf08tech.html *****/


typedef byte _address;

class SRF08 : public TwoWire
{
public:
	SRF08();
	~SRF08(){}
    void      ping        (_address, byte);
	void      write_reg   (_address, byte, char);

//  derived functions
	uint16_t  range       (_address);

private:

};

/** OBJECT 'DECLARATION' **/
     extern SRF08 Sonar;
/**************************/

#endif /* I2C_H_ */
