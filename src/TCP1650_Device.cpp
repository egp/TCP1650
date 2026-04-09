#include "TCP1650_Device.h"

TCP1650_Device::TCP1650_Device(TwoWire& wire) : wire_(&wire) {}

bool TCP1650_Device::begin(uint8_t sdaPin, uint8_t sclPin) {
  (void)sdaPin;
  (void)sclPin;
  return false;
}

bool TCP1650_Device::setDisplayMode(bool eightSegmentMode) {
  (void)eightSegmentMode;
  return false;
}

bool TCP1650_Device::setBrightness(uint8_t level) {
  (void)level;
  return false;
}

bool TCP1650_Device::writeDigit(uint8_t position, uint8_t segments) {
  (void)position;
  (void)segments;
  return false;
}

uint8_t TCP1650_Device::readButtonsRaw() {
  return 0;
}
