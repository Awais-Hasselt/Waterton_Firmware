#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <WiFiNINA.h>

struct Schedule {
    int version;
    char slots[48]; 
};

struct ScheduleResponse {
    bool today_updated;
    bool tomorrow_updated;
    Schedule schedule_today;
    Schedule schedule_tomorrow;
};

class ServerClient {
private:
    WiFiSSLClient _sslClient; // Changed to SSL
    String _name;
    String _host;

    void parseSchedule(Schedule &dest);

public:
    ServerClient();
    void begin(String name, String host);
    uint32_t getPublicEpoch();
    ScheduleResponse ContactServer(uint32_t timestamp, int v_today, int v_tomorrow, int battery, float water);
};

#endif