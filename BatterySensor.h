#ifndef BATTERY_SENSOR_H
#define BATTERY_SENSOR_H

#include <Arduino.h>

#define V_REF 3.3
#define V_FULL_BATTERY 12.6
#define V_LOW_BATTER 12.0
#define ADC_MAX 4095 // 2^12
#define VOLTAGE_DIVIDER_RATIO ((27000.0 + 6800.0) / 6800.0)

class BatterySensor {
  private:
    uint8_t _pin;

  public:
    BatterySensor(uint8_t pin);
    void begin();
    float getVoltage();
    int getLevel();
};

#endif