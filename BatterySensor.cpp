#include "BatterySensor.h"


BatterySensor::BatterySensor(uint8_t pin) : _pin(pin) {}

void BatterySensor::begin(){
  pinMode(_pin, INPUT);
  analogReadResolution(12);
}

float BatterySensor::getVoltage(){
  int rawADC = analogRead(_pin);
  float vOut = (rawADC / ADC_MAX) * V_REF;
  float vBat = vOut * VOLTAGE_DIVIDER_RATIO;
  return vBat;
}

int BatterySensor::getLevel(){
  float voltage = getVoltage();
  if (voltage <= V_LOW_BATTER) return 0;
  if (voltage >= V_FULL_BATTERY) return 5;
  int level = map(voltage * 100, V_LOW_BATTER * 100, V_FULL_BATTERY * 100, 0, 5);
  return constrain(level, 0, 5);
}