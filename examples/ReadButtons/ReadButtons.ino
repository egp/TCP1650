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
  Serial.begin(115200);
  while (!Serial) {
  }

  setupDisplayBus();

  (void)display.begin();
  (void)display.setNumber(0, true);
}

void loop() {
  const uint8_t buttons = display.getButtons();

  Serial.print(F("buttons raw = 0x"));
  if (buttons < 0x10) {
    Serial.print('0');
  }
  Serial.println(buttons, HEX);

  delay(500);
}