#ifndef TCP1650_DEVICE_H
#define TCP1650_DEVICE_H

#include <Arduino.h>
#include <Wire.h>

class TCP1650_Device {
public:
  explicit TCP1650_Device(TwoWire& wire = Wire);

  bool begin(uint8_t sdaPin, uint8_t sclPin);
  bool setDisplayMode(bool eightSegmentMode);
  bool setBrightness(uint8_t level);
  bool writeDigit(uint8_t position, uint8_t segments);
  uint8_t readButtonsRaw();

private:
  TwoWire* wire_;
};

#endif
