#include "TCP1650_Device.h"

#include "TCP1650_Font.h"
#include "TCP1650_Regs.h"

TCP1650_Device::TCP1650_Device(TCP1650_ByteTransport& transport)
    : transport_(transport),
      displayEnabled_(true),
      brightness_(7),
      segments_{0u, 0u, 0u, 0u} {}

bool TCP1650_Device::begin() {
  brightness_ = 7u;
  displayEnabled_ = true;
  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    segments_[i] = tcp1650EncodeDigit(0u);
  }

  if (!writeControl(true)) {
    return false;
  }
  return refreshDisplay();
}

bool TCP1650_Device::displayOn() {
  displayEnabled_ = true;
  if (!writeControl(true)) {
    return false;
  }
  return refreshDisplay();
}

bool TCP1650_Device::displayOff() {
  displayEnabled_ = false;
  return writeControl(true);
}

bool TCP1650_Device::setBrightness(uint8_t level) {
  if (level > 7u) {
    return false;
  }
  brightness_ = level;
  return writeControl(true);
}

bool TCP1650_Device::setNumber(uint16_t value, bool leadingZeros) {
  if (value > 9999u) {
    return false;
  }

  uint8_t rawDigits[TCP1650_DIGIT_COUNT] = {0u, 0u, 0u, 0u};
  uint8_t encoded[TCP1650_DIGIT_COUNT] = {0u, 0u, 0u, 0u};
  uint16_t working = value;

  for (int8_t position = static_cast<int8_t>(TCP1650_DIGIT_COUNT) - 1; position >= 0;
       --position) {
    rawDigits[position] = static_cast<uint8_t>(working % 10u);
    working = static_cast<uint16_t>(working / 10u);
  }

  bool seenNonZero = false;
  for (uint8_t position = 0; position < TCP1650_DIGIT_COUNT; ++position) {
    if (leadingZeros || seenNonZero || rawDigits[position] != 0u ||
        position == TCP1650_DIGIT_COUNT - 1u) {
      encoded[position] = tcp1650EncodeDigit(rawDigits[position]);
      seenNonZero = true;
    }
  }

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    const uint8_t dot = static_cast<uint8_t>(segments_[i] & TCP1650_DOT_BIT);
    segments_[i] = static_cast<uint8_t>(encoded[i] | dot);
  }

  return refreshDisplay();
}

bool TCP1650_Device::setHex(uint16_t value, bool leadingZeros) {
  uint8_t rawDigits[TCP1650_DIGIT_COUNT] = {0u, 0u, 0u, 0u};
  uint8_t encoded[TCP1650_DIGIT_COUNT] = {0u, 0u, 0u, 0u};
  uint16_t working = value;

  for (int8_t position = static_cast<int8_t>(TCP1650_DIGIT_COUNT) - 1; position >= 0;
       --position) {
    rawDigits[position] = static_cast<uint8_t>(working & 0x0Fu);
    working = static_cast<uint16_t>(working >> 4u);
  }

  bool seenNonZero = false;
  for (uint8_t position = 0; position < TCP1650_DIGIT_COUNT; ++position) {
    if (leadingZeros || seenNonZero || rawDigits[position] != 0u ||
        position == TCP1650_DIGIT_COUNT - 1u) {
      encoded[position] = tcp1650EncodeHexDigit(rawDigits[position]);
      seenNonZero = true;
    }
  }

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    const uint8_t dot = static_cast<uint8_t>(segments_[i] & TCP1650_DOT_BIT);
    segments_[i] = static_cast<uint8_t>(encoded[i] | dot);
  }

  return refreshDisplay();
}

bool TCP1650_Device::setDot(uint8_t position, bool on) {
  if (position >= TCP1650_DIGIT_COUNT) {
    return false;
  }

  if (on) {
    clearAllDots();
    segments_[position] = static_cast<uint8_t>(segments_[position] | TCP1650_DOT_BIT);
  } else {
    segments_[position] = static_cast<uint8_t>(segments_[position] & ~TCP1650_DOT_BIT);
  }

  return refreshDisplay();
}

uint8_t TCP1650_Device::getButtons() {
  uint8_t value = 0u;

  if (!writeControl(false)) {
    (void)writeControl(true);
    (void)refreshDisplay();
    return 0u;
  }

  if (!transport_.readByte(TCP1650_BUTTON_READ_ADDR, value)) {
    (void)writeControl(true);
    (void)refreshDisplay();
    return 0u;
  }

  (void)writeControl(true);
  (void)refreshDisplay();
  return value;
}

uint8_t TCP1650_Device::segmentAt(uint8_t position) const {
  if (position >= TCP1650_DIGIT_COUNT) {
    return 0u;
  }
  return segments_[position];
}

uint8_t TCP1650_Device::controlByteForTest(bool eightSegmentMode) const {
  return tcp1650MakeControlByte(displayEnabled_, eightSegmentMode, brightness_);
}

bool TCP1650_Device::writeControl(bool eightSegmentMode) {
  const uint8_t control = controlByteForTest(eightSegmentMode);

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    const uint8_t address = static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i);
    if (!transport_.writeByte(address, control)) {
      return false;
    }
  }
  return true;
}

bool TCP1650_Device::refreshDisplay() {
  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    const uint8_t address = static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + i);
    if (!transport_.writeByte(address, segments_[i])) {
      return false;
    }
  }
  return true;
}

void TCP1650_Device::clearAllDots() {
  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    segments_[i] = static_cast<uint8_t>(segments_[i] & ~TCP1650_DOT_BIT);
  }
}