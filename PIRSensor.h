#ifndef PIRSENSOR_H
#define PIRSENSOR_H

#include <Arduino.h>

class PIRSensor {
public:
  PIRSensor(int pin);
  void init();
  bool isMotionDetected();

private:
  int pin;
};

#endif
