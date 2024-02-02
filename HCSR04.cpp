#include "HCSR04.h"

HCSR04::HCSR04(int triggerPin, int echoPin) {
  this->triggerPin = triggerPin;
  this->echoPin = echoPin;
}

void HCSR04::init() {
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float HCSR04::getDistance() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}
