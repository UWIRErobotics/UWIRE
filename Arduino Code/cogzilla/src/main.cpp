#include "main.h"

char input = 'a' ;
int set_angle=1300;
int set_speed=0;


void setup()
{
	Serial0.begin(9600);   //user console
	//Serial1.begin(9600);    //insula comm

	pinMode(53,INPUT);
	vw_set_rx_pin(53);
    vw_setup(3000);	 // Bits per sec

    vw_rx_start();   // Start the receiver PLL running

	Serial0.println("Cogzilla Console");
	delay(500);
	neuro_bot.setup_vehicle();
}


void loop()
{
	uint8_t buf[VW_MAX_MESSAGE_LEN];
	uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if( vw_get_message(buf, &buflen) )
    {
    	Serial0.print("Got: ");
		for (int i = 0; i < buflen; i++)
		{
			Serial0.print(buf[i]);
			Serial0.print(" ");
		}

    	input = buf[0];

    	if (input == 'a')
    	{
    		neuro_bot.set_turn_angle(set_angle+=50);
    		Serial0.print ("Angle Set to:");
    		Serial0.println(set_angle);
    	}
    	else if (input =='d')
    	{
    		neuro_bot.set_turn_angle(set_angle-=50);
    		Serial0.print("Angle Set to:");
    		Serial0.println(set_angle);
    	}
    	else if(input =='w')
    	{
    		neuro_bot.set_speed(set_speed+=5);
    		Serial0.print("Speed Set to:");
    		Serial0.println(set_speed);
    	}
    	else if(input =='s')
    	{
    		neuro_bot.set_speed(set_speed-=5);
    		Serial0.print("Speed Set to:");
    		Serial0.println(set_speed);
    	}
    	else if (input == 'j')
    	{
    		neuro_bot.set_speed(0);
    		delay(50);
    		neuro_bot.set_speed(set_speed);
    		Serial0.println("Jogged");
    	}
    	else if (input == 'e')
    	{
    		Serial0.println("Exiting RC Drive");
    	}

    }

	if(Serial1.available() > 0)
		ArduinoComm();

	if(Serial0.available() > 0)
		CLI();

}


void rc_drive()
{
	Serial0.println("Entering RC Drive");
	while(input!='e')
	{
		if (Serial0.available() > 0)
		{
			input = Serial0.read();
			if (input == 'a')
			{
				neuro_bot.set_turn_angle(set_angle+=50);
				Serial0.print ("Angle Set to:");
				Serial0.println(set_angle);
			}
			else if (input =='d')
			{
				neuro_bot.set_turn_angle(set_angle-=50);
				Serial0.print("Angle Set to:");
				Serial0.println(set_angle);
			}
			else if(input =='w')
			{
				neuro_bot.set_speed(set_speed+=5);
				Serial0.print("Speed Set to:");
				Serial0.println(set_speed);
			}
			else if(input =='s')
			{
				neuro_bot.set_speed(set_speed-=5);
				Serial0.print("Speed Set to:");
				Serial0.println(set_speed);
			}
			else if (input == 'j')
			{
				neuro_bot.set_speed(0);
				delay(50);
				neuro_bot.set_speed(set_speed);
				Serial0.println("Jogged");
			}
			else if (input == 'e')
			{
				Serial0.println("Exiting RC Drive");
			}
			Serial0.flush();
		}
	}

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
	input = Serial0.read();
	if (input == 'd')
	{
		Serial0.flush();
		rc_drive();
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
