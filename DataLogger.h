#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <Arduino.h>
#include <SD.h>

struct SensorData {
  float distance;
  bool motionDetected;
  bool magneticFieldDetected;
  double latitude;  // Add latitude
  double longitude;
  // Add more fields as needed
};

class DataLogger {
public:
  DataLogger(int chipSelectPin);
  bool init();
  void logData(const SensorData& data);

private:
  int chipSelectPin;
  File logFile;
};

#endif
