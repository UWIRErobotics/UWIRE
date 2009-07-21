#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "wiring.h"
#include "wiring_private.h"
#include "HardwareSerial.h"


ring_buffer rx_buffer0  = { 0, 0, {0} };
ring_buffer rx_buffer1  = { 0, 0, {0} };
ring_buffer rx_buffer2  = { 0, 0, {0} };
ring_buffer rx_buffer3  = { 0, 0, {0} };
volatile USARTflags Serialflag = {0,0,0,0};

// GPS buffers
volatile unsigned char 	GPS_buffer[128] = {0x00};
volatile uint16_t 		GPS_counter 	=  0x00;
volatile uint8_t 		comma 			=  0x00;
		_GPS_package 	GPS_package     = {0,0,0,0,0};
		 bit8 			GPS_flags;

// LIDAR buffers
volatile unsigned char 	URG_buffer[640] = {0x00};
volatile uint16_t 		URG_counter 	=  0x00;


// Constructors ////////////////////////////////////////////////////////////////
HardwareSerial::HardwareSerial(ring_buffer *rx_buffer,
     volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
     volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
     volatile uint8_t *udr,
     uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udre)
{
  _rx_buffer = rx_buffer;
  _ubrrh     = ubrrh;
  _ubrrl     = ubrrl;
  _ucsra     = ucsra;
  _ucsrb     = ucsrb;
  _udr       = udr;
  _rxen      = rxen;
  _txen      = txen;
  _rxcie     = rxcie;
  _udre      = udre;
}


// Public Methods //////////////////////////////////////////////////////////////
void HardwareSerial::begin(long speed) {
	*_ubrrh = ((F_CPU / 16 + speed / 2) / speed - 1) >> 8;
	*_ubrrl = ((F_CPU / 16 + speed / 2) / speed - 1);

	sbi(*_ucsrb, _rxen);
	sbi(*_ucsrb, _txen);
	sbi(*_ucsrb, _rxcie);
}


uint8_t HardwareSerial::available(void) {
	return (RX_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % RX_BUFFER_SIZE;
}


int HardwareSerial::read(void) {
//  if the head isn't ahead of the tail, we don't have any characters
	if (_rx_buffer->head == _rx_buffer->tail)
		return -1;

	else {
		unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
		_rx_buffer->tail = (_rx_buffer->tail + 1) % RX_BUFFER_SIZE;
		return c;
	}
}


void HardwareSerial::flush() {
/* don't reverse this or there may be problems if the RX interrupt
 * occurs after reading the value of rx_buffer_head but before writing
 * the value to rx_buffer_tail; the previous value of rx_buffer_head
 * may be written to rx_buffer_tail, making it appear as if the buffer
 * don't reverse this or there may be problems if the RX interrupt
 * occurs after reading the value of rx_buffer_head but before writing
 * the value to rx_buffer_tail; the previous value of rx_buffer_head
 * may be written to rx_buffer_tail, making it appear as if the buffer
 * were full, not empty                                                 */
	_rx_buffer->head = _rx_buffer->tail;
}


void HardwareSerial::write(uint8_t c) {

	while (! ((*_ucsra) & (1 << _udre)) ); //wait for outgoing buffer to be empty

	*_udr = c;
}


/*********************************************************************************************/
inline void store_char(unsigned char c, ring_buffer *rx_buffer) {
	int i = (rx_buffer->head + 1) % RX_BUFFER_SIZE;

//  prevent buffer overflow
	if (i != rx_buffer->tail) {
		rx_buffer->buffer[rx_buffer->head] = c;
		rx_buffer->head = i;  }
}


/****** Preinstantiate Objects ******/
  HardwareSerial Serial0(&rx_buffer0, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UDR0, RXEN0, TXEN0, RXCIE0, UDRE0);
  HardwareSerial Brain  (&rx_buffer1, &UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UDR1, RXEN1, TXEN1, RXCIE1, UDRE1);
//HardwareSerial Serial2(&rx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, UDRE2);
//HardwareSerial Serial3(&rx_buffer3, &UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UDR3, RXEN3, TXEN3, RXCIE3, UDRE3);


/* SET SERIAL INTERRUPTS HERE (for now, just stores received value) */
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


// URG-04LX LIDAR
SIGNAL(SIG_USART2_RECV)
{
	unsigned char c = UDR2;

//  skip ring buffer
	if(Serialflag.flag2)
	{
		URG_buffer[URG_counter] = UDR2;
		URG_counter++;
	}

// regular communication
	else 	store_char(c, &rx_buffer2);
}


// GPS
SIGNAL(SIG_USART3_RECV)
{
	unsigned char c = UDR3;

//	start of new packet
	if ('$' == c)
	{
		digitalWrite(27, HIGH);
		comma       	 	  = 0x0;
		GPS_counter 	 	  = 0x0;
		GPS_flags.container	  = 0x0;
		Serialflag.flag3 	  = 0x0;

		GPS_package.time      = 0x0;
		GPS_package.speed     = 0x0;
		GPS_package.course    = 0x0;
		GPS_package.latitude  = 0x0;
		GPS_package.longitude = 0x0;
	}

	GPS_buffer[GPS_counter] = c;

	if		(',' == c)	comma++;

	else if (0xA == c)	//end of packet
	{
		digitalWrite(27, LOW);

		if(!(GPS_flags.a))	Serialflag.flag3 = 0x3;
/*		for(uint8_t i = 0; i <= GPS_counter; i++)	//DEBUG ONLY
			Serial0.print(GPS_buffer[i]);			*/
	}

	else if ('.' != c)	//ignore decimals
	{
		switch(comma)
		{
			case 0 :	//message ID
			{
				char header[6] = {'$','G','P','R','M','C'};

				if(c != header[GPS_counter])
					Serial0.println("Header mismatch!");

				break;
			}

			case 1://time
			{
				GPS_package.time *= 10;
				GPS_package.time += (c - 48);
				break;
			}

			case 2://status
			{
				if('A' != c)	GPS_flags.a = 0x1;		//set 'bad status' flag

				break;
			}

			case 3://latitude
			{
				GPS_package.latitude *= 10;
				GPS_package.latitude += (c - 48);
				break;
			}

			case 4://latitude indicator
			{
				if('N' != c)
					Serial0.println("Latitude heading incorrect!");

				break;
			}

			case 5://longitude
			{
				GPS_package.longitude *= 10;
				GPS_package.longitude += (c - 48);
				break;
			}

			case 6://longitude indicator
			{
				if('W' != c)
					Serial0.println("Longitude heading incorrect!");

				break;
			}

			case 7://speed
			{
				GPS_package.speed *= 10;
				GPS_package.speed += (c - 48);
				break;
			}

			case 8:		//course
			{
				GPS_package.course *= 10;
				GPS_package.course += (c - 48);
				break;
			}

			case 9:		//date
			case 10:	//magnetic variation
			default:
			{
				break;
			}
		}//switch...case
	}//separator != buffer_in[index]

	GPS_counter++;
}
