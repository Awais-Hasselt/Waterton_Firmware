struct Schedule {
  int version;
  char schedule[48];  // 48 slots of each a half hour
};

enum SchedulePumpMode {
  OFF,
  SIP,
  STEADY,
  HEAVY,
  FLOOD
}