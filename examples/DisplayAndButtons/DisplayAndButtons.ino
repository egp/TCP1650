#include <TCP1650.h>

TCP1650 display(SDA, SCL);

uint16_t counter = 0;
int8_t dot = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

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