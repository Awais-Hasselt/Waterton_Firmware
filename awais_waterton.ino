#include "CaptivePortalWifiSetup.h"
#include "BatterySensor.h"
#define BATTERY_PIN A0

CaptivePortalWifiSetup portal;
BatterySensor battery(BATTERY_PIN);

void setup() {
  BatterySensor.begin();
  portal.begin("awais-connect", "awais");
}

void loop() {
  portal.run();
}