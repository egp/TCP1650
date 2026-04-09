#include "TCP1650.h"

TCP1650::TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire)
    : wire_(&wire),
      sdaPin_(sdaPin),
      sclPin_(sclPin),
      brightness_(0),
      decimalPosition_(-1),
      digits_{0, 0, 0, 0} {}

bool TCP1650::begin() {
  return false;
}

bool TCP1650::clear() {
  for (uint8_t i = 0; i < 4; ++i) {
    digits_[i] = 0;
  }
  decimalPosition_ = -1;
  return false;
}

bool TCP1650::setBrightness(uint8_t level) {
  brightness_ = level;
  return false;
}

bool TCP1650::setDigits(const char digits[4]) {
  (void)digits;
  return false;
}

bool TCP1650::setDigits(const uint8_t segments[4]) {
  for (uint8_t i = 0; i < 4; ++i) {
    digits_[i] = segments[i];
  }
  return false;
}

bool TCP1650::setDecimalPosition(int8_t position) {
  decimalPosition_ = position;
  return false;
}

uint8_t TCP1650::readButtonsRaw() {
  return 0;
}

bool TCP1650::refresh() {
  return false;
}
