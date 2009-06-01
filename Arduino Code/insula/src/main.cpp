#include "main.h"

void setup()
{
	Serial.begin(19200);   //user console
	Serial3.begin(4800);   //GPS

	Serial.println("Insular Cortex Console");
}

void loop()
{
	if(Serial.available() > 0)
	{
		ibuff_console = Serial.read();
		Serial.flush();

		Serial.print("RECEIVED: ");
		Serial.println(ibuff_console);

		if('a' == ibuff_console)
			flag = stop_feed(GGA);

		if('b' == ibuff_console)
			flag = stop_feed(GLL);

		if('c' == ibuff_console)
			flag = stop_feed(GSA);

		if('d' == ibuff_console)
			flag = stop_feed(GSV);

		if('e' == ibuff_console)
			flag = stop_feed(VTG);
	}

	if(flag)
	{
		Serial.print("Sending command to GPS: ");

		for(int i = 0; i < 25; i++)
		{
			Serial.print(buffer_out[i]);
			Serial3.print(buffer_out[i]);
		}

		flag = 0x00;
	}


	if(Serial3.available() > 0)
	{
		ibuff_GPS = Serial3.read();
		Serial.print(ibuff_GPS);
	}

}

int main(void)
{
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

