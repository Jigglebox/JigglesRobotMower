#include "SerialCommandHandler.h"
#include <avr/wdt.h>  // Include the watchdog timer library

SerialCommandHandler::SerialCommandHandler()
  : showSensorOutput(false) {}


void SerialCommandHandler::checkCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any whitespace

    // Echo the command back to Serial Monitor
    Serial.print("Command received: ");
    Serial.println(command);

    if (command == "toggleSensorOutput") {
      toggleSensorOutput();
    } else if (command == "help") {
      showHelp();
    }
    // Add more commands here
  }
}


void SerialCommandHandler::toggleSensorOutput() {
  showSensorOutput = !showSensorOutput;
  Serial.println(showSensorOutput ? "Sensor output enabled" : "Sensor output disabled");
}

void SerialCommandHandler::toggleGPSOutput() {
  showGPSOutput = !showGPSOutput;
  Serial.println(showGPSOutput ? "GPS output enabled" : "GPS output disabled");
}

void SerialCommandHandler::showHelp() {
  Serial.println("  toggleSensorOutput          :Toggle sensor data serial output");
  Serial.println("  toggleGPSOutput             :Toggle GPS data serial output");
  Serial.println("  help                        :Show list of commands");
  Serial.println("  reload                      :Reboot the Arduino");
  // List other commands here
}

void SerialCommandHandler::handleReloadCommand() {
  wdt_enable(WDTO_15MS);  // Enable the watchdog timer for a 15 millisecond timeout
  while (true) {
    // Do nothing and wait for the watchdog timer to reset the Arduino
  }
}

bool SerialCommandHandler::isSensorOutputEnabled() {
  return showSensorOutput;
}

bool SerialCommandHandler::isGPSOutputEnabled() {
  return showGPSOutput;
}