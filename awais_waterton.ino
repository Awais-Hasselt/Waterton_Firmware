#include "CaptivePortalWifiSetup.h"

CaptivePortalWifiSetup portal;

void setup() {
  portal.begin("awais-connect", "awais");
}

void loop() {
  portal.run();
}