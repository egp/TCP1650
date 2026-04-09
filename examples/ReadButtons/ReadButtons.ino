#include <TCP1650.h>

TCP1650 display(SDA, SCL);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

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