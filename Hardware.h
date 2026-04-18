#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>

class Hardware {
private:
  uint8_t _buttonPin;
  uint8_t _buzzerPin;

public:
  Hardware(uint8_t buttonPin, uint8_t buzzerPin);
  void begin();
  bool buttonPressed();
  void beep(int times);
};

#endif