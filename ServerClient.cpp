#include "ServerClient.h"

ServerClient::ServerClient() : _name(""), _host("") {}

void ServerClient::begin(String name, String host) {
    _name = name;
    _host = host;
}

ScheduleResponse ServerClient::ContactServer(uint32_t timestamp, int v_today, int v_tomorrow, int battery, float water) {
    ScheduleResponse res = {false, false, {0}, {0}};

    // Check if we are actually connected to a network first
    if (WiFi.status() != WL_CONNECTED) {
        return res; 
    }

    if (_client.connect(_host.c_str(), 80)) {
        // 1. Format the Request Body
        String body = _name + "\n";
        body += "t" + String(timestamp) + ", v" + String(v_today) + "|" + String(v_tomorrow);
        body += ", b" + String(battery) + ", w" + String(water);

        // 2. Send POST Headers
        _client.println("POST /api/status HTTP/1.1");
        _client.println("Host: " + _host);
        _client.println("Content-Type: text/plain");
        _client.print("Content-Length: ");
        _client.println(body.length());
        _client.println("Connection: close");
        _client.println();
        
        // 3. Send Body
        _client.print(body);

        // 4. Skip HTTP Response Headers
        while (_client.connected()) {
            String line = _client.readStringUntil('\n');
            if (line == "\r") break; 
        }

        // 5. Parse the Protocol Body (yy, yn, ny, nn)
        if (_client.available() >= 2) {
            char todayFlag = _client.read();
            char tomorrowFlag = _client.read();

            res.today_updated = (todayFlag == 'y');
            res.tomorrow_updated = (tomorrowFlag == 'y');

            if (res.today_updated) parseSchedule(res.schedule_today);
            if (res.tomorrow_updated) parseSchedule(res.schedule_tomorrow);
        }
        _client.stop();
    }
    return res;
}

void ServerClient::parseSchedule(Schedule &dest) {
    // Wait briefly for data to arrive if the connection is slow
    unsigned long timeout = millis();
    while(_client.available() < 51 && millis() - timeout < 2000) { delay(10); }

    char verBuf[4] = {0};
    for(int i=0; i<3; i++) verBuf[i] = _client.read();
    dest.version = atoi(verBuf);

    for(int i=0; i<48; i++) {
        if(_client.available()) {
            dest.slots[i] = _client.read();
        }
    }
}