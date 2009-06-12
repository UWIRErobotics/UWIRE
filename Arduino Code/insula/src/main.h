#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"

//#include "ArduinoLink.h"
#include "GPS.h"
#include "I2C.h"


typedef union
{
	uint16_t container;
	struct{
		uint8_t high;
		uint8_t low;
	};
}byte16;

/*****MAIN FUNCTIONS*****/
void setup();
void loop();
int  main();

void CLI(char *);


#endif /* MAIN_H_ */
