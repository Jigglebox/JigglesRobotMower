#include "GPSModule.h"

TinyGPSPlus GPSModule::gps;

void GPSModule::init() {
  Serial1.begin(9600);  // Use hardware serial
}

void GPSModule::update() {
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }
}

double GPSModule::getLatitude() {
  return gps.location.isValid() ? gps.location.lat() : 0.0;
}

double GPSModule::getLongitude() {
  return gps.location.isValid() ? gps.location.lng() : 0.0;
}
