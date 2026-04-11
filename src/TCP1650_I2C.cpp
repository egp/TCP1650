// src/TCP1650_I2C.cpp V1
#include "TCP1650_I2C.h"

TCP1650_I2CTransport::TCP1650_I2CTransport(BBI2C& i2c)
  : i2c_(i2c) {}

bool TCP1650_I2CTransport::begin() {
  // Caller owns BBI2C setup and I2CInit().
  return true;
}

bool TCP1650_I2CTransport::writeByte(uint8_t address, uint8_t value) {
  uint8_t data[1] = { value };
  return I2CWrite(&i2c_, address, data, 1) == 1;
}

bool TCP1650_I2CTransport::readByte(uint8_t address, uint8_t& value) {
  uint8_t data[1] = { 0 };
  if (I2CRead(&i2c_, address, data, 1) != 1) {
    return false;
  }
  value = data[0];
  return true;
}
// src/TCP1650_I2C.cpp V1