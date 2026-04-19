#include "CaptivePortalWifiSetup.h"
#include "CredentialStorage.h"
#include "Hardware.h"

#define BATTERY_PIN A0
#define BUZZER_PIN 2
#define BUTTON_PIN 3
#define ULTRASONE_TRIG_PIN 19 // geel
#define ULTRASONE_ECHO_PIN 20 // oranje
#define POMP_PIN 21
#define CAPTIVE_PORTAL_HOSTNAME "awais"
#define CAPTIVE_PORTAL_SSID "awais-connect"

Hardware hw(BUTTON_PIN, BUZZER_PIN, BATTERY_PIN, POMP_PIN, ULTRASONE_ECHO_PIN, ULTRASONE_TRIG_PIN);
CredentialStorage storage;
CaptivePortalWifiSetup portal(storage);

void setup() {
  hw.begin();
  storage.begin();

  hw.togglePump(false);
  hw.beep(1); // 1 beep = system startuo

  // if no credentials stored, or button is held --> (re)configure
  if(storage.isEmpty() || hw.buttonPressed()){
    captivePortalWorkflow();
  }
}

void loop() {
}


void captivePortalWorkflow(){
  portal.begin(CAPTIVE_PORTAL_SSID, CAPTIVE_PORTAL_HOSTNAME);
    hw.beep(2); // 2 beep = entered captive portal mode
    while (portal.getState() != PORTAL_DONE) {
      portal.run();
    }
    hw.beep(3); // 3 beep in captive portal mode = success
    delay(1000);
    NVIC_SystemReset();
}
