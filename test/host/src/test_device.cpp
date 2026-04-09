#include "TCP1650_Device.h"
#include "TCP1650_Font.h"
#include "TCP1650_Regs.h"
#include "test_assert.h"

#include <cstddef>
#include <cstdint>
#include <vector>

struct BusOp {
  enum class Kind { Write, Read } kind;
  uint8_t address;
  uint8_t value;
};

class FakeTransport : public TCP1650_ByteTransport {
public:
  bool writeByte(uint8_t address, uint8_t value) override {
    ops.push_back(BusOp{BusOp::Kind::Write, address, value});
    return true;
  }

  bool readByte(uint8_t address, uint8_t& value) override {
    value = nextReadValue;
    ops.push_back(BusOp{BusOp::Kind::Read, address, value});
    return true;
  }

  void clear() {
    ops.clear();
  }

  std::vector<BusOp> ops;
  uint8_t nextReadValue = 0u;
};

static void assertWrite(const BusOp& op, uint8_t address, uint8_t value) {
  TEST_ASSERT_EQ(static_cast<int>(BusOp::Kind::Write), static_cast<int>(op.kind));
  TEST_ASSERT_EQ(static_cast<int>(address), static_cast<int>(op.address));
  TEST_ASSERT_EQ(static_cast<int>(value), static_cast<int>(op.value));
}

static void assertRead(const BusOp& op, uint8_t address, uint8_t value) {
  TEST_ASSERT_EQ(static_cast<int>(BusOp::Kind::Read), static_cast<int>(op.kind));
  TEST_ASSERT_EQ(static_cast<int>(address), static_cast<int>(op.address));
  TEST_ASSERT_EQ(static_cast<int>(value), static_cast<int>(op.value));
}

static void testEncodeDigit() {
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x3F), tcp1650EncodeDigit(0u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x06), tcp1650EncodeDigit(1u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x5B), tcp1650EncodeDigit(2u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x4F), tcp1650EncodeDigit(3u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x66), tcp1650EncodeDigit(4u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x6D), tcp1650EncodeDigit(5u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x7D), tcp1650EncodeDigit(6u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x07), tcp1650EncodeDigit(7u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x7F), tcp1650EncodeDigit(8u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x6F), tcp1650EncodeDigit(9u));
}

static void testBeginWritesControlThen0000() {
  FakeTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_EQ(static_cast<std::size_t>(8u), transport.ops.size());

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i], static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i), 0x71u);
  }
  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[TCP1650_DIGIT_COUNT + i],
                static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + i),
                tcp1650EncodeDigit(0u));
  }
}

static void testSetNumberFormatsWithoutLeadingZeros() {
  FakeTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(device.setNumber(42u, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  assertWrite(transport.ops[0], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u), 0x00u);
  assertWrite(transport.ops[1], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u), 0x00u);
  assertWrite(transport.ops[2], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u), tcp1650EncodeDigit(4u));
  assertWrite(transport.ops[3], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u), tcp1650EncodeDigit(2u));
}

static void testSetNumberFormatsWithLeadingZeros() {
  FakeTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(device.setNumber(42u, true));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x3F), device.segmentAt(0u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x3F), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(4u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(2u), device.segmentAt(3u));
}

static void testSetDotUsesHighBitAndEnforcesOneDot() {
  FakeTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setNumber(1234u, false));
  transport.clear();

  TEST_ASSERT_TRUE(device.setDot(2u, true));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());
  TEST_ASSERT_EQ(static_cast<uint8_t>(tcp1650EncodeDigit(3u) | TCP1650_DOT_BIT), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(4u), device.segmentAt(3u));

  TEST_ASSERT_TRUE(device.setDot(3u, true));
  TEST_ASSERT_EQ(static_cast<uint8_t>(tcp1650EncodeDigit(3u)), device.segmentAt(2u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(tcp1650EncodeDigit(4u) | TCP1650_DOT_BIT), device.segmentAt(3u));
}

static void testGetButtonsSwitchesModesAndRestoresDisplay() {
  FakeTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setNumber(1234u, false));
  TEST_ASSERT_TRUE(device.setDot(1u, true));
  transport.clear();
  transport.nextReadValue = 0x05u;

  const uint8_t buttons = device.getButtons();
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x05u), buttons);
  TEST_ASSERT_EQ(static_cast<std::size_t>(13u), transport.ops.size());

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i], static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i), 0x79u);
  }

  assertRead(transport.ops[4], TCP1650_BUTTON_READ_ADDR, 0x05u);

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[5 + i], static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i), 0x71u);
  }

  assertWrite(transport.ops[9], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u), tcp1650EncodeDigit(1u));
  assertWrite(transport.ops[10], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u), static_cast<uint8_t>(tcp1650EncodeDigit(2u) | TCP1650_DOT_BIT));
  assertWrite(transport.ops[11], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u), tcp1650EncodeDigit(3u));
  assertWrite(transport.ops[12], static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u), tcp1650EncodeDigit(4u));
}

static void testRejectsOutOfRangeValues() {
  FakeTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(!device.setBrightness(8u));
  TEST_ASSERT_TRUE(!device.setNumber(10000u, false));
  TEST_ASSERT_TRUE(!device.setDot(4u, true));
  TEST_ASSERT_EQ(static_cast<std::size_t>(0u), transport.ops.size());
}

int main() {
  testEncodeDigit();
  testBeginWritesControlThen0000();
  testSetNumberFormatsWithoutLeadingZeros();
  testSetNumberFormatsWithLeadingZeros();
  testSetDotUsesHighBitAndEnforcesOneDot();
  testGetButtonsSwitchesModesAndRestoresDisplay();
  testRejectsOutOfRangeValues();
  return 0;
}