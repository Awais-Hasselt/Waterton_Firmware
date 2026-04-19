#include "Hardware.h"

Hardware::Hardware(uint8_t buttonPin, uint8_t buzzerPin, uint8_t batteryPin, uint8_t pumpPin, uint8_t ultraSoneEchoPin, uint8_t ultraSoneTrigPin)
  : _buttonPin(buttonPin), _buzzerPin(buzzerPin), _battery(batteryPin), _pumpPin(pumpPin), _ultraSoneEchoPin(ultraSoneEchoPin), _ultraSoneTrigPin(ultraSoneTrigPin)
  {}

void Hardware::begin(){
  pinMode(_buttonPin, INPUT);
  pinMode(_buzzerPin, OUTPUT);
  pinMode(_pumpPin, OUTPUT);
  pinMode(_ultraSoneTrigPin, OUTPUT);
  pinMode(_ultraSoneEchoPin, INPUT);
  _battery.begin();
}

bool Hardware::buttonPressed(){
  return digitalRead(_buttonPin);
}

void Hardware::beep(int times){
  for(int i = 0; i<times; i++){
    digitalWrite(_buzzerPin, HIGH);
    delay(100);
    digitalWrite(_buzzerPin, LOW);
    if(i < times - 1){
      delay(100);
    }
  }
}

int Hardware::getBatteryLevel(){
  return _battery.getLevel();
}

int Hardware::getWaterDistance(){
  digitalWrite(_ultraSoneTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(_ultraSoneTrigPin,  HIGH);
  delayMicroseconds(10);
  digitalWrite(_ultraSoneTrigPin, LOW);
  const unsigned long duration = pulseIn(_ultraSoneEchoPin, HIGH);
  return duration/29/2;
}

void Hardware::togglePump(bool pumpOn){
  digitalWrite(_pumpPin, pumpOn);
}