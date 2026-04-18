#ifndef CREDENTIAL_STORAGE_H
#define CREDENTIAL_STORAGE_H

#include <Arduino.h>


// Struct to hold persistent data
// Fixed sizes ensure reliable storage in Flash memory
struct DeviceConfig {
  char ssid[33];      
  char password[64];  
  char name[20];
  bool isConfigured; 
};

class CredentialStorage {
  private:
    DeviceConfig _config;
    
  public:
    CredentialStorage();
    
    void begin();
    bool isEmpty();
    DeviceConfig getData();
    void save(const char* ssid, const char* password, const char* name);
    void clear();
};

#endif