
#ifndef VEHICLE_CONTROL_H_
#define VEHICLE_CONTROL_H_

#include "arduino/MegaServo.h"
#include "arduino/HardwareSerial.h"
#include "arduino/WProgram.h"
#include "cog_globals.h"

class vehicle
{
public:
	vehicle(){};
	~vehicle(){};

	void setup_vehicle();
	void set_turn_angle(int angle); //0 - full left, 1800 - full right
	void set_speed(int speed); //64 full forward, -64 full backward

private:
	int turn_angle;
	int drive_speed;
	int drive_speed_setting;

};

/** OBJECT 'DECLARATION' **/
     extern vehicle neuro_bot;
/**************************/


#endif /* VEHICLE_CONTROL_H_ */


