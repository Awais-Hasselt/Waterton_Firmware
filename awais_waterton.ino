#include "CaptivePortalWifiSetup.h"
#include "BatterySensor.h"
#include "CredentialStorage.h"
#include "Hardware.h"

#define BATTERY_PIN A0
#define BUZZER_PIN 2
#define BUTTON_PIN 3

Hardware hw(BUTTON_PIN, BUZZER_PIN);
CredentialStorage storage;
CaptivePortalWifiSetup portal(storage);
BatterySensor battery(BATTERY_PIN);

void setup() {
  hw.begin();
  battery.begin();
  storage.begin();

  hw.beep(1);

  // if no credentials stored, or reconfigure button is held --> (re)configure
  if(storage.isEmpty() || hw.buttonPressed()){
    portal.begin("awais-connect", "awais");
    hw.beep(2);
    while (portal.getState() != PORTAL_DONE) {
      portal.run();
    }
    hw.beep(3);
    NVIC_SystemReset();
  }
  
}

void loop() {
}



