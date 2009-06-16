#include "I2C.h"

/*** OBJECT DECLARATION ***/
        SRF08 Sonar;
/**************************/

// 'writing' registers
#define CMD_REG        0x00
#define MAX_GAIN       0x01
#define RANGE_REG      0x02
// 'reading' registers
#define SOFTWARE_REV   0x00
#define LIGHT_REG      0x01
#define ECHO_HIGH_REG  0x02
#define ECHO_LOW_REG   0x03


SRF08::SRF08() : TwoWire()	{begin();}


void SRF08::ping(_address address, byte reg)
{
	beginTransmission(address);
	send(reg);
	endTransmission();
}


void SRF08::write_reg(_address address, byte reg, char val)
{
	beginTransmission(address);
	send(reg);
	send(val);
	endTransmission();
}

uint16_t SRF08::range(_address address)
{
	uint16_t result = 0x0000;

//  send 'range' command (0x50 = inches, 0x51 = centimetres, 0x52 = milliseconds)
	write_reg(address, CMD_REG, 0x51);
	delay(75);

//  ask for the result, tell the bus to expect 2 bytes
	ping(address, RANGE_REG);
	requestFrom( address, (uint8_t)2 );

	if ( available() >= 2 ){
//  get upper byte
		result   = receive();
		result <<= 8;

//  get lower byte
		result  |= receive();}

	return result;
}
