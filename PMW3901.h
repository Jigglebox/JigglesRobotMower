#ifndef PMW3901_H
#define PMW3901_H

#include <Arduino.h>
#include <SPI.h>

class PMW3901 {
public:
    PMW3901(uint8_t csPin);
    void begin();
    bool readMotion(int16_t &deltaX, int16_t &deltaY);

private:
    uint8_t _csPin;
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
};

#endif
