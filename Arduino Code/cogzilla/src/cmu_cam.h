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
#include "globals.h"
#include "cog_globals.h"

#define CMU_RESULT_MX 0
#define CMU_RESULT_MY 1
#define CMU_RESULT_CONF 7

class cmu_cam
{
public:
	cmu_cam(){};
	~cmu_cam(){};
	bool ack_recieved(bool colon_expected);
	char read_cmu_byte();
	void setup_cmu_cam(HardwareSerial *serial_port );
	bool track_colour();

	bool stop_sign_in_view;
	char stop_sign_track_info[8];
};

/** OBJECT 'DECLARATION' **/
     extern cmu_cam cmu_cam1;
/**************************/

#endif /* CMU_CAM_H_ */
