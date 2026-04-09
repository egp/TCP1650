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

class FaultyTransport : public TCP1650_ByteTransport {
public:
  bool writeByte(uint8_t address, uint8_t value) override {
    const bool success = (writeCount_ != failWriteCall_);
    ops.push_back(BusOp{BusOp::Kind::Write, address, value, success});
    ++writeCount_;
    return success;
  }

  bool readByte(uint8_t address, uint8_t& value) override {
    const bool success = (readCount_ != failReadCall_);
    value = success ? nextReadValue : 0u;
    ops.push_back(BusOp{BusOp::Kind::Read, address, value, success});
    ++readCount_;
    return success;
  }

  void clear() {
    ops.clear();
    writeCount_ = 0;
    readCount_ = 0;
    failWriteCall_ = -1;
    failReadCall_ = -1;
    nextReadValue = 0u;
  }

  void failWriteOnCall(int zeroBasedCall) {
    failWriteCall_ = zeroBasedCall;
  }

  void failReadOnCall(int zeroBasedCall) {
    failReadCall_ = zeroBasedCall;
  }

  std::vector<BusOp> ops;
  uint8_t nextReadValue = 0u;

private:
  int writeCount_ = 0;
  int readCount_ = 0;
  int failWriteCall_ = -1;
  int failReadCall_ = -1;
};

static void assertWrite(const BusOp& op,
                        uint8_t address,
                        uint8_t value,
                        bool success = true) {
  TEST_ASSERT_EQ(static_cast<int>(BusOp::Kind::Write),
                 static_cast<int>(op.kind));
  TEST_ASSERT_EQ(static_cast<int>(address), static_cast<int>(op.address));
  TEST_ASSERT_EQ(static_cast<int>(value), static_cast<int>(op.value));
  TEST_ASSERT_EQ(static_cast<int>(success), static_cast<int>(op.success));
}

static void assertRead(const BusOp& op,
                       uint8_t address,
                       uint8_t value,
                       bool success = true) {
  TEST_ASSERT_EQ(static_cast<int>(BusOp::Kind::Read),
                 static_cast<int>(op.kind));
  TEST_ASSERT_EQ(static_cast<int>(address), static_cast<int>(op.address));
  TEST_ASSERT_EQ(static_cast<int>(value), static_cast<int>(op.value));
  TEST_ASSERT_EQ(static_cast<int>(success), static_cast<int>(op.success));
}

static void testDisplayOffThenOnWritesOnlyControlBytes() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(device.displayOff());
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());
  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x00u);
  }

  transport.clear();

  TEST_ASSERT_TRUE(device.displayOn());
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());
  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x01u);
  }
}

static void testBrightnessMappingAllLevels() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  static constexpr uint8_t kExpected[8] = {
      0x11u, 0x21u, 0x31u, 0x41u, 0x51u, 0x61u, 0x71u, 0x01u,
  };

  TEST_ASSERT_TRUE(device.begin());

  for (uint8_t level = 0; level < 8u; ++level) {
    transport.clear();
    TEST_ASSERT_TRUE(device.setBrightness(level));
    TEST_ASSERT_EQ(static_cast<uint8_t>(kExpected[level]),
                   device.controlByteForTest(true));
    TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());
    for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
      assertWrite(transport.ops[i],
                  static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                  kExpected[level]);
    }
  }
}

static void testSetNumberPreservesExistingDot() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setNumber(1234u, false));
  TEST_ASSERT_TRUE(device.setDot(2u, true));
  transport.clear();

  TEST_ASSERT_TRUE(device.setNumber(5678u, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());
  TEST_ASSERT_EQ(tcp1650EncodeDigit(5u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(6u), device.segmentAt(1u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(tcp1650EncodeDigit(7u) | TCP1650_DOT_BIT),
                 device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(8u), device.segmentAt(3u));
}

static void testSetHexPreservesExistingDot() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setHex(0x1A2Fu, false));
  TEST_ASSERT_TRUE(device.setDot(1u, true));
  transport.clear();

  TEST_ASSERT_TRUE(device.setHex(0xBCDEu, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(11u), device.segmentAt(0u));
  TEST_ASSERT_EQ(static_cast<uint8_t>(tcp1650EncodeHexDigit(12u) | TCP1650_DOT_BIT),
                 device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(13u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(14u), device.segmentAt(3u));
}

static void testSetDotFalseClearsSelectedDot() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setHex(0x1234u, false));
  TEST_ASSERT_TRUE(device.setDot(2u, true));
  transport.clear();

  TEST_ASSERT_TRUE(device.setDot(2u, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(1u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(2u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(3u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(4u), device.segmentAt(3u));
}

static void testGetButtonsReadFailureRestoresDisplayAndReturnsZero() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setHex(0x12AFu, false));
  TEST_ASSERT_TRUE(device.setDot(1u, true));
  transport.clear();
  transport.failReadOnCall(0);

  const uint8_t buttons = device.getButtons();
  TEST_ASSERT_EQ(static_cast<uint8_t>(0u), buttons);
  TEST_ASSERT_EQ(static_cast<std::size_t>(13u), transport.ops.size());

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x09u);
  }

  assertRead(transport.ops[4], TCP1650_BUTTON_READ_ADDR, 0x00u, false);

  for (uint8_t i = 0; i < TCP1650_DIGIT_COUNT; ++i) {
    assertWrite(transport.ops[5 + i],
                static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + i),
                0x01u);
  }

  assertWrite(transport.ops[9],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              tcp1650EncodeHexDigit(1u));
  assertWrite(transport.ops[10],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              static_cast<uint8_t>(tcp1650EncodeHexDigit(2u) | TCP1650_DOT_BIT));
  assertWrite(transport.ops[11],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              tcp1650EncodeHexDigit(10u));
  assertWrite(transport.ops[12],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              tcp1650EncodeHexDigit(15u));
}

static void testDisplayOnFailureReturnsFalse() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.displayOff());
  transport.clear();
  transport.failWriteOnCall(2);

  TEST_ASSERT_TRUE(!device.displayOn());
  TEST_ASSERT_EQ(static_cast<std::size_t>(3u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 0u),
              0x01u);
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 1u),
              0x01u);
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 2u),
              0x01u,
              false);
}

}  // namespace

void runStateAndFailureTests() {
  testDisplayOffThenOnWritesOnlyControlBytes();
  testBrightnessMappingAllLevels();
  testSetNumberPreservesExistingDot();
  testSetHexPreservesExistingDot();
  testSetDotFalseClearsSelectedDot();
  testGetButtonsReadFailureRestoresDisplayAndReturnsZero();
  testDisplayOnFailureReturnsFalse();
}