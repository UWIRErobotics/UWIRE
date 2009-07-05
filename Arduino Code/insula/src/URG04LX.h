#ifndef URG04LX_H_
#define URG04LX_H_

#include "arduino/WProgram.h"
#include "globals.h"

extern uint16_t LidarData[512];

class URG04LX : public HardwareSerial
{
public:
    URG04LX();
   ~URG04LX(){}

/* all commands are specified in the URG series comm. spec v2.0, page (6 / 19) */
   void     getInfo  (uint8_t);	  // 'VV' & 'PP' & 'II' commands
   void     laser    (uint8_t);	  // 'BM' & 'QT' commands
   void     baudRate (uint32_t);  // 'SS' command
   void     distAcq  (void); 	  // 'MD'command
   void     setMotor (uint16_t);  // 'CR' command
   void     timeInfo (void);	  // 'TM' command, but we only support GETTING time, for now
   void     reset    (void);	  // 'RS' command

   void     supertest(void);
   void     ForceCalc(uint16_t);


private:
	void     ObjectFilter(uint16_t);

	char      distance_msg [17];
	uint16_t  LidarData    [512];
	LidarObj  objects      [128];

	float    pos_force_x,  neg_force_x,
	         pos_force_y,  neg_force_y,
	         cumulative_x, cumulative_y;
};


/*** OBJECT 'DECLARATION' ***/
    extern URG04LX Lidar;
/****************************/

#endif /* URG04LX_H_ */
