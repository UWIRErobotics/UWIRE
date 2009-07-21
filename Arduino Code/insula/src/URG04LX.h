#ifndef URG04LX_H_
#define URG04LX_H_

#include "arduino/WProgram.h"
#include "arduino/globals.h"


class URG04LX : public HardwareSerial
{
public:
    URG04LX();
   ~URG04LX(){}

/* all commands are specified in the URG series comm. spec v2.0, page (6 / 19) */
   void     getInfo  (uint8_t);	  // 'VV' & 'PP' & 'II' commands
   void     laser    (uint8_t);	  // 'BM' & 'QT' commands
   void     baudRate (uint32_t);  // 'SS' command
   void     setMotor (uint16_t);  // 'CR' command
   void     timeInfo (void);	  // 'TM' command, but we only support GETTING time, for now
   void     reset    (void);	  // 'RS' command

   void     setSerial(uint8_t);	  // turn regular serial on / off
   uint16_t distAcq  (void); 	  // 'MD'command

   void     supertest(void);


private:
	uint8_t ObjectFilter(uint16_t);
	byte32  ForceCalc   (uint16_t);
	void 	BruteCalc	(uint16_t);
	void	send_force	(void);

	uint8_t 	distance_msg[17];	//outgoing message
	uint16_t 	LidarData   [512];
	signed int 	cumulative_x, cumulative_y;
};


/*** OBJECT 'DECLARATION' ***/
    extern URG04LX Lidar;
/****************************/

#endif /* URG04LX_H_ */
