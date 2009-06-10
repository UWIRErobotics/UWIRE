#include "main.h"

#define Sonar1 0x71
#define Sonar2 0x72
#define Sonar3 0x74
#define Sonar4 0x78

typedef union
{
	uint16_t container;
	struct{
		uint8_t high;
		uint8_t low;
	};
}byte16;

void setup()
{
	Serial0.begin(19200);   //user console
	Serial1.begin(9600);    //insula comm

	Serial0.println("Cogzilla Console");
}


void loop()
{
	if(Serial1.available() > 3)
	{
		char msgID = Serial1.read();
		byte16 result;
		result.container = 0x0;

		Serial0.println();
		Serial0.print("Received Message ID: 0x");
		Serial0.print(msgID, HEX);
		Serial0.print(" = (");
		Serial0.print(msgID, DEC);
		Serial0.print(") = ");
		Serial0.println((char) msgID);

		if(Sonar1 == msgID)
		{
			result.low = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar1 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);
		}

		else if(Sonar2 == msgID)
		{
			result.low = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar2 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);
		}

		else if(Sonar3 == msgID)
		{
			result.low = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar3 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);
		}

		else if(Sonar4 == msgID)
		{
			result.low = (int) Serial1.read();
			result.high  = (int) Serial1.read();
			Serial1.flush();

			Serial0.print("Sonar4 Measurment = ");
			Serial0.print((int)result.low);
			Serial0.print((int)result.high);
			Serial0.print(" = ");
			Serial0.println(result.container);
		}

		else if ('0' == msgID)
		{
			while(Serial1.available() > 0)
				Serial0.print( (char)Serial1.read() );

			Serial1.flush();
		}
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
