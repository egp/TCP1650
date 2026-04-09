#include <TCP1650.h>

TCP1650 display(SDA, SCL);

void setup() {
  display.begin();
}

void loop() {
  (void)display.readButtonsRaw();
}
