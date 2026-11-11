#include <cassert>
#include <iostream>
#include "Arduino.h"
int digitalLevels[20]={}, pwmValues[20]={}, pinModes[20]={};
SerialMock Serial;
void analogWrite(byte pin,int value) { pwmValues[pin]=value; }
void digitalWrite(byte pin,byte value) { digitalLevels[pin]=value; }
void pinMode(byte pin,byte mode) { pinModes[pin]=mode; }
void delayMicroseconds(unsigned int) {}
#include "../../electronics/robot_bench/robot_bench.ino"

void assertStopped() {
  assert(!motorsRunning && pwmValues[5]==0 && pwmValues[6]==0);
  assert(digitalLevels[2]==LOW && digitalLevels[7]==LOW);
#if ROBOT_DRIVER_DRV8833
  assert(digitalLevels[8]==LOW);
#else
  assert(digitalLevels[4]==LOW && digitalLevels[8]==LOW);
#endif
}

int main() {
  setup(); assertStopped();
  assert(Serial.output.find("TWO-MOTOR CONTINUOUS")!=std::string::npos);
  Serial.input={'r','\r','\n'}; loop();
  assert(motorsRunning && pwmValues[5]==255 && pwmValues[6]==255);
#if ROBOT_DRIVER_DRV8833
  assert(digitalLevels[2]==LOW && digitalLevels[7]==LOW && digitalLevels[8]==HIGH);
#else
  assert(digitalLevels[2]==HIGH && digitalLevels[4]==LOW);
  assert(digitalLevels[7]==HIGH && digitalLevels[8]==LOW);
#endif
  for(int i=0;i<10000;++i) loop(); // no timeout or jog expiry
  command('v'); command('h'); assert(motorsRunning);
  command('x'); assertStopped();
  command('r'); command('1'); assertStopped(); // obsolete commands fail stopped
  command('r'); command('2'); assertStopped();
  command('r'); command('g'); assertStopped(); // no autonomous route remains
  command('r'); command('o'); assertStopped(); // no servo control remains
  Serial.input={'r','x','\n'}; loop(); assertStopped();
  std::cout << "PASS: production robot_bench boot, full-duty BOTH drive, normal pin map, continuous operation, stop, obsolete commands and Serial input.\n";
}
