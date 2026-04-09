#include "TCP1650_Device.h"
#include "TCP1650_Font.h"
#include "TCP1650_Regs.h"
#include "test_assert.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

struct BusOp {
  enum class Kind { Write, Read } kind;
  uint8_t address;
  uint8_t value;
  bool success;
};

class RecordingTransport : public TCP1650_ByteTransport {
public:
  bool writeByte(uint8_t address, uint8_t value) override {
    ops.push_back(BusOp{BusOp::Kind::Write, address, value, true});
    return true;
  }

  bool readByte(uint8_t address, uint8_t& value) override {
    value = 0u;
    ops.push_back(BusOp{BusOp::Kind::Read, address, value, true});
    return true;
  }

  void clear() {
    ops.clear();
  }

  std::vector<BusOp> ops;
};

static void assertWrite(const BusOp& op, uint8_t address, uint8_t value) {
  TEST_ASSERT_EQ(static_cast<int>(BusOp::Kind::Write),
                 static_cast<int>(op.kind));
  TEST_ASSERT_EQ(static_cast<int>(address), static_cast<int>(op.address));
  TEST_ASSERT_EQ(static_cast<int>(value), static_cast<int>(op.value));
  TEST_ASSERT_EQ(1, static_cast<int>(op.success));
}

static void testDisplayOffThenSetNumberThenDisplayOnShowsCachedValue() {
  RecordingTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.displayOff());
  transport.clear();

  TEST_ASSERT_TRUE(device.setNumber(42u, true));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              tcp1650EncodeDigit(0u));
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              tcp1650EncodeDigit(0u));
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              tcp1650EncodeDigit(4u));
  assertWrite(transport.ops[3],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              tcp1650EncodeDigit(2u));

  TEST_ASSERT_EQ(static_cast<uint8_t>(0x00u), device.controlByteForTest(true));

  transport.clear();
  TEST_ASSERT_TRUE(device.displayOn());
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x01u);
  }

  TEST_ASSERT_EQ(tcp1650EncodeDigit(0u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(0u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(4u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(2u), device.segmentAt(3u));
}

static void testDisplayOffThenSetHexThenDisplayOnShowsCachedValue() {
  RecordingTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.displayOff());
  transport.clear();

  TEST_ASSERT_TRUE(device.setHex(0x03AFu, true));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              tcp1650EncodeHexDigit(0u));
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              tcp1650EncodeHexDigit(3u));
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              tcp1650EncodeHexDigit(10u));
  assertWrite(transport.ops[3],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              tcp1650EncodeHexDigit(15u));

  TEST_ASSERT_EQ(static_cast<uint8_t>(0x00u), device.controlByteForTest(true));

  transport.clear();
  TEST_ASSERT_TRUE(device.displayOn());
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x01u);
  }

  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(0u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(3u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(10u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(15u), device.segmentAt(3u));
}

static void testDisplayOffDoesNotClearDotOrCachedDigits() {
  RecordingTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setNumber(1234u, false));
  TEST_ASSERT_TRUE(device.setDot(1u, true));
  TEST_ASSERT_TRUE(device.displayOff());
  transport.clear();

  TEST_ASSERT_TRUE(device.displayOn());
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x01u);
  }

  TEST_ASSERT_EQ(tcp1650EncodeDigit(1u), device.segmentAt(0u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(tcp1650EncodeDigit(2u) | TCP1650_DOT_BIT),
                 device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(3u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(4u), device.segmentAt(3u));
}

static void testMultipleOffStateUpdatesKeepMostRecentCachedValue() {
  RecordingTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.displayOff());
  TEST_ASSERT_TRUE(device.setNumber(1111u, true));
  TEST_ASSERT_TRUE(device.setHex(0xBEEFu, true));
  transport.clear();

  TEST_ASSERT_TRUE(device.displayOn());
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x01u);
  }

  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(11u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(14u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(14u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(15u), device.segmentAt(3u));
}

}  // namespace

void runDisplayOffCacheTests() {
  testDisplayOffThenSetNumberThenDisplayOnShowsCachedValue();
  testDisplayOffThenSetHexThenDisplayOnShowsCachedValue();
  testDisplayOffDoesNotClearDotOrCachedDigits();
  testMultipleOffStateUpdatesKeepMostRecentCachedValue();
}