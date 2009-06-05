#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>

#include "Print.h"

struct ring_buffer;

class HardwareSerial : public Print
{
  public:
    HardwareSerial(ring_buffer *rx_buffer,
                   volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
                   volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
                   volatile uint8_t *udr,
                   uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udre);

        void     begin(long);
        uint8_t  available(void);
        int      read(void);
        void     flush(void);
virtual void     write(uint8_t);

// pull in write(str) and write(buf, size) from Print
        using   Print::write;

  private:
    ring_buffer *_rx_buffer;

//  USART registers
    volatile uint8_t *_ubrrh;	//baud rate 'high' byte
    volatile uint8_t *_ubrrl;	//baud rate 'low' byte
    volatile uint8_t *_ucsra;   //USART control/status register 'A'
    volatile uint8_t *_ucsrb;   //USART control/status register 'B'
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;             //interrupt on receive 'enable'
    uint8_t _udre;              //transmit data register empty
};

extern HardwareSerial Serial0;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;

#endif

