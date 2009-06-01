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

byte request_feed(NMEA_types type, byte rate, boolean chksum_enable)
{
	byte i = 0x00;    //counter

//  message template
	char temp[25] = {'$','P','S','R','F','1','0','3',',',
                     '0','0',',',
                     '0','0',',',
                     '0','0',',',
                     '0','0','*',
                     '0','0',13,10};

	for(i = 0; i < 25; i++)
		buffer_out[i] = temp[i];

//  update for user-defined NMEA type
	if     (GGA == type)	buffer_out[10] = '0';
	else if(GLL == type)	buffer_out[10] = '1';
	else if(GSA == type)	buffer_out[10] = '2';
	else if(GSV == type)	buffer_out[10] = '3';
	else if(RMC == type)	buffer_out[10] = '4';
	else if(VTG == type)	buffer_out[10] = '5';

//	update for user-defined checksum
	if(chksum_enable)
		buffer_out[19] = '1';


//  update for user-defined update rate
	if      (rate <= 9)
		buffer_out[19] = rate + 48;
	else if (rate >= 10 && rate <= 99)
	{
		//update the rate to represent double-digit vals
	}


//	calculate the checksum
	chksum checksum;
	checksum.container = 0x00;

	for(i = 1; i <= 19; i++)
		checksum.container ^= buffer_out[i];


//  convert checksum into ASCII values, put lower byte first
	if(checksum.sigchar.upper <= 0x9)
		buffer_out[22] = checksum.sigchar.upper + 48; //= ASCI code for the number
	else
		buffer_out[22] = checksum.sigchar.upper + 55; //=ASCI code for the letter!

	if(checksum.sigchar.lower <= 0x9)
		buffer_out[21] = checksum.sigchar.lower + 48; //= ASCI ode for the number
	else
		buffer_out[21] = checksum.sigchar.lower + 55; //=ASCI code for the letter!

	return 0xFF;
}

