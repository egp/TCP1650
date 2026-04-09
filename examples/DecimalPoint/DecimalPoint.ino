#include <TCP1650.h>

TCP1650 display(SDA, SCL);

void setup() {
  display.begin();
  display.setDigits("3141");
  display.setDecimalPosition(0);
}

void loop() {}
