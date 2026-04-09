#ifndef TCP1650_H
#define TCP1650_H

#include <Arduino.h>
#include <Wire.h>

class TCP1650 {
public:
  TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire = Wire);

  bool begin();
  bool clear();
  bool setBrightness(uint8_t level);
  bool setDigits(const char digits[4]);
  bool setDigits(const uint8_t segments[4]);
  bool setDecimalPosition(int8_t position);
  uint8_t readButtonsRaw();
  bool refresh();

private:
  TwoWire* wire_;
  uint8_t sdaPin_;
  uint8_t sclPin_;
  uint8_t brightness_;
  int8_t decimalPosition_;
  uint8_t digits_[4];
};

#endif
