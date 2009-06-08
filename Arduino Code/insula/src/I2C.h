#ifndef I2C_H_
#define I2C_H_
#include <stddef.h>
#include "arduino/wiring.h"
#include "arduino/Wire.h"

/***** http://www.robot-electronics.co.uk/htm/srf08tech.shtml *****/

#define CMD_REG        0x00
#define LIGHT_REG      0x01
#define ECHO_HIGH_REG  0x02
#define ECHO_LOW_REG   0x03
#define SOFTWARE_REV   CMD_REG
#define MAX_GAIN       LIGHT_REG
#define RANGE_REG      ECHO_HIGH_REG

typedef byte _address;	//HW address of devices
typedef byte _index;


class SRF08 : public TwoWire
{
public:
	SRF08();
	~SRF08(){}
	_index   attach      (_address); //return index; the only function with 'address' as an argument
	_address detach      (_index);

	void     write_reg   (_index, byte, char);
	_index   change_addr (_index, _address);

inline void  write_cmd   (_index index, char val)  {write_reg(index, CMD_REG, val);}
inline void  range       (_index index, char val)  {write_cmd(index, 0x51); send(RANGE_REG);}


private:
	byte num_devices;

	struct _devices{
		_address addr;
		boolean  used;
	}devices[17];
};

/** OBJECT 'DECLARATION' **/
     extern SRF08 Sonar;
/**************************/
#endif /* I2C_H_ */
