#include "TCP1650.h"

TCP1650::TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire)
    : transport_(wire, sdaPin, sclPin), device_(transport_) {}

bool TCP1650::begin() {
  if (!transport_.begin()) {
    return false;
  }
  return device_.begin();
}

bool TCP1650::displayOn() {
  return device_.displayOn();
}

bool TCP1650::displayOff() {
  return device_.displayOff();
}

bool TCP1650::setBrightness(uint8_t level) {
  return device_.setBrightness(level);
}

bool TCP1650::setNumber(uint16_t value, bool leadingZeros) {
  return device_.setNumber(value, leadingZeros);
}

bool TCP1650::setHex(uint16_t value, bool leadingZeros) {
  return device_.setHex(value, leadingZeros);
}

bool TCP1650::setDot(uint8_t position, bool on) {
  return device_.setDot(position, on);
}

uint8_t TCP1650::getButtons() {
  return device_.getButtons();
}