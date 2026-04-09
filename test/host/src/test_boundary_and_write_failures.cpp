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

static void testSetNumberZeroWithoutLeadingZeros() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(device.setNumber(0u, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              0x00u);
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              0x00u);
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              0x00u);
  assertWrite(transport.ops[3],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              tcp1650EncodeDigit(0u));
}

static void testSetHexZeroWithoutLeadingZeros() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(device.setHex(0x0000u, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              0x00u);
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              0x00u);
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              0x00u);
  assertWrite(transport.ops[3],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              tcp1650EncodeHexDigit(0u));
}

static void testSetNumberUpperBoundary() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(device.setNumber(9999u, false));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(9u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(9u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(9u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(9u), device.segmentAt(3u));
}

static void testSetHexUpperBoundary() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();

  TEST_ASSERT_TRUE(device.setHex(0xFFFFu, true));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(15u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(15u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(15u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(15u), device.segmentAt(3u));
}

static void testSetNumberWriteFailureReturnsFalseAndUpdatesCache() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();
  transport.failWriteOnCall(2);

  TEST_ASSERT_TRUE(!device.setNumber(9876u, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(3u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              tcp1650EncodeDigit(9u));
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              tcp1650EncodeDigit(8u));
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              tcp1650EncodeDigit(7u),
              false);

  TEST_ASSERT_EQ(tcp1650EncodeDigit(9u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(8u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(7u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeDigit(6u), device.segmentAt(3u));
}

static void testSetHexWriteFailureReturnsFalseAndUpdatesCache() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  transport.clear();
  transport.failWriteOnCall(1);

  TEST_ASSERT_TRUE(!device.setHex(0xABCDu, false));
  TEST_ASSERT_EQ(static_cast<std::size_t>(2u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              tcp1650EncodeHexDigit(10u));
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              tcp1650EncodeHexDigit(11u),
              false);

  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(10u), device.segmentAt(0u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(11u), device.segmentAt(1u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(12u), device.segmentAt(2u));
  TEST_ASSERT_EQ(tcp1650EncodeHexDigit(13u), device.segmentAt(3u));
}

static void testDisplayOffThenSetHexKeepsOffState() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.displayOff());
  transport.clear();

  TEST_ASSERT_TRUE(device.setHex(0x00AFu, false));
  TEST_ASSERT_EQ(static_cast<uint8_t>(0x00u), device.controlByteForTest(true));
  TEST_ASSERT_EQ(static_cast<std::size_t>(4u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              0x00u);
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              0x00u);
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              tcp1650EncodeHexDigit(10u));
  assertWrite(transport.ops[3],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              tcp1650EncodeHexDigit(15u));
}

static void testGetButtonsInitialControlWriteFailureRestoresDisplayAndReturnsZero() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setHex(0x1234u, false));
  TEST_ASSERT_TRUE(device.setDot(2u, true));
  transport.clear();
  transport.failWriteOnCall(2);

  const uint8_t buttons = device.getButtons();
  TEST_ASSERT_EQ(static_cast<uint8_t>(0u), buttons);
  TEST_ASSERT_EQ(static_cast<std::size_t>(11u), transport.ops.size());

  assertWrite(transport.ops[0],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 0u),
              0x09u);
  assertWrite(transport.ops[1],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 1u),
              0x09u);
  assertWrite(transport.ops[2],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 2u),
              0x09u,
              false);

  assertWrite(transport.ops[3],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 0u),
              0x01u);
  assertWrite(transport.ops[4],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 1u),
              0x01u);
  assertWrite(transport.ops[5],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 2u),
              0x01u);
  assertWrite(transport.ops[6],
              static_cast<uint8_t>(TCP1650_CONTROL_BASE_ADDR + 3u),
              0x01u);

  assertWrite(transport.ops[7],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 0u),
              tcp1650EncodeHexDigit(1u));
  assertWrite(transport.ops[8],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              tcp1650EncodeHexDigit(2u));
  assertWrite(transport.ops[9],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              static_cast<uint8_t>(tcp1650EncodeHexDigit(3u) | TCP1650_DOT_BIT));
  assertWrite(transport.ops[10],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              tcp1650EncodeHexDigit(4u));
}

static void testGetButtonsReadFailureRestoresCachedDotState() {
  FaultyTransport transport;
  TCP1650_Device device(transport);

  TEST_ASSERT_TRUE(device.begin());
  TEST_ASSERT_TRUE(device.setNumber(2468u, false));
  TEST_ASSERT_TRUE(device.setDot(3u, true));
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
              tcp1650EncodeDigit(2u));
  assertWrite(transport.ops[10],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 1u),
              tcp1650EncodeDigit(4u));
  assertWrite(transport.ops[11],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 2u),
              tcp1650EncodeDigit(6u));
  assertWrite(transport.ops[12],
              static_cast<uint8_t>(TCP1650_DISPLAY_BASE_ADDR + 3u),
              static_cast<uint8_t>(tcp1650EncodeDigit(8u) | TCP1650_DOT_BIT));
}

}  // namespace

void runBoundaryAndWriteFailureTests() {
  testSetNumberZeroWithoutLeadingZeros();
  testSetHexZeroWithoutLeadingZeros();
  testSetNumberUpperBoundary();
  testSetHexUpperBoundary();
  testSetNumberWriteFailureReturnsFalseAndUpdatesCache();
  testSetHexWriteFailureReturnsFalseAndUpdatesCache();
  testDisplayOffThenSetHexKeepsOffState();
  testGetButtonsInitialControlWriteFailureRestoresDisplayAndReturnsZero();
  testGetButtonsReadFailureRestoresCachedDotState();
}