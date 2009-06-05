#include "main.h"


void setup()
{
	Serial0.begin(19200);   //user console
	Serial0.println("Insular Cortex Console");
}


void loop()
{
	if(Serial0.available() > 0)
		CLI((char *) Serial0.read());

	if(GPS.available() > 0)
		Serial0.print(GPS.read());

}

byte CLI(char *msg)
{
//  ignore any leading spaces
	while(' ' == *msg)
		msg++;

	Serial0.print("Received: ");
	Serial0.println(*msg);
	Serial0.flush();	//ONLY RECEIVED 1 BYTE OMG!

	if     ('a' == *msg)	GPS.stop_feed(GPS.GGA);
	else if('b' == *msg)	GPS.stop_feed(GPS.GLL);
	else if('c' == *msg)	GPS.stop_feed(GPS.GSA);
	else if('d' == *msg)	GPS.stop_feed(GPS.GSV);
	else if('e' == *msg)	GPS.stop_feed(GPS.RMC);
	else if('f' == *msg)	GPS.stop_feed(GPS.VTG);

	else if('g' == *msg)	GPS.start_feed(GPS.GGA, 1);
	else if('h' == *msg)	GPS.start_feed(GPS.GLL, 1);
	else if('i' == *msg)	GPS.start_feed(GPS.GSA, 1);
	else if('j' == *msg)	GPS.start_feed(GPS.GSV, 1);
	else if('k' == *msg)	GPS.start_feed(GPS.RMC, 1);
	else if('l' == *msg)	GPS.start_feed(GPS.VTG, 1);

//	    void set_param(long baud, byte data_bits, boolean stop, byte parity)
	else if('m' == *msg)	GPS.set_param(1200,  8, true, 0);
	else if('n' == *msg)	GPS.set_param(2400,  8, true, 0);
	else if('o' == *msg)	GPS.set_param(4800,  8, true, 0);
	else if('p' == *msg)	GPS.set_param(9600,  8, true, 0);
	else if('q' == *msg)	GPS.set_param(19200, 8, true, 0);
	else if('r' == *msg)	GPS.set_param(38400, 8, true, 0);

	return 0;
}



int main(void)
{
	init();
	setup();

	for (;;)
		loop();

	return 0;
}
