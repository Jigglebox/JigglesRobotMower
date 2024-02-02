#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <TinyGPS++.h>

class GPSModule {
public:
  static void init();
  static void update();
  static double getLatitude();
  static double getLongitude();

private:
  static TinyGPSPlus gps;
};

#endif
