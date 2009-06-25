#include "main.h"


int main(void)
{
	init();
	setup();

	for (;;)
	{
		check_GPS();

		check_msg();



	}///loop()

	return 0;
}


void setup()
{
//  start all comm links
	Serial0.begin(19200); 	// user console
	Brain.begin  (38400); 	// Serial1
//	Lidar.begin  (19200);	// Serial2
	GPS.begin    (38400);	// Serial3

//	setup wireless link
	pinMode(36, INPUT);
	vw_set_rx_pin(36);
    vw_setup(2400);
    vw_rx_start();

	Serial0.println("Insular Cortex Console");
}


void check_msg(void)
{
//  input buffers
	char     buff_console [VW_MAX_MESSAGE_LEN];	//same size as RF buffer (why not?)
	uint8_t  buff_rf      [VW_MAX_MESSAGE_LEN];
	uint8_t  len_rf   =    VW_MAX_MESSAGE_LEN;
//  check RF
	if(vw_get_message(buff_rf, &len_rf))
		CLI((char *) buff_rf, len_rf);

//  check user console
	if(Serial0.available() > 0)
	{
		uint8_t len_console = 0x00;
		for(; Serial0.available() > 0; len_console++)
			buff_console[len_console] = Serial0.read();

		CLI(buff_console, len_console);
	}
}


void check_GPS()
{
	if(0x3 == Serialflag.done3)
	{
		char *data = NULL;

		GPS.fill();
		data = (char *) GPS.parse();
		Serialflag.done3 = 0x0;

		Brain.write(GPS_time);		//4 bytes
		Serial0.write(GPS_time);
		Brain.write(data);	Serial0.write(data); data++;
		Brain.write(data);	Serial0.write(data); data++;
		Brain.write(data);	Serial0.write(data); data++;
		Brain.write(data);	Serial0.write(data); data++;


		Brain.write(GPS_latitude);	//4 bytes
		Brain.write(data);	data++;
		Brain.write(data);	data++;
		Brain.write(data);	data++;
		Brain.write(data);	data++;

		Brain.write(GPS_longitude);	//4 bytes
		Brain.write(data);	data++;
		Brain.write(data);	data++;
		Brain.write(data);	data++;
		Brain.write(data);	data++;

		Brain.write(GPS_speed);		//2 bytes
		Brain.write(data);	data++;
		Brain.write(data);	data++;

		Brain.write(GPS_course);	//2 bytes
		Brain.write(data);	data++;
		Brain.write(data);
	}
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
		RC_mode();
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

/*******************GPS configuration *************************/
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

	Serial0.println();
}

//TODO: clean this up...
void RC_mode()
{
	char temp = ERROR;
	Brain.write(RemoteControl);
	delay(10);

//  we need an acknowledgement, but don't freeze waiting for it
	for(int i = 0; i < 10; i++)
	{
		if(Brain.available() > 0)
		{
			temp = Brain.read();
			break;
		}
		else	delay(10);
	}

//	only proceed if we got an aknowledgement
	while(RemoteControl == temp)
	{
		//TODO: wait for RF to come in, send packet immediately,
		//      and wait for the stop byte ( ~ ) to be sent
	}
}
