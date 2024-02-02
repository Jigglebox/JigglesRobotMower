#ifndef SERIALCOMMANDHANDLER_H
#define SERIALCOMMANDHANDLER_H

#include <Arduino.h>

class SerialCommandHandler {
public:
  SerialCommandHandler();
  void checkCommands();
  void showHelp();
  bool isSensorOutputEnabled();
  bool isGPSOutputEnabled();
  void handleReloadCommand();
  void toggleSensorOutput();
  void toggleGPSOutput();

private:
  bool showSensorOutput;
  bool showGPSOutput;
};

#endif
