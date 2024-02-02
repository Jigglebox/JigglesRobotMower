#include "HallSensor.h"

HallSensor::HallSensor(int pin) {
  this->pin = pin;
}

void HallSensor::init() {
  pinMode(pin, INPUT);
}

bool HallSensor::isMagneticFieldDetected() {
  return digitalRead(pin) == LOW;  // Invert the logic here
}
