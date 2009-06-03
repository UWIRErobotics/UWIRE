#include "main.h"

int main(void)
{
	init();
	setup();

	for (;;)
		loop();

	return 0;
}

void setup()
{
	Serial.begin(19200);   //user console
	Serial.println("Insular Cortex Console");
}

void loop()
{
	if(Serial.available() > 0)
	{
		console_in = Serial.read();

		Serial.print("Received: ");
		Serial.println(console_in);
		Serial.flush();

		if     ('a' == console_in)	GPS.stop_feed(GPS.GGA);
		else if('b' == console_in)	GPS.stop_feed(GPS.GLL);
		else if('c' == console_in)	GPS.stop_feed(GPS.GSA);
		else if('d' == console_in)	GPS.stop_feed(GPS.GSV);
		else if('e' == console_in)	GPS.stop_feed(GPS.RMC);
		else if('f' == console_in)	GPS.stop_feed(GPS.VTG);

		else if('g' == console_in)	GPS.start_feed(GPS.GGA, 1);
		else if('h' == console_in)	GPS.start_feed(GPS.GLL, 1);
		else if('i' == console_in)	GPS.start_feed(GPS.GSA, 1);
		else if('j' == console_in)	GPS.start_feed(GPS.GSV, 1);
		else if('k' == console_in)	GPS.start_feed(GPS.RMC, 1);
		else if('l' == console_in)	GPS.start_feed(GPS.VTG, 1);
	}

	if(GPS.available() > 0)
	{
		Serial.print(GPS.read());
	}

}

