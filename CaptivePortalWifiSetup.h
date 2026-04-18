#ifndef CAPTIVE_PORTAL_WIFI_SETUP_H
#define CAPTIVE_PORTAL_WIFI_SETUP_H

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMDNS.h>
#include <vector>
#include "CredentialStorage.h"

enum PortalState { PORTAL_ACTIVE, PORTAL_DONE };

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
  // Pass storage by reference to save resources/share with main app
  CaptivePortalWifiSetup(CredentialStorage& storage, uint16_t port = 80);
  
  void begin(const char* ssid, const char* hostname);
  void run();
  PortalState getState() { return _state; }

private:
  WiFiServer _server;
  WiFiUDP _udp;
  MDNS _mdns;
  CredentialStorage& _storage;
  std::vector<SsidEntry> _ssids;
  PortalState _state = PORTAL_ACTIVE;
  
  void _scanNetworks();
  HttpRequest _parseRequest(String request);
  void _handleClient(WiFiClient& client);
  String _getParam(String body, String param);
  String _urlDecode(String str);
};

#endif