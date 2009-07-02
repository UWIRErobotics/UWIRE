#include "main.h"

#define  SERIAL_BAUD  19200
#define  RF_BAUD       2400

char buff[VW_MAX_PAYLOAD];

void setup()
{
	for(uint8_t i = 0; i < VW_MAX_PAYLOAD; i++)
		buff[i] = '\n';

	vw_set_tx_pin(36);
    vw_setup(RF_BAUD);

	Serial0.begin(SERIAL_BAUD);   //user console

	Serial0.println("Welcome to Jeff's VirtualWire Console");
	Serial0.print("RF Baud     =  "); Serial0.println(RF_BAUD, DEC);
	Serial0.print("Serial Baud =  "); Serial0.println(SERIAL_BAUD, DEC);
	Serial0.println();
}


void loop()
{
	if(Serial0.available() > 0)
	{
		delay(50);	//wait for a little while for more keystrokes...

		while(Serial0.available() > 0)
		{

		}
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
