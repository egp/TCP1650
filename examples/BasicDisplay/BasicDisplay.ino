#include <TCP1650.h>

TCP1650 display(SDA, SCL);

void setup() {
  display.begin();
  display.setDigits("1234");
}

void loop() {}
