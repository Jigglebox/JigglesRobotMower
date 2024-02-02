#ifndef HALLSENSOR_H
#define HALLSENSOR_H

#include <Arduino.h>

class HallSensor {
public:
  HallSensor(int pin);
  void init();
  bool isMagneticFieldDetected();

private:
  int pin;
};

#endif
