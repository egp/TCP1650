#include <TCP1650.h>

TCP1650 display(SDA, SCL);

void setup() {
  display.begin();
  display.setDigits("0000");
  display.setDecimalPosition(2);
}

void loop() {
  (void)display.readButtonsRaw();
}
