#ifndef TCP1650_DEVICE_H
#define TCP1650_DEVICE_H

#include <stdint.h>

class TCP1650_ByteTransport {
public:
  virtual ~TCP1650_ByteTransport() = default;

  virtual bool writeByte(uint8_t address, uint8_t value) = 0;
  virtual bool readByte(uint8_t address, uint8_t& value) = 0;
};

class TCP1650_Device {
public:
  explicit TCP1650_Device(TCP1650_ByteTransport& transport);

  bool begin();
  bool displayOn();
  bool displayOff();
  bool setBrightness(uint8_t level);
  bool setNumber(uint16_t value, bool leadingZeros = false);
  bool setDot(uint8_t position, bool on);
  uint8_t getButtons();

  uint8_t segmentAt(uint8_t position) const;
  uint8_t controlByteForTest(bool eightSegmentMode) const;

private:
  bool writeControl(bool eightSegmentMode);
  bool refreshDisplay();
  void clearAllDots();

  TCP1650_ByteTransport& transport_;
  bool displayEnabled_;
  uint8_t brightness_;
  uint8_t segments_[4];
};

#endif