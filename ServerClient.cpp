#include "ServerClient.h"

ServerClient::ServerClient() : _name(""), _host("") {}

void ServerClient::begin(String name, String host) {
    _name = name;
    _host = host;
}

uint32_t ServerClient::getPublicEpoch() {
    WiFiSSLClient timeClient; 
    const char* timeHost = "aisenseapi.com";
    
    if (!timeClient.connect(timeHost, 443)) return 0;

    timeClient.println("GET /services/v1/timestamp HTTP/1.1");
    timeClient.println("Host: aisenseapi.com");
    timeClient.println("Connection: close");
    timeClient.println();

    while (timeClient.connected()) {
        String line = timeClient.readStringUntil('\n');
        if (line == "\r") break;
    }

    uint32_t foundTime = 0;
    if (timeClient.find("\"timestamp\":")) {
        foundTime = timeClient.parseInt();
    }
    timeClient.stop();
    return foundTime;
}

ScheduleResponse ServerClient::ContactServer(uint32_t timestamp, int v_today, int v_tomorrow, int battery, float water) {
    ScheduleResponse res = {false, false, {0}, {0}};

    if (WiFi.status() != WL_CONNECTED) return res; 

    // Connect via HTTPS (Port 443)
    if (_sslClient.connect(_host.c_str(), 443)) {
        String body = _name + "\n";
        body += "t" + String(timestamp) + ", v" + String(v_today) + "|" + String(v_tomorrow);
        body += ", b" + String(battery) + ", w" + String(water);

        _sslClient.println("POST /api/status HTTP/1.1");
        _sslClient.println("Host: " + _host);
        _sslClient.println("Content-Type: text/plain");
        _sslClient.print("Content-Length: ");
        _sslClient.println(body.length());
        _sslClient.println("Connection: close");
        _sslClient.println();
        _sslClient.print(body);

        // Skip Headers
        while (_sslClient.connected()) {
            String line = _sslClient.readStringUntil('\n');
            if (line == "\r") break; 
        }

        // Parse protocol body
        if (_sslClient.available() >= 2) {
            char todayFlag = _sslClient.read();
            char tomorrowFlag = _sslClient.read();

            res.today_updated = (todayFlag == 'y');
            res.tomorrow_updated = (tomorrowFlag == 'y');

            if (res.today_updated) parseSchedule(res.schedule_today);
            if (res.tomorrow_updated) parseSchedule(res.schedule_tomorrow);
        }
        _sslClient.stop();
    }
    return res;
}

void ServerClient::parseSchedule(Schedule &dest) {
    unsigned long timeout = millis();
    // SSL can be slower, so we wait up to 3 seconds for the data packet
    while(_sslClient.available() < 51 && millis() - timeout < 3000) { delay(10); }

    char verBuf[4] = {0};
    for(int i=0; i<3; i++) verBuf[i] = _sslClient.read();
    dest.version = atoi(verBuf);

    for(int i=0; i<48; i++) {
        if(_sslClient.available()) {
            dest.slots[i] = _sslClient.read();
        }
    }
}