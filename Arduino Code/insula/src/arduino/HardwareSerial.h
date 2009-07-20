#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>
#include "globals.h"
#include "Print.h"

#define RX_BUFFER_SIZE  256

// Preinstantiate buffers & flags here for global access
extern volatile unsigned char GPS_buffer[128], URG_buffer[640];
extern volatile uint16_t 	  GPS_counter,     URG_counter;
extern 		   _GPS_package   GPS_package;

struct ring_buffer{
	int           head;
	int           tail;
	unsigned char buffer[RX_BUFFER_SIZE];
}extern rx_buffer0, rx_buffer1, rx_buffer2, rx_buffer3;


struct USARTflags{
	uint8_t flag0 : 2;
	uint8_t flag1 : 2;
	uint8_t flag2 : 2;
	uint8_t flag3 : 2;
}extern volatile Serialflag;


class HardwareSerial : public Print
{
  public:
    HardwareSerial(ring_buffer *rx_buffer,
                   volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
                   volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
                   volatile uint8_t *udr,
                   uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udre);

        void     begin     (long);
        uint8_t  available (void);
        int      read      (void);
        void     flush     (void);
virtual void     write     (uint8_t);
using   Print::  write; // pull in write(str) and write(buff, size) from Print


  private:
    ring_buffer *_rx_buffer;

//  USART registers
    volatile uint8_t *_ubrrh;	// baud rate 'high' byte
    volatile uint8_t *_ubrrl;	// baud rate 'low' byte
    volatile uint8_t *_ucsra;   // USART control/status register 'A'
    volatile uint8_t *_ucsrb;   // USART control/status register 'B'
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;             // interrupt on receive 'enable'
    uint8_t _udre;              // transmit data register empty
};

/*****************************/
extern HardwareSerial Serial0;
extern HardwareSerial Brain;
//extern HardwareSerial Serial2;
//extern HardwareSerial Serial3;
/*****************************/

#endif
