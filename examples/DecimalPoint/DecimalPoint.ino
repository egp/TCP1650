#include <string.h>

#include <TCP1819.h>
#include <TCP1650.h>

namespace {
static const uint8_t kDisplaySdaPin = SDA;
static const uint8_t kDisplaySclPin = SCL;
static const uint32_t kDisplayI2cHz = 100000UL;

BBI2C displayBus{};
TCP1650 display(displayBus);

void setupDisplayBus() {
  memset(&displayBus, 0, sizeof(displayBus));
  displayBus.bWire = 0;
  displayBus.iSDA = kDisplaySdaPin;
  displayBus.iSCL = kDisplaySclPin;
  I2CInit(&displayBus, kDisplayI2cHz);
}
}  // namespace

void setup() {
  setupDisplayBus();
  (void)display.begin();
  (void)display.setNumber(3141, false);
  (void)display.setDot(1, true);
}

void loop() {
}