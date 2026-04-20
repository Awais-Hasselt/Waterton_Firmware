#include "PumpController.h"

PumpController::PumpController() {
    _today.version = 0;
    _tomorrow.version = 0;
    for(int i = 0; i < 48; i++) {
        _today.slots[i] = '-';
        _tomorrow.slots[i] = '-';
    }
}

void PumpController::updateSchedules(Schedule today, Schedule tomorrow) {
    _today = today;
    _tomorrow = tomorrow;
}

int PumpController::getTodayVersion() { return _today.version; }
int PumpController::getTomorrowVersion() { return _tomorrow.version; }

// --- Timing Translators ---
uint32_t PumpController::getCycleLength(char p) {
    if (p == '1') return 300; // 5 min cycle
    if (p == '2') return 120; // 2 min cycle
    if (p == '3') return 60;  // 1 min cycle
    return 1800; // Default to 30 min slot
}

uint32_t PumpController::getOnDuration(char p) {
    if (p == '1') return 5;   // 5s ON
    if (p == '2') return 10;  // 10s ON
    if (p == '3') return 20;  // 20s ON
    if (p == 'f') return 1800;// Always ON
    return 0;                 // '-'
}

// --- Logic ---
bool PumpController::isPumpActive(uint32_t currentEpoch) {
    uint32_t secondsToday = currentEpoch % 86400;
    int slotIndex = secondsToday / 1800; // 1800s = 30 mins
    char p = _today.slots[slotIndex];

    if (p == '-' || p == '\0') return false;
    if (p == 'f') return true;

    // Modulo math perfectly aligns bursts with the wall clock
    uint32_t cycleLen = getCycleLength(p);
    uint32_t onDur = getOnDuration(p);
    uint32_t posInCycle = currentEpoch % cycleLen;

    return (posInCycle < onDur);
}

uint32_t PumpController::getNextWakeTime(uint32_t currentEpoch) {
    uint32_t secondsToday = currentEpoch % 86400;
    int slotIndex = secondsToday / 1800;
    char p = _today.slots[slotIndex];
    
    // Calculate the absolute end of the current 30-min slot
    uint32_t endOfSlot = currentEpoch + (1800 - (secondsToday % 1800));

    if (p == '-' || p == 'f' || p == '\0') {
        return endOfSlot; // Only wake up when the 30-min block changes
    }

    uint32_t cycleLen = getCycleLength(p);
    uint32_t onDur = getOnDuration(p);
    uint32_t posInCycle = currentEpoch % cycleLen;

    uint32_t nextTransition = 0;
    if (posInCycle < onDur) {
        // Pump is ON, next wake is when it turns OFF
        nextTransition = currentEpoch + (onDur - posInCycle);
    } else {
        // Pump is OFF, next wake is when it turns ON
        nextTransition = currentEpoch + (cycleLen - posInCycle);
    }

    // Clamp the next wake time so it doesn't bleed into the next 30-min slot
    return (nextTransition < endOfSlot) ? nextTransition : endOfSlot;
}