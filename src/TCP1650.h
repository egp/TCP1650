#ifndef TCP1650_H
#define TCP1650_H

#include <Arduino.h>
#include <Wire.h>

#include "TCP1650_Device.h"
#include "TCP1650_Wire.h"

class TCP1650 {
public:
  explicit TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire = Wire);

  bool begin();
  bool displayOn();
  bool displayOff();
  bool setBrightness(uint8_t level);
  bool setNumber(uint16_t value, bool leadingZeros = false);
  bool setHex(uint16_t value, bool leadingZeros = false);
  bool setDot(uint8_t position, bool on);
  uint8_t getButtons();

private:
  TCP1650_WireTransport transport_;
  TCP1650_Device device_;
};

#endif