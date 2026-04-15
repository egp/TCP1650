// src/TCP1650_I2C.h V1
#ifndef TCP1650_I2C_H
#define TCP1650_I2C_H

#include <Arduino.h>
#include <TCP1819.h>
#include "TCP1650_Device.h"

class TCP1650_I2CTransport : public TCP1650_ByteTransport {
public:
  explicit TCP1650_I2CTransport(BBI2C& i2c);

  bool begin();
  bool writeByte(uint8_t address, uint8_t value) override;
  bool readByte(uint8_t address, uint8_t& value) override;

private:
  BBI2C& i2c_;
};

#endif
// src/TCP1650_I2C.h V1