// src/TCP1650.h V1
#ifndef TCP1650_H
#define TCP1650_H

#include <Arduino.h>
#include <BitBang_I2C.h>
#include "TCP1650_Device.h"
#include "TCP1650_I2C.h"

class TCP1650 {
public:
  explicit TCP1650(BBI2C& i2c);

  bool begin();
  bool displayOn();
  bool displayOff();
  bool setBrightness(uint8_t level);
  bool setNumber(uint16_t value, bool leadingZeros = false);
  bool setHex(uint16_t value, bool leadingZeros = false);
  bool setDot(uint8_t position, bool on);
  uint8_t getButtons();

private:
  TCP1650_I2CTransport transport_;
  TCP1650_Device device_;
};

#endif
// src/TCP1650.h V1
