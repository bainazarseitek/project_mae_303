#pragma once
// Compile-time driver tests only. No mocked hardware validation is claimed.
#include <stdint.h>
typedef uint8_t byte;
const byte LOW = 0, HIGH = 1, OUTPUT = 1;
const byte A0 = 14, A1 = 15, A2 = 16, A3 = 17, A4 = 18, A5 = 19;
#define F(value) value
void analogWrite(byte, int);
void digitalWrite(byte, byte);
void pinMode(byte, byte);
void delayMicroseconds(unsigned int);
struct SerialStub { void println(const char*); };
extern SerialStub Serial;
