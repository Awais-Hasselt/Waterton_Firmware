#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <WiFiNINA.h>
#include "CredentialStorage.h"

struct Schedule {
    int version;
    char slots[48]; // '-', '1', '2', '3', 'f'
};

struct ScheduleResponse {
    bool today_updated;
    bool tomorrow_updated;
    Schedule schedule_today;
    Schedule schedule_tomorrow;
};

class ServerClient {
private:
    WiFiClient _client;
    String _name;
    String _host;

    void parseSchedule(Schedule &dest);

public:
    // Simple constructor
    ServerClient();
    
    // Setup server details without touching WiFi hardware
    void begin(String name, String host);
    
    // Assumes WiFi is already connected before calling
    ScheduleResponse ContactServer(uint32_t timestamp, int v_today, int v_tomorrow, int battery, float water);
};

#endif