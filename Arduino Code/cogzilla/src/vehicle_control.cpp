

#include "vehicle_control.h"

/*** OBJECT DECLARATION ***/
        vehicle neuro_bot;
/**************************/

#define MAX_PW 					1750
#define MIN_PW 					850

#define MAX_TURN_ANGLE 			1800
#define MIN_TURN_ANGLE 			0

#define MAX_FWD_SPEED_SETTING 	128
#define MAX_BCKWD_SPEED_SETTING 0
#define MAX_FWD_SPEED 			64
#define MAX_BCKWD_SPEED 		-64

#define SERVO_PIN 				4

MegaServo Servo;
HardwareSerial *Drive_Serial;


void vehicle::setup_vehicle()
{
	Servo.attach(4,800,2100);
	Drive_Serial = &Serial1;
	Drive_Serial->begin(9600);
}

void vehicle:: set_turn_angle(int angle)
{
	turn_angle = angle;
	Servo.writeMicroseconds(angle);
}

void vehicle::set_speed(int speed)
{
	int speed_setting=0;
	speed_setting = map(speed, MAX_BCKWD_SPEED, MAX_FWD_SPEED,MAX_BCKWD_SPEED_SETTING,MAX_FWD_SPEED_SETTING );
	drive_speed = speed;
	drive_speed_setting = speed_setting;
	Drive_Serial->write(speed_setting);
}
