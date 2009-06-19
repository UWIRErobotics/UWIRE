#ifndef URG04LX_H_
#define URG04LX_H_

#include <stddef.h>					//for "NULL"
#include "arduino/wiring.h"			//for data types
#include "arduino/HardwareSerial.h"	//for Serial
#include "globals.h"				//for UWIRE data types


class URG04LX : public HardwareSerial
{
public:
    URG04LX();
   ~URG04LX(){}

   void processMsg      ( Buffer, Buffer*, Buffer *, Buffer * );
   void changeBaudRate  (void);
   void updateDistances (void);

};

#endif /* URG04LX_H_ */