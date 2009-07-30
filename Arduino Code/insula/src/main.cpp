#include "main.h"

nybble8 functionflags;

int main(void)
{
	init();
	setup();

	do
	{
		if(0x3 == Serialflag.flag3)
			GPS_send();

		if (functionflags.upper) {
			digitalWrite(24, HIGH);
			Lidar.supertest();
			digitalWrite(24, LOW);
		}

		if (functionflags.lower) {
			digitalWrite(26, HIGH);
			Sonar_calc();
			digitalWrite(26, LOW);
		}

		check_msg();

		delay(25);
	} while(1);

	return 0;
}


void setup()
{
	pinMode(22, OUTPUT);	pinMode(23, OUTPUT);
	pinMode(24, OUTPUT);	pinMode(25, OUTPUT);
	pinMode(26, OUTPUT);	pinMode(27, OUTPUT);

	functionflags.container = 0x00;
/********** COMM. LINKS **********/
	#define RF_BAUD       2400
	#define BRAIN_BAUD    19200
	#define LIDAR_BAUD    250000
	#define GPS_BAUD	  38400
/********************************/

//  start user console
	Serial0.begin(19200);
	Serial0.println ("Insular Cortex Console");

	Brain.begin  (BRAIN_BAUD); 	  // Serial1
	Serial0.print ("Brain Baud = ");  Serial0.println(BRAIN_BAUD, DEC);
	digitalWrite(22, HIGH);		delay(75);

	Lidar.begin  (LIDAR_BAUD);	  // Serial2
	Serial0.print ("Lidar Baud = ");  Serial0.println(LIDAR_BAUD, DEC);
	digitalWrite(23, HIGH);		delay(75);

	GPS.begin    (GPS_BAUD);	  // Serial3
	Serial0.print ("GPS   Baud = ");  Serial0.println(GPS_BAUD,   DEC);
	digitalWrite(25, HIGH);		delay(75);

	pinMode(36,INPUT);		vw_set_rx_pin(36);
    vw_setup(RF_BAUD);		vw_rx_start  ();
	Serial0.print ("RF    Baud = ");  Serial0.println(RF_BAUD,    DEC);

	Serial0.println(); 			delay(500);

	Serial0.flush();  Brain.flush();  Lidar.flush();  GPS.flush();
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

//  check RF
	uint8_t  buff_rf      [VW_MAX_MESSAGE_LEN];
	uint8_t  len_rf   =    VW_MAX_MESSAGE_LEN;
	if(vw_get_message(buff_rf, &len_rf))
	{
		Serial0.print("RF: ");
		for(uint8_t i = 0; i < len_rf; i++)
			Serial0.print(buff_rf[i]);
		Serial0.println();

		CLI((char *) buff_rf, len_rf);
	}

	if(Brain.available() > 0)
		while(Brain.available() > 0)
			Serial0.write( Brain.read() );

//	we should never get anything from LIDAR!
	if(Lidar.available() > 0)
		while(Lidar.available() > 0)
			Serial0.write( Lidar.read() );
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
		RC_mode();
		Serial0.println("Leaving RC mode");
	}

/******************** LIDAR *****************/
	else if('a' == *msg)	Lidar.getInfo('V');
	else if('b' == *msg)	Lidar.getInfo('P');
	else if('c' == *msg)	Lidar.getInfo('I');
	else if('d' == *msg)	Lidar.laser  (OFF);
	else if('e' == *msg)	Lidar.laser  (ON);
	else if('f' == *msg)	Lidar.timeInfo();
	else if('g' == *msg)	Lidar.supertest();
	else if('r' == *msg)	Lidar.reset();

	else if('h' == *msg)
	{
		(functionflags.upper) ^= 0xF;	// toggle LIDAR detection

		if(functionflags.upper)		Serial0.println("LIDAR on");
		else					Serial0.println("LIDAR off");
	}

	else if('i' == *msg)
	{
		(functionflags.lower) ^= 0xF;	// toggle SONAR detection

		if(functionflags.lower)	Serial0.println("SONAR on");
		else					Serial0.println("SONAR off");
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

	Serial0.println();
	Serial0.flush();
}


void GPS_send()
{
	byte16 temp16;
	byte32 temp32;

	temp16.container = GPS_package.speed;
	Brain.write(GPS_speed);
	Brain.write(temp16.high);
	Brain.write(temp16.low);

	temp16.container = GPS_package.course;
	Brain.write(GPS_course);
	Brain.write(temp16.high);
	Brain.write(temp16.low);

	temp32.container = GPS_package.latitude;
	Brain.write(GPS_latitude);
	Brain.write(temp32.highest);
	Brain.write(temp32.high);
	Brain.write(temp32.low);
	Brain.write(temp32.lowest);

	temp32.container = GPS_package.longitude;
	Brain.write(GPS_longitude);
	Brain.write(temp32.highest);
	Brain.write(temp32.high);
	Brain.write(temp32.low);
	Brain.write(temp32.lowest);

	Serialflag.flag3 = 0x0;
}


void Sonar_calc()
{
	double obs_distance[NUM_READINGS];
	double obs_angle   [NUM_READINGS] = {2.35619,0.7853981};
	double x_distance[NUM_READINGS], 	y_distance[NUM_READINGS];
	double x_force	 [NUM_READINGS], 	y_force	  [NUM_READINGS];
	double cum_force_x = 0, cum_force_y = 0;

	for(int i=0;i<NUM_READINGS;i++)
	{
		obs_distance[i] = Sonar.range(Sonar1 + 7*i); //for now this is just 0x71 and 0x72...add lidar update here.

		x_distance[i] = obs_distance[i] * (cos(obs_angle[i]));
		y_distance[i] = obs_distance[i] * (sin(obs_angle[i]));

		x_force[i] 	  = (SONAR_FORCE/obs_distance[i]) * (x_distance[i]/obs_distance[i]);
		y_force[i] 	  = (SONAR_FORCE/obs_distance[i]) * (y_distance[i]/obs_distance[i]);

		cum_force_x  += x_force[i];
		cum_force_y  += y_force[i];
	}

	Serial0.print  ("Sonar:\t Fx = ");
	Serial0.print  (cum_force_x);
	Serial0.print  (",\t Fy = ");
	Serial0.println(cum_force_y);

	byte16 cogzilla_info;	cogzilla_info.container = 0;
//	fill x
	if 	 (255 < abs(cum_force_x))		cogzilla_info.high = 255;
	else								cogzilla_info.high = abs(cum_force_x);
//	fill y
	if 	 (255 < abs(cum_force_y))		cogzilla_info.low  = 255;
	else								cogzilla_info.low  = abs(cum_force_y);

//  send x
	if(cum_force_x < 0)		Brain.write(FORCE_X_NEG);
	else					Brain.write(FORCE_X_POS);
	Brain.write(cogzilla_info.high);

//	send y, always (+)
	Brain.write(FORCE_Y);
	Brain.write(cogzilla_info.low);
}
