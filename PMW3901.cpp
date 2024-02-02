#include "PMW3901.h"

PMW3901::PMW3901(uint8_t csPin) : _csPin(csPin) {}

void PMW3901::begin() {
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    SPI.begin();

    // Initialize sensor (This is where you will include the initialization sequence from the sensor's datasheet or example code)
}

void PMW3901::writeRegister(uint8_t reg, uint8_t value) {
    digitalWrite(_csPin, LOW);
    SPI.transfer(reg | 0x80); // Write operation
    SPI.transfer(value);
    digitalWrite(_csPin, HIGH);
}

uint8_t PMW3901::readRegister(uint8_t reg) {
    digitalWrite(_csPin, LOW);
    SPI.transfer(reg & 0x7F); // Read operation
    uint8_t value = SPI.transfer(0);
    digitalWrite(_csPin, HIGH);
    return value;
}

bool PMW3901::readMotion(int16_t &deltaX, int16_t &deltaY) {
    // Read motion registers here
    // Return true if successful, false otherwise
}

