#include <TCP1650.h>

TCP1650 display(SDA, SCL);

void setup() {
  (void)display.begin();
  (void)display.setBrightness(4);
  (void)display.setNumber(1234, false);
}

void loop() {
}