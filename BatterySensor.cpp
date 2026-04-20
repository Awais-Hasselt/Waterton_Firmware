#include "BatterySensor.h"

BatterySensor::BatterySensor(uint8_t pin) : _pin(pin) {}

void BatterySensor::begin(){
  pinMode(_pin, INPUT);
  analogReadResolution(12);
}

float BatterySensor::getVoltage(){
  int rawADC = analogRead(_pin);
  Serial.print("bat analog read:");
  Serial.println(rawADC);
  float vOut = (rawADC / ADC_MAX) * V_REF;
  float vBat = vOut * VOLTAGE_DIVIDER_RATIO;
  return vBat;
}

int BatterySensor::getLevel(){
  // float voltage = getVoltage();
  int rawADC = analogRead(_pin);
  float vOut = (rawADC / 4095.0) * 3.3;
  float voltage = 4.9705 * vOut;
  
  Serial.print("bat voltage:");
  Serial.println(voltage);
  if (voltage <= V_LOW_BATTER) return 0;
  if (voltage >= V_FULL_BATTERY) return 5;
  int level = map(voltage * 100, V_LOW_BATTER * 100, V_FULL_BATTERY * 100, 0, 5);
  level = constrain(level, 0, 5);
  Serial.print("bat voltage:");
  Serial.println(level);
  return level;
}