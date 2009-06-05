#ifndef TwoWire_h
#define TwoWire_h
#include <inttypes.h>

#define BUFFER_LENGTH 32

class TwoWire
{
public:
    TwoWire();
    void    begin             (void);
    void    begin             (uint8_t);
    void    begin             (int);
    void    beginTransmission (uint8_t);
    void    beginTransmission (int);
    uint8_t endTransmission   (void);
    uint8_t requestFrom       (uint8_t, uint8_t);
    uint8_t requestFrom       (int, int);
    void    send              (uint8_t);
    void    send              (uint8_t*, uint8_t);
    void    send              (int);
    void    send              (char*);
    uint8_t available         (void);
    uint8_t receive           (void);
    void    onReceive         (void (*)(int));
    void    onRequest         (void (*)(void));

private:
    static uint8_t* rxBuffer;
    static uint8_t  rxBufferIndex;
    static uint8_t  rxBufferLength;

    static uint8_t  txAddress;
    static uint8_t* txBuffer;
    static uint8_t  txBufferIndex;
    static uint8_t  txBufferLength;

    static uint8_t  transmitting;
    static void (*user_onRequest)(void);
    static void (*user_onReceive)(int);
    static void onRequestService(void);
    static void onReceiveService(uint8_t*, int);
};

extern TwoWire Wire;

#endif

