#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include "BatterySensor.h"

class Hardware {
private:
  uint8_t _buttonPin;
  uint8_t _buzzerPin;
  uint8_t _pumpPin;
  uint8_t _ultraSoneEchoPin;
  uint8_t _ultraSoneTrigPin;
  BatterySensor _battery;

public:
  Hardware(uint8_t buttonPin, uint8_t buzzerPin, uint8_t batteryPin, uint8_t pumpPin, uint8_t ultraSoneEchoPin, uint8_t ultraSoneTrigPin);
  void begin();
  bool buttonPressed();
  void beep(int times);
  int getBatteryLevel();
  int getWaterDistance();
  void togglePump(bool pumpOn);
};

#endif