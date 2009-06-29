#ifndef URG04LX_H_
#define URG04LX_H_

#include <stddef.h>					//for "NULL"
#include "arduino/wiring.h"			//for data types
#include "arduino/HardwareSerial.h"	//for Serial


class URG04LX : public HardwareSerial
{
public:
    URG04LX();
   ~URG04LX(){}

/* all commands are specified in the URG series comm. spec v2.0, page (6 / 19) */
   void     getInfo  (uint8_t);	 // 'VV' & 'PP' & 'II' commands
   void     laser    (uint8_t);	 // 'BM' & 'QT' commands
   void     baudRate (uint32_t); // 'SS' command
   void     distAcq  (uint16_t, uint16_t, uint8_t); // 'MD'command
   void     setMotor (uint8_t);	 // 'CR' command
   void     setMotor (uint16_t); //
   void     timeInfo (void);	 // 'TM' command, but we only support GETTING time, for now
   void     reset    (void);	 // 'RS' command



private:
	uint8_t  input_buffer[1140];	//from step 100 to 668 = 568 values (and double that for bytes)
	uint16_t data_buffer[668];
};


/*** OBJECT 'DECLARATION' ***/
    extern URG04LX Lidar;
/****************************/

#endif /* URG04LX_H_ */
