#include "DataLogger.h"

DataLogger::DataLogger(int chipSelectPin)
  : chipSelectPin(chipSelectPin) {}

bool DataLogger::init() {
  if (!SD.begin(chipSelectPin)) {
    Serial.println("SD Card initialization failed!");
    return false;
  }
  logFile = SD.open("datalog.txt", FILE_WRITE);
  if (!logFile) {
    Serial.println("Error opening datalog.txt");
    return false;
  }
  return true;
}

void DataLogger::logData(const SensorData& data) {
  if (logFile) {
    logFile.print("Distance: ");
    logFile.print(data.distance);
    logFile.print(", Motion: ");
    logFile.print(data.motionDetected ? "Yes" : "No");
    logFile.print(", Magnetic Field: ");
    logFile.println(data.magneticFieldDetected ? "Yes" : "No");
    logFile.print(", Latitude: ");
    logFile.print(data.latitude, 6);
    logFile.print(", Longitude: ");
    logFile.println(data.longitude, 6);
    logFile.flush();  // Ensure data is written to the card
  }
}
