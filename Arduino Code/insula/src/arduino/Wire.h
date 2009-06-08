#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>
#include <string.h>

#define BUFFER_LENGTH 32

class TwoWire
{
public:
    TwoWire(){}
    void    begin             (void);
    void    begin             (uint8_t);
    void    beginTransmission (uint8_t);
    uint8_t endTransmission   (void);
    uint8_t requestFrom       (uint8_t, uint8_t);
    void    send              (uint8_t);
    void    send              (uint8_t*, uint8_t);
    uint8_t available         (void)							  {return rxBufferLength - rxBufferIndex;}
    uint8_t receive           (void);
    void    onReceive         (void (*)(int));
    void    onRequest         (void (*)(void));

    inline void     begin             (int address)	              {begin((uint8_t)address);}
    inline void     beginTransmission (int address)	              {beginTransmission((uint8_t)address);}
    inline uint8_t  requestFrom       (int address, int quantity) {return requestFrom((uint8_t)address, (uint8_t)quantity);}
    inline void     send              (int data) 				  {send((uint8_t)data);}
    inline void     send              (char* data) 				  {send((uint8_t*)data, strlen(data));}


private:
    static void    (*user_onRequest)  (void);
    static void    (*user_onReceive)  (int);
    static void     onRequestService  (void);
    static void     onReceiveService  (uint8_t*, int);


    static uint8_t* rxBuffer;
    static uint8_t  rxBufferIndex;
    static uint8_t  rxBufferLength;

    static uint8_t* txBuffer;
    static uint8_t  txBufferIndex;
    static uint8_t  txBufferLength;

    static uint8_t  txAddress;
    static uint8_t  transmitting;
};

//extern TwoWire Wire;

#endif

