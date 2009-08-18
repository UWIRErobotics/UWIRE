#ifndef MAIN_H_
#define MAIN_H_

#include "arduino/WProgram.h"
#include "URG04LX.h"
#include "GPS.h"


void setup();
int	 main();

void CLI		(char *, uint8_t);
void check_msg	();
//void RC_mode	();

#endif /* MAIN_H_ */
