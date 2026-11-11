#pragma once
#include <cstdint>
#include <deque>
#include <string>

using byte = uint8_t;
constexpr byte LOW=0, HIGH=1, OUTPUT=1;
constexpr byte A0=14, A1=15, A2=16, A3=17, A4=18, A5=19;
#define F(value) value
extern int digitalLevels[20], pwmValues[20], pinModes[20];
void analogWrite(byte pin, int value);
void digitalWrite(byte pin, byte value);
void pinMode(byte pin, byte mode);
void delayMicroseconds(unsigned int);
struct SerialMock {
  std::deque<char> input;
  std::string output;
  void begin(int) {}
  int available() { return static_cast<int>(input.size()); }
  char read() { char c=input.front(); input.pop_front(); return c; }
  void println(const char* text) { output += text; output += '\n'; }
};
extern SerialMock Serial;
