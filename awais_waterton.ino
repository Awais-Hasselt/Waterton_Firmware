#include "CaptivePortalWifiSetup.h"
#include "CredentialStorage.h"
#include "Hardware.h"
#include "ServerClient.h"
#include "PumpController.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include <RTCZero.h>

// Pins & Dimensions
#define BATTERY_PIN A0
#define BUZZER_PIN 2
#define BUTTON_PIN 3
#define ULTRASONE_TRIG_PIN 19 
#define ULTRASONE_ECHO_PIN 20 
#define POMP_PIN 21
#define CAPTIVE_PORTAL_HOSTNAME "awais"
#define CAPTIVE_PORTAL_SSID "awais-connect"

const float BARREL_RADIUS = 25.0;
const float BARREL_HEIGHT = 90.0;
const uint32_t POLL_INTERVAL = 5; // 5 minutes

// System Objects
Hardware hw(BUTTON_PIN, BUZZER_PIN, BATTERY_PIN, POMP_PIN, ULTRASONE_ECHO_PIN, ULTRASONE_TRIG_PIN);
CredentialStorage storage;
CaptivePortalWifiSetup portal(storage);
RTCZero rtc;
ServerClient server;
PumpController pumpCtrl;

// State Tracking
bool isManualOverride = false;
uint32_t nextPollTime = 0;

String name;

void setup() {
  Serial.begin(115200);
  // Wait up to 3 seconds for Serial Monitor to open
  uint32_t startSerial = millis();
  while (!Serial && millis() - startSerial < 3000);

  Serial.println("\n--- AWAIS Water Barrel System Startup ---");

  hw.begin();
  storage.begin();
  hw.togglePump(false);
  
  hw.beep(1); 

  if(storage.isEmpty() || hw.buttonPressed()) {
    Serial.println("ENTRY CONDITION: Storage empty or button held. Starting Portal...");
    captivePortalWorkflow();
  }

  Serial.println("Connecting to Network...");
  connectToWiFi();
  rtc.begin();
  server.begin(storage.getData().name);
  
  Serial.print("Getting public unix timestamp... ");
  uint32_t timestamp_ref = server.getPublicEpoch();
  Serial.print(" --> ");
  Serial.println(timestamp_ref);
  Serial.println("Syncing RTC with timestamp");
  rtc.setEpoch(timestamp_ref);
  hw.beep(3); 
  
  nextPollTime = rtc.getEpoch() + 5; 
  Serial.println("Setup Complete. Entering Loop.\n");
}

void loop() {
  uint32_t now = rtc.getEpoch();

  // ==========================================
  // 1. HANDLE BUTTON
  // ==========================================
  if (hw.buttonPressed()) {
    if (isManualOverride) {
      Serial.println("BUTTON: Manual Override CANCELLED.");
      isManualOverride = false;
      hw.togglePump(false);
      hw.beep(2);
      while(hw.buttonPressed()); 
    } else {
      Serial.println("BUTTON: Press detected, checking for 2s hold...");
      hw.beep(1); 
      uint32_t pressStart = millis();
      bool heldLongEnough = true;

      while (millis() - pressStart < 2000) {
        if (!hw.buttonPressed()) {
          heldLongEnough = false;
          Serial.println("BUTTON: Released too early.");
          break; 
        }
        delay(10); 
      }

      if (heldLongEnough) {
        Serial.println("BUTTON: 2s hold confirmed. Manual Override ACTIVE.");
        isManualOverride = true;
        hw.togglePump(true);
        hw.beep(2); 
        while(hw.buttonPressed()); 
      }
    }
  }

  if (isManualOverride) {
    delay(50);
    return; 
  }

  // ==========================================
  // 2. SERVER POLLING
  // ==========================================
  if (now >= nextPollTime) {
    Serial.println("\n--- POLLING SERVER ---");
    Serial.print("name:"); Serial.println(name);
    pollServer(now);
    nextPollTime = now + POLL_INTERVAL;
    Serial.println("Next poll at epoch: " + String(nextPollTime));
  }

  // ==========================================
  // 3. PUMP SCHEDULING
  // ==========================================
  bool shouldBeActive = pumpCtrl.isPumpActive(now);
  static bool lastPumpState = false;

  if (shouldBeActive != lastPumpState) {
    Serial.print("SCHEDULE: Pump state changed to: ");
    Serial.println(shouldBeActive ? "ON" : "OFF");
    lastPumpState = shouldBeActive;
  }

  hw.togglePump(shouldBeActive);

  delay(100); 
}

// ==========================================
// HELPERS
// ==========================================

void pollServer(uint32_t currentEpoch) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost. Reconnecting...");
    connectToWiFi();
  }

  int batLevel = hw.getBatteryLevel();
  int distance = hw.getWaterDistance();
  float waterHeight = BARREL_HEIGHT - distance;
  if (waterHeight < 0) waterHeight = 0; 
  float volumeLiters = (3.14159 * pow(BARREL_RADIUS, 2) * waterHeight) / 1000.0;

  Serial.print("Data: Bat="); Serial.print(batLevel);
  Serial.print(", Dist="); Serial.print(distance);
  Serial.print("cm, Vol="); Serial.print(volumeLiters); Serial.println("L");

  ScheduleResponse res = server.ContactServer(currentEpoch, 
                                              pumpCtrl.getTodayVersion(), 
                                              pumpCtrl.getTomorrowVersion(), 
                                              batLevel, 
                                              volumeLiters);

  if (res.today_updated) Serial.println("SERVER: Today's schedule updated!");
  if (res.tomorrow_updated) Serial.println("SERVER: Tomorrow's schedule updated!");
  
  if (res.today_updated || res.tomorrow_updated) {
    pumpCtrl.updateSchedules(res.schedule_today, res.schedule_tomorrow);
    hw.beep(2); 
  } else {
    Serial.println("SERVER: No schedule updates available.");
  }
}

void connectToWiFi() {
  DeviceConfig conf = storage.getData();
  name = conf.name;
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.println("\n--- Initializing WiFi ---");

  // 1. FORCED RESET: If the module is confused, we shut it down first
  WiFi.end(); 
  delay(500); 

  // 2. TIMEOUT LOOP: We try to connect multiple times
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 3) {
    attempts++;
    Serial.print("Attempt "); Serial.print(attempts);
    Serial.print(" to connect to: "); Serial.println(conf.ssid);

    // Some routers need a "firm" begin. 
    // Passing SSID/Pass explicitly even if stored helps.
    WiFi.begin(conf.ssid, conf.password);

    // 3. PATIENCE: Wait up to 10 seconds per attempt
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\nFailed attempt. Retrying...");
      WiFi.end(); // Clean the slate again
      delay(1000);
    }
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected! IP: " + WiFi.localIP().toString());
    hw.beep(2);
  } else {
    Serial.println("\nCRITICAL: WiFi failed after 3 attempts.");
    hw.beep(10); // Long series of beeps for error
  }
}

void captivePortalWorkflow() {
  Serial.println("PORTAL: Initializing...");
  portal.begin(CAPTIVE_PORTAL_SSID, CAPTIVE_PORTAL_HOSTNAME);
  hw.beep(2); 
  while (portal.getState() != PORTAL_DONE) {
    portal.run();
    // Minor delay to keep loop from hogging CPU
    delay(1); 
  }
  Serial.println("PORTAL: Setup Complete. Resetting System...");
  hw.beep(3); 
  delay(1000);
  NVIC_SystemReset();
}