//GPSMower.ino

#include "HallSensor.h"
#include "HCSR04.h"
#include "PIRSensor.h"
#include "DataLogger.h"
#include "SerialCommandHandler.h"
#include "GPSModule.h"
#include <avr/wdt.h>

DataLogger dataLogger(4);  // Initialize with the chip select pin for the SD card
HCSR04 ultrasonic(8, 9);   // Echo - 9, Trig - 8
PIRSensor pir(2);          // S - 2
HallSensor hall(3);        // S - 3
SerialCommandHandler commandHandler;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // Wait for serial port to connect (needed for boards with native USB)
  }

  // Print sensor pin configuration
  Serial.println();
  Serial.println();
  Serial.println("----------------    PIN CONFIG    ----------------");
  Serial.println();
  Serial.println("SD Card -               CS Pin: 4");
  Serial.println("Ultrasonic Sensor -     Echo Pin: 9, Trig Pin: 8");
  Serial.println("PIR Sensor -            S Pin: 2");
  Serial.println("Hall Sensor -           S Pin: 3");
  Serial.println("GPS -                   RX Pin: 18, TX Pin: 19");
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("----------------    COMMANDS    ----------------");
  Serial.println();
  Serial.println("  toggleSensorOutput          :Toggle sensor data serial output");
  Serial.println("  toggleGPSOutput             :Toggle GPS data serial output");
  Serial.println("  help                        :Show list of commands");
  Serial.println("  reload                      :Reboot the Arduino");
  Serial.println();
  Serial.println("----------------------------------------------------");
  Serial.println();
  Serial.println();

  pinMode(10, OUTPUT);     // Have to do this so the SDcard works. DO NOT USE PIN 10
  digitalWrite(10, HIGH);  //

  if (!dataLogger.init()) {
    Serial.println("DataLogger failed to initialize.");
  }
  ultrasonic.init();
  pir.init();
  hall.init();
  GPSModule::init();
}





void loop() {
  commandHandler.checkCommands();
  SensorData data;

  // Collect data from sensors
  data.distance = ultrasonic.getDistance();
  data.motionDetected = pir.isMotionDetected();
  data.magneticFieldDetected = hall.isMagneticFieldDetected();

  // Update GPS data
  GPSModule::update();
  data.latitude = GPSModule::getLatitude();
  data.longitude = GPSModule::getLongitude();

  // Check for serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any whitespace

    if (command == "toggleSensorOutput") {
      commandHandler.toggleSensorOutput();
    } else if (command == "toggleGPSOutput") {
      commandHandler.toggleGPSOutput();
    } else if (command == "help") {
      commandHandler.showHelp();
    } else if (command == "reload") {
      commandHandler.handleReloadCommand();
    }
  }

  // Print GPS data if output is enabled
  if (commandHandler.isGPSOutputEnabled()) {
    Serial.print("Latitude: ");
    Serial.print(data.latitude, 6);
    Serial.print(", Longitude: ");
    Serial.println(data.longitude, 6);
  }

  // Log the collected data
  dataLogger.logData(data);

  // Print sensor data if output is enabled
  if (commandHandler.isSensorOutputEnabled()) {
    Serial.print("Distance: ");
    Serial.print(data.distance);
    Serial.println(" cm");
    Serial.print("Motion Detected: ");
    Serial.println(data.motionDetected ? "Yes" : "No");
    Serial.print("Magnetic Field Detected: ");
    Serial.println(data.magneticFieldDetected ? "Yes" : "No");
  }

  delay(500);
}
