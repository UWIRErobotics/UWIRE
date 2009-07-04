#include "main.h"

#define NUM_READINGS 2

double obs_distance[NUM_READINGS];
double obs_angle [NUM_READINGS] = {2.35619,0.7853981};
double x_distance[NUM_READINGS];
double y_distance[NUM_READINGS];
double x_force [NUM_READINGS];
double y_force [NUM_READINGS];


void setup()
{
//  start all comm links
	Serial0.begin(19200); 	// user console
	Brain.begin  (9600); 	// Serial1
	GPS.begin    (38400);	// Serial3

	Serial0.println("Insular Cortex Console");
	delay(500);
}

void GPS_print()
{

	Serial0.flush();
	Serial0.println("Beginning GPS Stream!");
	while(Serial0.read() != 'e')
	{
		//  once the checksum character ( * ) is found, 'done' goes high
		if(Serialflag.done3 == 0x3)
		{
			unsigned long temp = 0;
			char *pbuff = GPS.fill();	//fill our 'parsing' buffer

			GPS.parse();
			Serialflag.done3 = 0x0;

	/*//      Only for debugging purposes
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
			Serial0.println(temp);*/

			//Printing out in MATLAB-friendly format
			temp = GPS.get(GPS.time);
			Serial0.print(temp);
			Serial0.print(",");

			/*temp = (unsigned int) GPS.get(GPS.speed);
			Serial0.print(temp);
			Serial0.print(",");

			temp = (unsigned int) GPS.get(GPS.course);
			Serial0.print(temp);
			Serial0.print(",");*/

			temp = GPS.get(GPS.latitude);
			Serial0.print(temp);
			Serial0.print(",");

			temp = GPS.get(GPS.longitude);
			Serial0.print(temp);
			Serial0.print(",");

			temp = GPS.get(GPS.pos_fix);
			Serial0.print(temp);
			Serial0.print(",");

			temp = GPS.get(GPS.sats_used);
			Serial0.print(temp);
			Serial0.print(",");

			temp = GPS.get(GPS.HDOP);
			Serial0.println(temp);
		}
	}

	Serial0.println("Leaving GPS stream!");

}


void Sonar_calc()
{
	double cum_force_x = 0;
	double cum_force_y = 0;
	byte32 cogzilla_info;
	/* Highest  = negative x
	 * High 	= positive x
	 * Low		= negative y
	 * Lowest	= positive y*/

	while(Serial0.read() != 'e')
	{
		cum_force_x = 0;
		cum_force_y = 0;
		for(int i=0;i<NUM_READINGS;i++)
		{
			obs_distance[i] = Sonar.range(Sonar1 + i); //for now this is just 0x71 and 0x72...add lidar update here.
			x_distance[i] = obs_distance[i]*(cos(obs_angle[i]));
			y_distance[i] = obs_distance[i]*(sin(obs_angle[i]));
			x_force[i] = (FORCE_CONST/obs_distance[i]) * (x_distance[i]/obs_distance[i]);
			y_force[i] = (FORCE_CONST/obs_distance[i]) * (y_distance[i]/obs_distance[i]);
			cum_force_x += x_force[i];
			cum_force_y += y_force[i];
		}
		Serial0.print (cum_force_x);
		Serial0.print ("     ");
		Serial0.println(cum_force_y);


		cogzilla_info.container = 0;
		if (cum_force_x < -255)
			cogzilla_info.highest = 255;
		else if(cum_force_x > -255 && cum_force_x < 0)
			cogzilla_info.highest = abs(cum_force_x);
		else if(cum_force_x > 0 && cum_force_x < 255)
			cogzilla_info.high = cum_force_x;
		else //cum_force_X > 255
			cogzilla_info.high = 255;

		if (cum_force_y < -255)
			cogzilla_info.low = 255;
		else if(cum_force_y > -255 && cum_force_y < 0)
			cogzilla_info.low = cum_force_y;
		else if(cum_force_y > 0 && cum_force_y < 255)
			cogzilla_info.lowest = cum_force_y;
		else //cum_force_y > 255
			cogzilla_info.lowest = 255;

		Serial0.print ((int)cogzilla_info.highest);
		Serial0.print ("     ");
		Serial0.println ((int)cogzilla_info.high);
		Serial0.print ((int)cogzilla_info.low);
		Serial0.print ("     ");
		Serial0.println ((int)cogzilla_info.lowest);
		Serial0.println();
		Serial0.println();

		Brain.write(FORCE_HEADER_X);
		Brain.write(cogzilla_info.highest);
		Brain.write(cogzilla_info.high);
		Brain.write(FORCE_HEADER_Y);
		Brain.write(cogzilla_info.low);
		Brain.write(cogzilla_info.lowest);
	}
}



void loop()
{
	Sonar_calc();
}


/* Insular Cortex Command Line Interpreter *
 *******************************************
 * currently, we don't do anything with    *
 * length, just parse the first byte in    *
 * the received array from Serial0 or RF   */
void CLI(char *msg, uint8_t length)
{
//  ignore any leading spaces
	while(' ' == *msg)
		msg++;

	Serial0.print("Received: ");
	Serial0.println(*msg);

	byte16 val;	// sonar buffer

	if     ('~' == *msg)
	{
		Serial0.println("Entering RC mode");
		//RC_mode();
		Serial0.println("Leaving RC mode");
	}




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
	else if('G' == *msg)
	{
		GPS_print();
	}
	else if('H' == *msg)
	{
		Sonar_calc();
	}

/*******************GPS configuration *************************
		else if('a' == *msg)	GPS.stop_feed(GPS.GGA);
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
*/
	Serial0.println();
	Serial0.flush();
}


int main(void)
{
	init();
	setup();

	for (;;)
	{
		loop();
	}///loop()

	return 0;
}
