#include "PIRSensor.h"

PIRSensor::PIRSensor(int pin) {
  this->pin = pin;
}

void PIRSensor::init() {
  pinMode(pin, INPUT);
}

bool PIRSensor::isMotionDetected() {
  return digitalRead(pin) == HIGH;
}
