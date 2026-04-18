#include "CredentialStorage.h"
#include <FlashStorage_SAMD.h>

FlashStorage(flash_store, DeviceConfig);

CredentialStorage::CredentialStorage() {}

void CredentialStorage::begin() {
  flash_store.read(_config);
}

bool CredentialStorage::isEmpty() {
  return !_config.isConfigured;
}

DeviceConfig CredentialStorage::getData() {
  return _config;
}

void CredentialStorage::save(const char* ssid, const char* password, const char* name) {
  memset(&_config, 0, sizeof(DeviceConfig)); 
  
  strncpy(_config.ssid, ssid, sizeof(_config.ssid) - 1);
  strncpy(_config.password, password, sizeof(_config.password) - 1);
  strncpy(_config.name, name, sizeof(_config.name) - 1);
  
  _config.isConfigured = true;
  
  flash_store.write(_config);
}

void CredentialStorage::clear() {
  _config.isConfigured = false;
  flash_store.write(_config);
}