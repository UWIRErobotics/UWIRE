#include "main.h"


void setup()
{
	Serial0.begin(19200);   //user console
	Serial1.begin(9600);    //insula comm

	Serial0.println("Cogzilla Console");
}


void loop()
{
	if(Serial1.available() > 0)
		ArduinoComm();

	if(Serial0.available() > 0)
		CLI();
}


void ArduinoComm()
{
	char msgID = Serial1.read();

	Serial0.println();
	Serial0.print("Received Message ID: (");
	Serial0.print(msgID, DEC);
	Serial0.print(") = 0x");
	Serial0.print(msgID, HEX);
	Serial0.print(" = ");
	Serial0.println((char) msgID);

	switch(msgID)
	{
		case GPS_RAW:
		case ArduinoMSG:
		{
			while(Serial1.available() > 0)
				Serial0.print( (char)Serial1.read() );

			Serial1.flush();

			break;
		}

		case Sonar1:
		{
			byte16 result;	result.container = 0x0;

			result.low   = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar1 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);

			break;
		}

		case Sonar2:
		{
			byte16 result;	result.container = 0x0;

			result.low   = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar2 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);

			break;
		}

		case Sonar3:
		{
			byte16 result;	result.container = 0x0;

			result.low   = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar3 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);

			break;
		}

		case Sonar4:
		{
			byte16 result;	result.container = 0x0;

			result.low   = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar4 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);

			break;
		}
	}

}

void CLI()
{

}

int main(void)
{
	init();
	setup();

	for (;;)
		loop();

	return 0;
}
