#include "TCP1650_Font.h"

#include <stdint.h>

uint8_t tcp1650EncodeDigit(uint8_t digit) {
  static constexpr uint8_t kDigitSegments[10] = {
      0x3F,  // 0
      0x06,  // 1
      0x5B,  // 2
      0x4F,  // 3
      0x66,  // 4
      0x6D,  // 5
      0x7D,  // 6
      0x07,  // 7
      0x7F,  // 8
      0x6F,  // 9
  };

  if (digit > 9u) {
    return 0u;
  }
  return kDigitSegments[digit];
}

uint8_t tcp1650EncodeHexDigit(uint8_t digit) {
  static constexpr uint8_t kHexSegments[16] = {
      0x3F,  // 0
      0x06,  // 1
      0x5B,  // 2
      0x4F,  // 3
      0x66,  // 4
      0x6D,  // 5
      0x7D,  // 6
      0x07,  // 7
      0x7F,  // 8
      0x6F,  // 9
      0x77,  // A
      0x7C,  // b
      0x39,  // C
      0x5E,  // d
      0x79,  // E
      0x71,  // F
  };

  if (digit > 15u) {
    return 0u;
  }
  return kHexSegments[digit];
}