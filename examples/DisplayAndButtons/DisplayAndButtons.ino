#include <string.h>

#include <TCP1819.h>
#include <TCP1650.h>

namespace {
static const uint8_t kDisplaySdaPin = SDA;
static const uint8_t kDisplaySclPin = SCL;
static const uint32_t kDisplayI2cHz = 100000UL;

BBI2C displayBus{};
TCP1650 display(displayBus);

uint16_t counter = 0;
int8_t dot = 0;

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
  (void)display.setBrightness(4);
  (void)display.setNumber(counter, true);
  (void)display.setDot(static_cast<uint8_t>(dot), true);
}

void loop() {
  const uint8_t buttons = display.getButtons();

  (void)display.setNumber(counter, true);
  (void)display.setDot(static_cast<uint8_t>(dot), true);

  Serial.print(F("counter="));
  Serial.print(counter);
  Serial.print(F(" buttons=0x"));
  if (buttons < 0x10) {
    Serial.print('0');
  }
  Serial.println(buttons, HEX);

  ++counter;
  dot = static_cast<int8_t>((dot + 1) % 4);
  delay(1000);
}