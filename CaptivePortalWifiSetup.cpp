#include "CaptivePortalWifiSetup.h"
#include "captive_portal_html.h" // Ensure this uses the SsidEntry struct

CaptivePortalWifiSetup::CaptivePortalWifiSetup(CredentialStorage& storage, uint16_t port) : _server(port), _udp(), _mdns(_udp), _storage(storage) {}

void CaptivePortalWifiSetup::begin(const char* ssid, const char* hostname) {
  _scanNetworks();
  if (WiFi.beginAP(ssid) != WL_AP_LISTENING) {
    while (true); 
  }
  _mdns.begin(WiFi.localIP(), hostname);
  _server.begin();
  _state = PORTAL_ACTIVE;
}

void CaptivePortalWifiSetup::run() {
  if (_state == PORTAL_DONE) return;
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
  String requestStr = client.readString();
  HttpRequest req = _parseRequest(requestStr);
  
  if (req.headerCommand == "GET /") {
    client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
    client.print(getFullHtmlString(_ssids));
  } 
  else if (req.headerCommand == "POST /save") {
    // Extract parameters from the POST body
    String ssid = _urlDecode(_getParam(req.body, "ssid"));
    String pass = _urlDecode(_getParam(req.body, "password"));
    String name = _urlDecode(_getParam(req.body, "name"));

    if (ssid.length() > 0) {
      _storage.save(ssid.c_str(), pass.c_str(), name.c_str());
      
      client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
      client.print("<html><body><h1>Opslaan...</h1><p>De ton herstart nu.</p></body></html>");
      
      delay(500); 
      _state = PORTAL_DONE; // Signal the main loop to proceed
    }
  }
  
  delay(10);
  client.stop();
}

String CaptivePortalWifiSetup::_getParam(String body, String param) {
  int start = body.indexOf(param + "=");
  if (start == -1) return "";
  start += param.length() + 1;
  int end = body.indexOf("&", start);
  if (end == -1) end = body.length();
  return body.substring(start, end);
}

String CaptivePortalWifiSetup::_urlDecode(String str) {
  String decoded = "";
  char ch;
  int i, j;
  for (i = 0; i < str.length(); i++) {
    if (str[i] == '%') {
      sscanf(str.substring(i + 1, i + 3).c_str(), "%x", &j);
      ch = static_cast<char>(j);
      decoded += ch;
      i = i + 2;
    } else if (str[i] == '+') {
      decoded += ' ';
    } else {
      decoded += str[i];
    }
  }
  return decoded;
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