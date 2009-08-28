#include "main.h"
#include "globals.h"

/********** COMM. LINKS **********/
	#define BRAIN_BAUD    38400
	#define GPS_BAUD	  38400
	#define LIDAR_BAUD    250000
/********************************/

nybble8 functionflags;


void setup()
{
	pinMode(22, OUTPUT);	pinMode(23, OUTPUT);
	pinMode(24, OUTPUT);	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);	pinMode(27, OUTPUT);

//  start user console
	Serial0.begin(19200);
	Serial0.println ("Cerebellum Console");

/*	Brain.begin  (BRAIN_BAUD); 	  // Serial1
	Serial0.print ("Brain Baud = ");  Serial0.println(BRAIN_BAUD, DEC);
	digitalWrite(22, HIGH);		delay(75);

	GPS.begin    (GPS_BAUD);	  // Serial3
	Serial0.print ("GPS   Baud = ");  Serial0.println(GPS_BAUD,   DEC);
	digitalWrite(25, HIGH);		delay(75);									*/

	Lidar.begin  (LIDAR_BAUD);	  // Serial2
	Serial0.print ("Lidar Baud = ");  Serial0.println(LIDAR_BAUD, DEC);
	digitalWrite(23, HIGH);		delay(75);

	Serial0.println(); delay(250);
	Serial0.flush();  Brain.flush();  Lidar.flush();  GPS.flush();
}



void check_msg(void)
{
	if(Serial0.available() > 0)
	{
		char 	buff_console [36];
		uint8_t len_console = 0x00;

		for(; Serial0.available() > 0; len_console++)
			buff_console[len_console] = Serial0.read();

		CLI(buff_console, len_console);
	}

/*	if(Brain.available() > 0)
		while(Brain.available() > 0)
			Serial0.write( Brain.read() );

	if(GPS.available() > 0)
		while(GPS.available() > 0)
			Serial0.write( GPS.read() );	*/

	if(Lidar.available() > 0)
		while(Lidar.available() > 0)
			Serial0.write( Lidar.read() );
}



void CLI(char *msg, uint8_t length)
{
//  ignore any leading spaces
	while(' ' == *msg)
		msg++;

	Serial0.print("Received: ");
	Serial0.println(*msg);


	if ('~' == *msg)
	{
		//RC_mode();
		Serial0.println("Leaving RC mode");
	}

/******************** LIDAR *****************/
	else if('a' == *msg)	Lidar.getInfo('V');
	else if('b' == *msg)	Lidar.getInfo('P');
	else if('c' == *msg)	Lidar.getInfo('I');
	else if('d' == *msg)	Lidar.laser  (OFF);
	else if('e' == *msg)	Lidar.laser  (ON);
	else if('f' == *msg)	Lidar.timeInfo();
	else if('g' == *msg)	Lidar.GetPF();
	else if('h' == *msg)	Lidar.supertest();
	else if('r' == *msg)	Lidar.reset();

	else if('i' == *msg)
	{
		(functionflags.upper) ^= 0xF;

		if(functionflags.upper)		Serial0.println("NEW LIDAR on");
		else						Serial0.println("NEW LIDAR off");
	}

	else if('j' == *msg)
	{
		(functionflags.lower) ^= 0xF;

		if(functionflags.lower)	Serial0.println("LKG LIDAR on");
		else					Serial0.println("LKG LIDAR off");
	}



/*******************GPS configuration *************************/
	else if('1' == *msg)	GPS.stop_feed(GPS.GGA);
	else if('2' == *msg)	GPS.stop_feed(GPS.GLL);
	else if('3' == *msg)	GPS.stop_feed(GPS.GSA);
	else if('4' == *msg)	GPS.stop_feed(GPS.GSV);
	else if('5' == *msg)	GPS.stop_feed(GPS.RMC);
	else if('6' == *msg)	GPS.stop_feed(GPS.VTG);

	else if('!' == *msg)	GPS.start_feed(GPS.GGA, 1, true);
	else if('@' == *msg)	GPS.start_feed(GPS.GLL, 1, true);
	else if('#' == *msg)	GPS.start_feed(GPS.GSA, 1, true);
	else if('$' == *msg)	GPS.start_feed(GPS.GSV, 1, true);
	else if('%' == *msg)	GPS.start_feed(GPS.RMC, 1, true);
	else if('^' == *msg)	GPS.start_feed(GPS.VTG, 1, true);

//  void set_param(long baud, byte data_bits, boolean stop, byte parity)
	else if('G' == *msg)	GPS.set_param(1200,  8, true, 0);
	else if('H' == *msg)	GPS.set_param(2400,  8, true, 0);
	else if('I' == *msg)	GPS.set_param(4800,  8, true, 0);
	else if('J' == *msg)	GPS.set_param(9600,  8, true, 0);
	else if('K' == *msg)	GPS.set_param(19200, 8, true, 0);
	else if('L' == *msg)	GPS.set_param(38400, 8, true, 0);

	Serial0.println();
	Serial0.flush();
}


int main(void)
{
	init();
	setup();
    
	do
	{
		if (functionflags.upper) {
			digitalWrite(24, HIGH);
			Lidar.supertest();
			digitalWrite(24, LOW);
		}

		if (functionflags.lower) {
			digitalWrite(26, HIGH);
			Lidar.GetPF();
			digitalWrite(26, LOW);
		}

		check_msg();

		delay(25);
	} while(1);
        
	return 0;
}

