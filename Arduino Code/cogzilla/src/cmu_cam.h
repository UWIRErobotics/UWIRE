/*
 * cmu_cam.h
 *
 *  Created on: 23-Jun-2009
 *      Author: prasenjit
 */

#ifndef CMU_CAM_H_
#define CMU_CAM_H_

#include "arduino/HardwareSerial.h"
#include "arduino/WProgram.h"

class cmu_cam
{
public:
	cmu_cam(){};
	~cmu_cam(){};
	bool ack_recieved(bool colon_expected);
	char read_cmu_byte();
	void setup_cmu_cam(HardwareSerial *serial_port );
	bool track_colour();



private:


	int mx;
	int my;
	int x1;
	int	x2;
	int y1;
	int y2;
	int pixels;
	int confidence;

};

/** OBJECT 'DECLARATION' **/
     extern cmu_cam cmu_cam1;
/**************************/

#endif /* CMU_CAM_H_ */
