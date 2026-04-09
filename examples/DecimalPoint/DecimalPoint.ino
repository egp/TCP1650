#include <TCP1650.h>

TCP1650 display(SDA, SCL);

void setup() {
  (void)display.begin();
  (void)display.setNumber(3141, false);
  (void)display.setDot(1, true);
}

void loop() {
}