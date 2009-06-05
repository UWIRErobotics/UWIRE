#ifndef Print_h
#define Print_h

#include <inttypes.h>
#include <stdio.h> // for size_t

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

class Print
{
  private:
    void printNumber(unsigned long, uint8_t);
    void printFloat(double, uint8_t);
  public:
    virtual void write(uint8_t);
    virtual void write(const char *str);
    virtual void write(const uint8_t *buffer, size_t size);
    void print(char);
    void print(const char[]);
    void print(uint8_t);
    void print(int);
    void print(unsigned int);
    void print(long);
    void print(unsigned long);
    void print(long, int);
    void print(double);
    void println(void);
    void println(char);
    void println(const char[]);
    void println(uint8_t);
    void println(int);
    void println(unsigned int);
    void println(long);
    void println(unsigned long);
    void println(long, int);
    void println(double);
};

#endif
