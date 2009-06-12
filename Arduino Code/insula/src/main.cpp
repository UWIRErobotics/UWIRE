#include "main.h"

// Message Headers
#define Sonar1  0x71
#define Sonar2  0x72
#define Sonar3  0x74
#define Sonar4  0x78
#define TestMsg 0xFF


byte16 val;

void setup()
{
	Serial0.begin(19200);   //user console
	Serial1.begin(9600);	//cogzilla
//  Serial3.begin(19200);	//GPS

	Serial0.println("Insular Cortex Console");
}


void loop()
{

	if(Serial3.done == true)
	{
		unsigned long temp = 0;
		char *pbuff = GPS.fill();

		Serial0.println();
		Serial0.print(pbuff);
		//Serial1.print(pbuff);

		GPS.parse();

		Serial3.done = false;

		temp = GPS.get(GPS.time);
		Serial0.print("Time = ");
		Serial0.println(temp);

		temp = GPS.get(GPS.speed);
		Serial0.print("Speed = ");
		Serial0.println(temp);

		temp = GPS.get(GPS.latitude);
		Serial0.print("Lat = ");
		Serial0.println(temp);

		temp = GPS.get(GPS.longitude);
		Serial0.print("Long = ");
		Serial0.println(temp);
		//Serial0.print(GPS.read());
	}

	if(Serial0.available() > 0)
	{
		char cmd = Serial0.read();
		CLI( &cmd );
	}

}


void CLI(char *msg)
{
//  ignore any leading spaces
	while(' ' == *msg)
		msg++;

	Serial0.print("Received: ");
	Serial0.println(*msg);
	Serial0.flush();	//ONLY RECEIVED 1 BYTE OMG!

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

		Serial1.print((char) Sonar1);
		Serial1.print((char) val.low);
		Serial1.print((char) val.high);
	}
	else if('B' == *msg)
	{
		val.container = Sonar.range(Sonar2);

		Serial0.print("0x");
		Serial0.print(Sonar2, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.println((int) val.high);

		Serial1.write(Sonar2);
		Serial1.write(val.low);
		Serial1.write(val.high);
	}
	else if('C' == *msg)
	{
		val.container = Sonar.range(Sonar3);

		Serial0.print("0x");
		Serial0.print(Sonar3, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.println((int) val.high);

		Serial1.print((char) Sonar3);
		Serial1.print((char) val.low);
		Serial1.print((char) val.high);
	}
	else if('D' == *msg)
	{
		val.container = Sonar.range(Sonar4);

		Serial0.print("0x");
		Serial0.print(Sonar4, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.println((int) val.high);

		Serial1.write( Sonar4  );
		Serial1.write( val.low );
		Serial1.write( val.high);
	}
	else if('E' == *msg)
	{
		Serial0.print("Sending that message: ");

		char message[] = {'G','O','u','1','r','E','!', '\0'};

		Serial0.write(message);
		Serial0.println();

		Serial1.write( TestMsg );
		Serial1.write(message);
	}

	else if('F' == *msg)
	{
		val.container = 0x1234;

		Serial0.print("0x");
		Serial0.print(Sonar4, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.print((int) val.high);
		Serial0.print(" = ");
		Serial0.println((int) val.container);

		Serial1.write( Sonar4  );
		Serial1.write( val.low );
		Serial1.write( val.high);
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
