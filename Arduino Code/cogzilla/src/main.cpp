#include "main.h"

char input = 'a' ;
int set_angle=1300;
int set_speed=0;
bool timer_var = 0;

byte32 insula_info;
bool new_sonar_data = false;
bool pin_mode = true;

void virtual_timer()
{
	timer_var = true;
	/*if (pin_mode)
	{
		digitalWrite(53,HIGH);
		pin_mode = false;
	}
	else
	{
		digitalWrite(53,LOW);
		pin_mode = true;
	}*/

}

void setup()
{
	Serial0.begin(9600);   //user console
	Serial3.begin(9600);    //insula comm
	Serial0.println("Cogzilla Console");

	neuro_bot.setup_vehicle();
	//cmu_cam1.setup_cmu_cam(&Serial2);

	pinMode(53,OUTPUT);

	MsTimer2::set(50,virtual_timer);
	MsTimer2::start();



}


void loop()
{
	if(Serial0.available() > 0)
		CLI();
}



void cmu_processing()
{
	while(cmu_cam1.track_colour() == false )
		cmu_cam1.track_colour();
}



void rc_drive()
{
	Serial0.println("Entering RC Drive");
	MsTimer2::start();
	while(input!='e' && !cmu_cam1.stop_sign_in_view)
	if(Serial0.available() > 0)
	{
		if (timer_var)
		{
			//cmu_processing();
			timer_var = false;
		}

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

			Serial0.flush();
		}
	}

	Serial0.println("Exiting RC Drive");
	neuro_bot.set_speed(0);
	MsTimer2::stop();

}

char read_insula_byte()
{
	while (Serial3.available()< 1) {//
		Serial0.println("NOT GETTING ANYTHING!");
	} ; //wait for packet

	return Serial3.read();
}

void insula_read()
{
	insula_info.container = 0;
	if (Serial3.read()==FORCE_HEADER_X)
	{
		insula_info.highest = read_insula_byte();
		insula_info.high = read_insula_byte();
	}

	if (Serial3.read()==FORCE_HEADER_Y)
	{
		insula_info.low = read_insula_byte();
		insula_info.lowest = read_insula_byte();
		new_sonar_data = true;
	}

	//ADD ON GPS/IR HERE!
}

void Sonar_Drive()
{
	int steering_angle = 0;
	int force_x = 0;
	int force_y = 0;

	neuro_bot.set_speed(13);

	while (Serial0.read() !='e')
	{
		if (timer_var)
		{
			insula_read();
			timer_var = false;
		}

		if (new_sonar_data)
		{
			force_x = (-1 * (int)insula_info.highest) + (int)insula_info.high;
			force_y = (-1 * (int)insula_info.lowest) + (int)insula_info.low;

			if (force_x < 0)
			{
				steering_angle = map (force_x,-255,0,900,1300);
				neuro_bot.set_turn_angle(steering_angle);
			}
			else
			{
				steering_angle = map (force_x,255,0,1800,1300);
				neuro_bot.set_turn_angle(steering_angle);
			}
			new_sonar_data = false;
			Serial0.print(force_x);
			Serial0.print("    ");
			Serial0.println(force_y);
		}
	}

	neuro_bot.set_speed(0);
}

void CLI()
{
	Serial0.println("CLI-ing");
	input = Serial0.read();

	if (input == 'c')
	{
		Serial0.println("Entering Cam Processing");
		MsTimer2::start();
		while (Serial0.read()!='e')
		{
			if (timer_var)
			{
				cmu_processing();
				timer_var = false;
			}

			if (cmu_cam1.stop_sign_in_view)
			{
				//neuro_bot.set_speed(0);
				Serial0.println ("Found Stop Sign");
			}
			else
			{
				//neuro_bot.set_speed(25);
				Serial0.println ("No Stop Sign");
			}
		}
		neuro_bot.set_speed(0);
		MsTimer2::stop();
		Serial0.println("Exiting Cam Processing");
	}

	if (input=='s')
	{
		Serial0.println("Sonar Drive!");
		Sonar_Drive();
		Serial0.println("Exiting Sonar Drive");

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
