#include "CaptivePortalWifiSetup.h"
#include "captive_portal_html.h" // Ensure this uses the SsidEntry struct

CaptivePortalWifiSetup::CaptivePortalWifiSetup(uint16_t port) : _server(port), _udp(), _mdns(_udp) {}

void CaptivePortalWifiSetup::begin(const char* ssid, const char* hostname) {
  Serial.begin(9600);
  delay(1000);
  
  // Scan before starting AP to ensure radio is available
  _scanNetworks();

  Serial.print("Starting AP: ");
  Serial.println(ssid);
  if (WiFi.beginAP(ssid) != WL_AP_LISTENING) {
    Serial.println("AP Setup Failed");
    while (true);
  }

  _mdns.begin(WiFi.localIP(), hostname);
  _server.begin();
  Serial.println("Captive Portal Ready.");
}

void CaptivePortalWifiSetup::run() {
  _mdns.run();
  WiFiClient client = _server.available();
  if (client) {
    _handleClient(client);
  }
}

void CaptivePortalWifiSetup::_scanNetworks() {
  int numSsid = WiFi.scanNetworks();
  _ssids.clear();
  for (int i = 0; i < numSsid; i++) {
    _ssids.push_back({WiFi.SSID(i), WiFi.RSSI(i)});
  }
}

void CaptivePortalWifiSetup::_handleClient(WiFiClient& client) {
  String requestStr = "";
  if (client.connected() && client.available()) {
    requestStr = client.readString();
  } else {
    return;
  }

  HttpRequest req = _parseRequest(requestStr);
  
  if (req.headerCommand == "GET /") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    client.print(getFullHtmlString(_ssids));
  }
  
  delay(10);
  client.stop();
}

HttpRequest CaptivePortalWifiSetup::_parseRequest(String request) {
  HttpRequest req;
  int httpIndex = request.indexOf("HTTP");
  if (httpIndex != -1) {
    req.headerCommand = request.substring(0, httpIndex - 1);
    int bodyStart = request.indexOf("\r\n\r\n");
    if (bodyStart != -1) {
      req.body = request.substring(bodyStart + 4);
    }
  }
  return req;
}