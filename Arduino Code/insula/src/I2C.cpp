#include "I2C.h"

/*** OBJECT DECLARATION ***/
        SRF08 Sonar;
/**************************/

#define CMD_REG       0x00
#define LIGHT_REG     0x01
#define ECHO_HIGH_REG 0x02
#define ECHO_LOW_REG  0x03

#define SOFTWARE_REV  CMD_REG
#define MAX_GAIN      LIGHT_REG
#define RANGE_REG     ECHO_HIGH_REG


SRF08::SRF08()
      : TwoWire()
{
	begin(); //Initialise as the bus master

//  Initialise all internal var's
	num_devices      = 0x00;	devices[0].addr = 0x00;

//  array of valid device addresses
	devices[1].addr  = 0xE0;	devices[2].addr  = 0xE2;
	devices[3].addr  = 0xE4;	devices[4].addr  = 0xE6;
	devices[5].addr  = 0xE8;	devices[6].addr  = 0xEA;
	devices[7].addr  = 0xEC;	devices[8].addr  = 0xEE;
	devices[9].addr  = 0xF0;	devices[10].addr = 0xF2;
	devices[11].addr = 0xF4;	devices[12].addr = 0xF6;
	devices[13].addr = 0xF8;	devices[14].addr = 0xFA;
	devices[15].addr = 0xFC;	devices[16].addr = 0xFE;

	for(int i = 0; i <= 16; i++)
		devices[i].used = false;
}


_index SRF08::attach(_address address)
{
	if(16 == num_devices)	return 0x00;	//can't attach more than 16 devices

	boolean error = true;
	_index  index = 0x00;

//  check that it is a valid address
	for(int i = 0; i <= 16; i++)
	{
		if(devices[i].addr == address)
		{
			index = (_index)i;			//record index of this address
			error = devices[i].used;	//if 'used' is true, trying to attach duplicate address!

			break;
		}
	}

	if(error)	return 0x00;	//duplicate or not-valid address

	num_devices++;
	devices[index].used = true;

	return index;
}


_address SRF08::detach(_index index)
{
//  can't detach what doesn't exist
	if( (0x00 == index) || (0x00 == num_devices) )
		return 0x00;

//  make sure device at that index exists
	if(false == devices[index].used)
		return 0x00;

	devices[index].used = false;
	num_devices--;

	return devices[index].addr;
}


inline void SRF08::write_reg(_index index, byte reg, char val)
{
	beginTransmission(devices[index].addr);
	send(reg);
	send(val);
	endTransmission();
}

_index SRF08::change_addr(_index index, _address addr)
{
//  make sure device exists
	if(false == devices[index].used)	return 0x00;

	boolean error    = true;
	_index  newindex = 0x00;

//  make sure new address is valid and not already being used
	for(; newindex <= 16; newindex++)
	{
		if(devices[newindex].addr == addr)
		{
			error = devices[newindex].used;	//if proposed address is taken...
			break;
		}
	}
	if(error)	return 0x00;

	devices[addr].used     = false;	//old address is now free
	devices[newindex].used = true;

	write_cmd(index, 0xA0);
	write_cmd(index, 0xAA);
	write_cmd(index, 0xA5);
	write_cmd(index, addr);

	return newindex;
}
