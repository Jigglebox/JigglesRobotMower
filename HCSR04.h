#ifndef HCSR04_H
#define HCSR04_H

#include <Arduino.h>

class HCSR04 {
public:
  HCSR04(int triggerPin, int echoPin);
  void init();
  float getDistance();

private:
  int triggerPin;
  int echoPin;
  long duration, distance;
};

#endif
