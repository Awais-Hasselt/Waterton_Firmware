#ifndef CAPTIVE_PORTAL_WIFI_SETUP_H
#define CAPTIVE_PORTALWIFI_SETUP_H

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMDNS.h>
#include <vector>

struct SsidEntry {
  String ssid;
  long rssi;
};

struct HttpRequest {
  String headerCommand;
  String body;
};

class CaptivePortalWifiSetup {
public:
  CaptivePortalWifiSetup(uint16_t port = 80);
  
  void begin(const char* ssid, const char* hostname);
  void run();

private:
  WiFiServer _server;
  WiFiUDP _udp;
  MDNS _mdns;
  std::vector<SsidEntry> _ssids;
  
  void _scanNetworks();
  HttpRequest _parseRequest(String request);
  void _handleClient(WiFiClient& client);
};

#endif // CAPTIVE_PORTALWIFI_SETUP_H