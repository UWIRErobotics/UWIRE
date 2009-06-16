#include "main.h"

byte16 val;

void setup()
{
//  start all comm links
	Serial0.begin(19200); //user console
	Brain.begin  (9600 ); //Serial1
    GPS.begin    (38400); //Serial3

	Serial0.println("Insular Cortex Console");
}


void loop()
{
//  once the checksum character ( * ) is found, 'done' goes high
	if(Serialflag.done3 == 0x3)
	{
		unsigned long temp = 0;
		char *pbuff = GPS.fill();	//fill our 'parsing' buffer

//      Parse & Package the data, then indicate we're done
		GPS.parse();
		Serialflag.done3 = 0x0;

//      Only for debugging purposes
		Serial0.println();
		Serial0.print(pbuff);

		temp = GPS.get(GPS.time);
		Serial0.print("Time = ");
		Serial0.println(temp);

		temp = (unsigned int) GPS.get(GPS.speed);
		Serial0.print("Speed = ");
		Serial0.println(temp);

		temp = (unsigned int) GPS.get(GPS.course);
		Serial0.print("Course = ");
		Serial0.println(temp);

		temp = GPS.get(GPS.latitude);
		Serial0.print("Lat = ");
		Serial0.println(temp);

		temp = GPS.get(GPS.longitude);
		Serial0.print("Long = ");
		Serial0.println(temp);
	}

	if(Serial0.available() > 0)
	{
		char cmd[16];
		for(int i = 0; Serial0.available() > 0; i++)
			cmd[i] = Serial0.read();

		Serial0.flush();
		CLI( cmd );
	}
}


void CLI(char *msg)
{
//  ignore any leading spaces
	while(' ' == *msg)
		msg++;

	Serial0.print("Received: ");
	Serial0.println(*msg);


/************************* GPS *************************/
	if     ('a' == *msg)	GPS.stop_feed(GPS.GGA);
	else if('b' == *msg)	GPS.stop_feed(GPS.GLL);
	else if('c' == *msg)	GPS.stop_feed(GPS.GSA);
	else if('d' == *msg)	GPS.stop_feed(GPS.GSV);
	else if('e' == *msg)	GPS.stop_feed(GPS.RMC);
	else if('f' == *msg)	GPS.stop_feed(GPS.VTG);

	else if('g' == *msg)	GPS.start_feed(GPS.GGA, 1, true);
	else if('h' == *msg)	GPS.start_feed(GPS.GLL, 1, true);
	else if('i' == *msg)	GPS.start_feed(GPS.GSA, 1, true);
	else if('j' == *msg)	GPS.start_feed(GPS.GSV, 1, true);
	else if('k' == *msg)	GPS.start_feed(GPS.RMC, 1, true);
	else if('l' == *msg)	GPS.start_feed(GPS.VTG, 1, true);

//  void set_param(long baud, byte data_bits, boolean stop, byte parity)
	else if('m' == *msg)	GPS.set_param(1200,  8, true, 0);
	else if('n' == *msg)	GPS.set_param(2400,  8, true, 0);
	else if('o' == *msg)	GPS.set_param(4800,  8, true, 0);
	else if('p' == *msg)	GPS.set_param(9600,  8, true, 0);
	else if('q' == *msg)	GPS.set_param(19200, 8, true, 0);
	else if('r' == *msg)	GPS.set_param(38400, 8, true, 0);


/************************* Arduino-Arduino Communication *************************/
	else if('A' == *msg)
	{
		val.container = Sonar.range(Sonar1);

		Serial0.print("0x");
		Serial0.print(Sonar1, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.println((int) val.high);

		Brain.write( Sonar1  );
		Brain.write( val.low );
		Brain.write( val.high);
	}
	else if('B' == *msg)
	{
		val.container = Sonar.range(Sonar2);

		Serial0.print("0x");
		Serial0.print(Sonar2, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.println((int) val.high);

		Brain.write( Sonar2  );
		Brain.write( val.low );
		Brain.write( val.high);
	}
	else if('C' == *msg)
	{
		val.container = Sonar.range(Sonar3);

		Serial0.print("0x");
		Serial0.print(Sonar3, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.println((int) val.high);

		Brain.write( Sonar3  );
		Brain.write( val.low );
		Brain.write( val.high);
	}
	else if('D' == *msg)
	{
		val.container = Sonar.range(Sonar4);

		Serial0.print("0x");
		Serial0.print(Sonar4, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.println((int) val.high);

		Brain.write( Sonar4  );
		Brain.write( val.low );
		Brain.write( val.high);
	}
	else if('E' == *msg)	//just a test for sending text messages... doubt we'll ever use this
	{
		Serial0.print("Sending that message: ");

		char message[] = {'G','O','u','1','r','E','!', '\0'};

		Serial0.write(message);
		Serial0.println();

		Brain.write( ArduinoMSG );
		Brain.write(  message  );
	}

	else if('F' == *msg)	//just a test for sending 16-bit value as 2x 8-bit values
	{
		val.container = 0x1234;

		Serial0.print("0x");
		Serial0.print(Sonar4, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.print((int) val.high);
		Serial0.print(" = ");
		Serial0.println((int) val.container);

		Brain.write( Sonar4  );
		Brain.write( val.low );
		Brain.write( val.high);
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
