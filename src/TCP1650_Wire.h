#ifndef TCP1650_WIRE_H
#define TCP1650_WIRE_H

#include <Arduino.h>
#include <Wire.h>

#include "TCP1650_Device.h"

class TCP1650_WireTransport : public TCP1650_ByteTransport {
public:
  TCP1650_WireTransport(TwoWire& wire, uint8_t sdaPin, uint8_t sclPin);

  bool begin();
  bool writeByte(uint8_t address, uint8_t value) override;
  bool readByte(uint8_t address, uint8_t& value) override;

private:
  TwoWire& wire_;
  uint8_t sdaPin_;
  uint8_t sclPin_;
};

#endif