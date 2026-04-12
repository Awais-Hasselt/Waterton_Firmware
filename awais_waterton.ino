#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMDNS.h>

#define ACCESS_POINT_SSID "awais-connect"
#define MDNS_HOSTNAME "awais"
WiFiServer server(80);
WiFiUDP udp;
MDNS mdns(udp);
int numSsid;


typedef struct HttpRequest{
  String headerCommand;
  String body;
};

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Awais starting setup...");

  numSsid = WiFi.scanNetworks();
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
    String ssidsJson = "";
    for (int i = 0; i < numSsid; i++) {
      ssidsJson += "\"";
      ssidsJson += WiFi.SSID(i);
      ssidsJson += "\"";
      if (i < numSsid - 1) {
        ssidsJson += ",";
      }
    }
    client.print("ssids:[");
    client.print(ssidsJson);
    client.println("]}");
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
