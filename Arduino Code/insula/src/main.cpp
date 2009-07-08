#include "main.h"


int main(void)
{
	init();
	setup();

	do
	{
//		check_msg();

		Lidar_Send();

//		Sonar_calc();
	} while(1);

	return 0;
}


void setup()
{
//  start user console
	Serial0.begin(19200);
	Serial0.println ("Insular Cortex Console");

	Brain.begin  (BRAIN_BAUD); 	  // Serial1
	Serial0.print ("Brain Baud = ");  Serial0.println(BRAIN_BAUD, DEC);

	Lidar.begin  (LIDAR_BAUD);	  // Serial2
	Serial0.print ("Lidar Baud = ");  Serial0.println(LIDAR_BAUD, DEC);

	GPS.begin    (GPS_BAUD);	  // Serial3
	Serial0.print ("GPS   Baud = ");  Serial0.println(GPS_BAUD,   DEC);

	Serial0.println(); delay(400);
}



void check_msg(void)
{
	if(Serial0.available() > 0)
	{
		char 	buff_console [VW_MAX_MESSAGE_LEN];	//same size as RF buffer, I guess
		uint8_t len_console = 0x00;

		for(; Serial0.available() > 0; len_console++)
			buff_console[len_console] = Serial0.read();

		CLI(buff_console, len_console);
	}

/*  check RF
	uint8_t  buff_rf      [VW_MAX_MESSAGE_LEN];
	uint8_t  len_rf   =    VW_MAX_MESSAGE_LEN;

	if(vw_get_message(buff_rf, &len_rf))
		CLI((char *) buff_rf, len_rf);	*/

	if(Lidar.available() > 0)
		Serial0.write( Lidar.read() );

//	if(GPS.available() > 0)
//		Serial0.write( GPS.read() );
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

	if ('~' == *msg)
	{
		Serial0.println("Entering RC mode");
		RC_mode();
		Serial0.println("Leaving RC mode");
	}

/******************** LIDAR *****************/
	else if('a' == *msg)	Lidar.getInfo('V');
	else if('b' == *msg)	Lidar.getInfo('P');
	else if('c' == *msg)	Lidar.getInfo('I');
	else if('d' == *msg)	Lidar.laser  ( 0 );
	else if('e' == *msg)	Lidar.laser  ( 1 );
	else if('f' == *msg)	Lidar.timeInfo();
	else if('r' == *msg)	Lidar.reset();

	else if('g' == *msg)	Lidar.supertest();

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



/************************* I2C Sonar *************************/
	else if('A' == *msg)
	{
		val.container = Sonar.range(Sonar1);

		Serial0.print("0x");
		Serial0.print(Sonar1, HEX);
		Serial0.print(" : ");
		Serial0.print(  (int) val.low);
		Serial0.println((int) val.high);
	}
	else if('B' == *msg)
	{
		val.container = Sonar.range(Sonar2);

		Serial0.print("0x");
		Serial0.print(Sonar2, HEX);
		Serial0.print(" : ");
		Serial0.print(  (int) val.low);
		Serial0.println((int) val.high);
	}
	else if('C' == *msg)
	{
		val.container = Sonar.range(Sonar3);

		Serial0.print("0x");
		Serial0.print(Sonar3, HEX);
		Serial0.print(" : ");
		Serial0.print(  (int) val.low);
		Serial0.println((int) val.high);
	}
	else if('D' == *msg)
	{
		val.container = Sonar.range(Sonar4);

		Serial0.print("0x");
		Serial0.print(Sonar4, HEX);
		Serial0.print(" : ");
		Serial0.print(  (int) val.low);
		Serial0.println((int) val.high);
	}
	else if('E' == *msg)	//just a test for sending text messages...
	{
		Serial0.print("Sending test message: ");

		char message[] = {'G','O','u','1','r','E','!', '\0'};

		Serial0.write(message);
		Serial0.println();

		Brain.write( COMM_TEST );
		Brain.write(  message  );
	}
	else if('F' == *msg)	//just a test for sending 16-bit value as 2x 8-bit values
	{
		val.container = 0x1234;

		Serial0.print("0x");	Serial0.print(Sonar4, HEX);
		Serial0.print(" : ");
		Serial0.print((int) val.low);
		Serial0.print((int) val.high);
		Serial0.print(" = ");
		Serial0.println((int) val.container);

		Brain.write( Sonar4  );
		Brain.write( val.low );
		Brain.write( val.high);
	}
//	else if('G' == *msg)	GPS_print();
//	else if('H' == *msg)	Sonar_calc();


	Serial0.println();
	Serial0.flush();
}


