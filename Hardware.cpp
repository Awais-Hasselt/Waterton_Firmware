#include "Hardware.h"

Hardware::Hardware(uint8_t buttonPin, uint8_t buzzerPin) : _buttonPin(buttonPin), _buzzerPin(buzzerPin) {}

void Hardware::begin(){
  pinMode(_buttonPin, INPUT);
  pinMode(_buzzerPin, OUTPUT);
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