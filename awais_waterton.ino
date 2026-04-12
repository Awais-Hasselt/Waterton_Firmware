#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMDNS.h>
#include <vector>

#define ACCESS_POINT_SSID "awais-connect"
#define MDNS_HOSTNAME "awais"
WiFiServer server(80);
WiFiUDP udp;
MDNS mdns(udp);



typedef struct SsidEntry{
  String ssid;
  long rssi;
};

typedef struct HttpRequest{
  String headerCommand;
  String body;
};
std::vector<SsidEntry> ssids;
#include "captive_portal_html.h"


std::vector<SsidEntry> getSsids(int numSsid){
  std::vector<SsidEntry> output;
  output.reserve(numSsid);
  for (int i = 0; i < numSsid; i++) {
    SsidEntry entry;
    entry.ssid = WiFi.SSID(i);
    entry.rssi = WiFi.RSSI(i);
    output.push_back(entry);
  }
  return output;
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Awais starting setup...");

  int numSsid = WiFi.scanNetworks();
  ssids = getSsids(numSsid);
  // Start Access Point
  Serial.print("Access Point: ");
  Serial.println(ACCESS_POINT_SSID);
  if (WiFi.beginAP(ACCESS_POINT_SSID) != WL_AP_LISTENING) {
    Serial.println("AP Setup Failed");
    while (true);
  }
  mdns.begin(WiFi.localIP(), MDNS_HOSTNAME);

  server.begin();
}

void loop() {
  mdns.run();
  WiFiClient client = server.available();
  if (!client) return;

  String requestStr = "";
  if (client.connected() && client.available()) {
    requestStr = client.readString();
  } else {
    return;
  }
  
  HttpRequest req = parseRequest(requestStr);
  Serial.print("got request: ");
  Serial.println(req.headerCommand);

  if (req.headerCommand == "GET /"){
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.print(getFullHtmlString(ssids));
  }
  delay(10);
  client.stop();
}




HttpRequest parseRequest(String request){
  String firstLine = request.substring(0, request.indexOf("\n"));
  int httpIndex = request.indexOf("HTTP");
  HttpRequest req;
  if (httpIndex == -1) return req;
  req.headerCommand = request.substring(0, httpIndex - 1);
  req.body = request.substring(request.indexOf("\n\n"));
  return req;
}