void Lidar_Send()
{
	byte32 cogzilla_info = Lidar.supertest();

	Brain.write(FORCE_HEADER_X);
	Brain.write(cogzilla_info.highest);
	Brain.write(cogzilla_info.high);
	Brain.write(FORCE_HEADER_Y);
	Brain.write(cogzilla_info.low);
	Brain.write(cogzilla_info.lowest);

	Serial0.print ((int)cogzilla_info.highest);
	Serial0.print ("     ");
	Serial0.println ((int)cogzilla_info.high);
	Serial0.print ((int)cogzilla_info.low);
	Serial0.print ("     ");
	Serial0.println ((int)cogzilla_info.lowest);
	Serial0.println();
	Serial0.println();

	delay(25);
}



#define NUM_READINGS 2

void Sonar_calc()
{
	double obs_distance[NUM_READINGS];
	double obs_angle   [NUM_READINGS] = {2.35619,0.7853981};
	double x_distance  [NUM_READINGS];
	double y_distance  [NUM_READINGS];
	double x_force     [NUM_READINGS];
	double y_force     [NUM_READINGS];

	double cum_force_x = 0;
	double cum_force_y = 0;

	while(Serial0.read() != 'e')
	{
		cum_force_x = 0;
		cum_force_y = 0;
		for(int i=0;i<NUM_READINGS;i++)
		{
			obs_distance[i] = Sonar.range(Sonar1 + i); //for now this is just 0x71 and 0x72...add lidar update here.

			x_distance[i] = obs_distance[i] * (cos(obs_angle[i]));
			y_distance[i] = obs_distance[i] * (sin(obs_angle[i]));

			x_force[i] 	  = (SONAR_FORCE/obs_distance[i]) * (x_distance[i]/obs_distance[i]);
			y_force[i] 	  = (SONAR_FORCE/obs_distance[i]) * (y_distance[i]/obs_distance[i]);

			cum_force_x  += x_force[i];
			cum_force_y  += y_force[i];
		}
		Serial0.print  ("Fx = ");
		Serial0.print  (cum_force_x);
		Serial0.print  (",  Fy = ");
		Serial0.println(cum_force_y);

		byte32 cogzilla_info;	cogzilla_info.container = 0;
		/* Highest  = negative x
		 * High 	= positive x
		 * Low		= negative y
		 * Lowest	= positive y*/

		if 		(cum_force_x < -255)						cogzilla_info.highest = 255;
		else if	(cum_force_x > -255 && cum_force_x < 0)		cogzilla_info.highest = abs(cum_force_x);
		else if	(cum_force_x > 0 && cum_force_x < 255)		cogzilla_info.high    = cum_force_x;
		else /*	(cum_force_X > 255 ) */						cogzilla_info.high    = 255;

		if 		(cum_force_y < -255)						cogzilla_info.low 	 = 255;
		else if	(cum_force_y > -255 && cum_force_y < 0)		cogzilla_info.low 	 = cum_force_y;
		else if	(cum_force_y > 0 && cum_force_y < 255)		cogzilla_info.lowest = cum_force_y;
		else /* (cum_force_y > 255) */						cogzilla_info.lowest = 255;

		Brain.write(FORCE_HEADER_X);
		Brain.write(cogzilla_info.highest);
		Brain.write(cogzilla_info.high);

		Brain.write(FORCE_HEADER_Y);
		Brain.write(cogzilla_info.low);
		Brain.write(cogzilla_info.lowest);

//		DEBUG ONLY
		Serial0.print ((int)cogzilla_info.highest);
		Serial0.print ("     ");
		Serial0.println ((int)cogzilla_info.high);
		Serial0.print ((int)cogzilla_info.low);
		Serial0.print ("     ");
		Serial0.println ((int)cogzilla_info.lowest);
		Serial0.println();
		Serial0.println();
	}
}


void GPS_print()
{
	Serial0.flush();
	Serial0.println("Beginning GPS Stream!");
	while(Serial0.read() != 'e')
	{
		//  once the checksum character ( * ) is found, 'done' goes high
		if(Serialflag.flag3 == 0x3)
		{
			char *rawGPS = GPS.fill();	//TODO: GET RID OF THIS BUFFER ENTIRELY

			_GPS_package *GPSdata = GPS.parse();	// get pointer to parsed data
			Serialflag.flag3 = 0x0;					// turn on regular serial

//			printing in MATLAB-friendly format
			Serial0.print(GPSdata->time);		Serial0.print(",");
			Serial0.print(GPSdata->course);		Serial0.print(",");
			Serial0.print(GPSdata->latitude);	Serial0.print(",");
			Serial0.print(GPSdata->longitude);	Serial0.print(",");
			Serial0.print(GPSdata->pos_fix);	Serial0.print(",");
			Serial0.print(GPSdata->sats_used);	Serial0.print(",");
			Serial0.print(GPSdata->HDOP);		Serial0.print(",");
		}
	}

	Serial0.println("Leaving GPS stream!");
}
