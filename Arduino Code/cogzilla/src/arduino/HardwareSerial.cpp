#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "wiring.h"
#include "wiring_private.h"
#include "HardwareSerial.h"



// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which rx_buffer_head is the index of the
// location to which to write the next incoming character and rx_buffer_tail
// is the index of the location from which to read.
#define RX_BUFFER_SIZE 128

bool positive_force_coming = false;
bool negative_force_coming = false;
//extern vehicle neuro_bot;
//extern float steering_angle;
//extern bool new_steering_angle;
float steering_angle=0;
bool new_steering_angle = false;
int x_force=0;

struct ring_buffer {
  unsigned char buffer[RX_BUFFER_SIZE];
  int head;
  int tail;
};

ring_buffer rx_buffer0 = { { 0 }, 0, 0 };
ring_buffer rx_buffer1 = { { 0 }, 0, 0 };
ring_buffer rx_buffer2 = { { 0 }, 0, 0 };
ring_buffer rx_buffer3 = { { 0 }, 0, 0 };

inline void store_char(unsigned char c, ring_buffer *rx_buffer)
{
	int i = (rx_buffer->head + 1) % RX_BUFFER_SIZE;

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != rx_buffer->tail) {
		rx_buffer->buffer[rx_buffer->head] = c;
		rx_buffer->head = i;
	}
}

SIGNAL(SIG_USART0_RECV)
{
	unsigned char c = UDR0;
  store_char(c, &rx_buffer0);
}

SIGNAL(SIG_USART1_RECV)
{
	unsigned char c = UDR1;
  store_char(c, &rx_buffer1);
}

SIGNAL(SIG_USART2_RECV)
{
	unsigned char c = UDR2;
  store_char(c, &rx_buffer2);
}

SIGNAL(SIG_USART3_RECV)
{
	unsigned char c = UDR3;

	if (positive_force_coming)
	{
		x_force = c;
		x_force *=-1;
		steering_angle = (0.00615148 * sq(-1*(float)x_force - 255.0)) + 900;
//		neuro_bot.set_turn_angle(steering_angle);
		new_steering_angle = true;
		positive_force_coming = false;
	}
	else if(negative_force_coming)
	{
		x_force = c;

		steering_angle = (0.00768900 * sq((float)x_force)) + 1300;

//		neuro_bot.set_turn_angle(steering_angle);
		new_steering_angle = true;
		negative_force_coming = false;
	}
	else if (c==FORCE_X_POS)
	  positive_force_coming = true;
	else if (c==FORCE_X_NEG)
	  negative_force_coming = true;
	else
		store_char(c, &rx_buffer3);
}

// Constructors ////////////////////////////////////////////////////////////////

HardwareSerial::HardwareSerial(ring_buffer *rx_buffer,
  volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
  volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
  volatile uint8_t *udr,
  uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udre)
{
  _rx_buffer = rx_buffer;
  _ubrrh = ubrrh;
  _ubrrl = ubrrl;
  _ucsra = ucsra;
  _ucsrb = ucsrb;
  _udr = udr;
  _rxen = rxen;
  _txen = txen;
  _rxcie = rxcie;
  _udre = udre;
}

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::begin(long speed)
{
	*_ubrrh = ((F_CPU / 16 + speed / 2) / speed - 1) >> 8;
	*_ubrrl = ((F_CPU / 16 + speed / 2) / speed - 1);
  sbi(*_ucsrb, _rxen);
  sbi(*_ucsrb, _txen);
  sbi(*_ucsrb, _rxcie);
}

uint8_t HardwareSerial::available(void)
{
	return (RX_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % RX_BUFFER_SIZE;
}

int HardwareSerial::read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (_rx_buffer->head == _rx_buffer->tail) {
		return -1;
	} else {
		unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
		_rx_buffer->tail = (_rx_buffer->tail + 1) % RX_BUFFER_SIZE;
		return c;
	}
}

void HardwareSerial::flush()
{
	// don't reverse this or there may be problems if the RX interrupt
	// occurs after reading the value of rx_buffer_head but before writing
	// the value to rx_buffer_tail; the previous value of rx_buffer_head
	// may be written to rx_buffer_tail, making it appear as if the buffer
	// don't reverse this or there may be problems if the RX interrupt
	// occurs after reading the value of rx_buffer_head but before writing
	// the value to rx_buffer_tail; the previous value of rx_buffer_head
	// may be written to rx_buffer_tail, making it appear as if the buffer
	// were full, not empty.
	_rx_buffer->head = _rx_buffer->tail;
}

void HardwareSerial::write(uint8_t c)
{
	while (! ((*_ucsra) & (1 << _udre)) );

	*_udr = c;
}

// Preinstantiate Objects //////////////////////////////////////////////////////
HardwareSerial Serial0(&rx_buffer0, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UDR0, RXEN0, TXEN0, RXCIE0, UDRE0);
HardwareSerial Serial1(&rx_buffer1, &UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UDR1, RXEN1, TXEN1, RXCIE1, UDRE1);
HardwareSerial Serial2(&rx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, UDRE2);
HardwareSerial Serial3(&rx_buffer3, &UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UDR3, RXEN3, TXEN3, RXCIE3, UDRE3);
