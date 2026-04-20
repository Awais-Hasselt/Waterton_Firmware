#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <Arduino.h>
#include "ServerClient.h" // Includes the Schedule struct

class PumpController {
private:
    Schedule _today;
    Schedule _tomorrow;

    // Helper functions to decode the characters into seconds
    uint32_t getCycleLength(char p);
    uint32_t getOnDuration(char p);

public:
    PumpController();
    
    // Load new schedules from the server
    void updateSchedules(Schedule today, Schedule tomorrow);
    
    // Returns the current version for the request body
    int getTodayVersion();
    int getTomorrowVersion();

    // Tells the main loop if the pump should be HIGH or LOW right now
    bool isPumpActive(uint32_t currentEpoch);
    
    // Calculates the exact timestamp of the next transition (burst or slot change)
    uint32_t getNextWakeTime(uint32_t currentEpoch);
};

#endif