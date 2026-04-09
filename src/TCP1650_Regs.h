#ifndef TCP1650_REGS_H
#define TCP1650_REGS_H

#include <stdint.h>

static constexpr uint8_t TCP1650_DIGIT_COUNT = 4;

static constexpr uint8_t TCP1650_CONTROL_BASE_ADDR = 0x24;
static constexpr uint8_t TCP1650_DISPLAY_BASE_ADDR = 0x34;
static constexpr uint8_t TCP1650_BUTTON_READ_ADDR = 0x24;

static constexpr uint8_t TCP1650_DOT_BIT = 0x80;
static constexpr uint8_t TCP1650_MODE_7SEG_BIT = 0x08;
static constexpr uint8_t TCP1650_DISPLAY_ON_BIT = 0x01;
static constexpr uint8_t TCP1650_BRIGHTNESS_SHIFT = 4;

static inline uint8_t tcp1650MakeControlByte(bool displayOn,
                                             bool eightSegmentMode,
                                             uint8_t brightness) {
  const uint8_t clamped = static_cast<uint8_t>(brightness & 0x07u);
  uint8_t value = static_cast<uint8_t>(clamped << TCP1650_BRIGHTNESS_SHIFT);
  if (!eightSegmentMode) {
    value = static_cast<uint8_t>(value | TCP1650_MODE_7SEG_BIT);
  }
  if (displayOn) {
    value = static_cast<uint8_t>(value | TCP1650_DISPLAY_ON_BIT);
  }
  return value;
}

#endif