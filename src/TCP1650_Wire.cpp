#include "TCP1650_Wire.h"

TCP1650_WireTransport::TCP1650_WireTransport(TwoWire& wire, uint8_t sdaPin, uint8_t sclPin)
    : wire_(wire), sdaPin_(sdaPin), sclPin_(sclPin) {}

bool TCP1650_WireTransport::begin() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_RP2040)
  wire_.begin(static_cast<int>(sdaPin_), static_cast<int>(sclPin_));
#else
  (void)sdaPin_;
  (void)sclPin_;
  wire_.begin();
#endif
  return true;
}

bool TCP1650_WireTransport::writeByte(uint8_t address, uint8_t value) {
  wire_.beginTransmission(address);
  wire_.write(value);
  return wire_.endTransmission() == 0;
}

bool TCP1650_WireTransport::readByte(uint8_t address, uint8_t& value) {
  const int count = wire_.requestFrom(static_cast<int>(address), 1);
  if (count != 1) {
    return false;
  }
  value = static_cast<uint8_t>(wire_.read());
  return true;
}