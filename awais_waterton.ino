#include <SPI.h>
#include <WiFiNINA.h>

#define ACCESS_POINT_SSID "awais-connect"
WiFiServer server(80);

typedef struct HttpRequest{
  String headerCommand;
  String body;
};

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Awais starting setup...");

  // Start Access Point
  Serial.print("Access Point: ");
  Serial.println(ACCESS_POINT_SSID);
  if (WiFi.beginAP(ACCESS_POINT_SSID) != WL_AP_LISTENING) {
    Serial.println("AP Setup Failed");
    while (true);
  }

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String requestStr = "";
  while (client.connected() && client.available()) {
    requestStr = client.readString();
  }

  HttpRequest req = parseRequest(requestStr);
  Serial.print("got request: ");
  Serial.println(req.headerCommand);

  if (req.headerCommand == "GET /"){
    Serial.println("yes");
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("hello");
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
