#include "ArduinoLink.h"


ArduinoLink::ArduinoLink()
: HardwareSerial(&rx_buffer1, &UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UDR1, RXEN1, TXEN1, RXCIE1, UDRE1)
{


}

/** OBJECT DECLARATION **/
    ArduinoLink Brain;
/************************/
