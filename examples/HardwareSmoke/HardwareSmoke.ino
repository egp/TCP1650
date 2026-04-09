#include <TCP1650.h>

TCP1650 display(SDA, SCL);

namespace {

uint8_t currentBrightness = 4;
int8_t currentDot = -1;
uint16_t currentValue = 0;

void printHelp() {
  Serial.println();
  Serial.println(F("Commands:"));
  Serial.println(F("  h              help"));
  Serial.println(F("  n <0-9999>     display number"));
  Serial.println(F("  z <0|1>        leading zeros off/on"));
  Serial.println(F("  d <0-3>        set decimal point position"));
  Serial.println(F("  x              clear decimal point"));
  Serial.println(F("  b              read buttons"));
  Serial.println(F("  +              brightness up"));
  Serial.println(F("  -              brightness down"));
  Serial.println(F("  o              display on"));
  Serial.println(F("  f              display off"));
  Serial.println();
}

bool readIntFromSerial(long& value) {
  while (!Serial.available()) {
  }
  value = Serial.parseInt();
  return true;
}

void clearDot() {
  if (currentDot >= 0) {
    (void)display.setDot(static_cast<uint8_t>(currentDot), false);
    currentDot = -1;
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  Serial.println(F("TCP1650 HardwareSmoke"));

  if (!display.begin()) {
    Serial.println(F("begin() failed"));
    return;
  }

  (void)display.setBrightness(currentBrightness);
  (void)display.setNumber(currentValue, true);
  printHelp();
}

void loop() {
  if (!Serial.available()) {
    return;
  }

  const char command = static_cast<char>(Serial.read());

  if (command == 'h' || command == '?') {
    printHelp();
    return;
  }

  if (command == 'n') {
    long value = 0;
    (void)readIntFromSerial(value);
    if (value < 0 || value > 9999) {
      Serial.println(F("number out of range"));
      return;
    }
    currentValue = static_cast<uint16_t>(value);
    if (display.setNumber(currentValue, false)) {
      Serial.print(F("displayed "));
      Serial.println(currentValue);
    } else {
      Serial.println(F("setNumber failed"));
    }
    return;
  }

  if (command == 'z') {
    long enabled = 0;
    (void)readIntFromSerial(enabled);
    if (display.setNumber(currentValue, enabled != 0)) {
      Serial.print(F("leading zeros "));
      Serial.println(enabled != 0 ? F("on") : F("off"));
    } else {
      Serial.println(F("setNumber failed"));
    }
    return;
  }

  if (command == 'd') {
    long position = 0;
    (void)readIntFromSerial(position);
    if (position < 0 || position > 3) {
      Serial.println(F("dot position out of range"));
      return;
    }
    if (currentDot >= 0) {
      (void)display.setDot(static_cast<uint8_t>(currentDot), false);
    }
    currentDot = static_cast<int8_t>(position);
    if (display.setDot(static_cast<uint8_t>(currentDot), true)) {
      Serial.print(F("dot set to "));
      Serial.println(currentDot);
    } else {
      Serial.println(F("setDot failed"));
    }
    return;
  }

  if (command == 'x') {
    clearDot();
    Serial.println(F("dot cleared"));
    return;
  }

  if (command == 'b') {
    const uint8_t buttons = display.getButtons();
    Serial.print(F("buttons raw = 0x"));
    if (buttons < 0x10) {
      Serial.print('0');
    }
    Serial.println(buttons, HEX);
    return;
  }

  if (command == '+') {
    if (currentBrightness < 7) {
      ++currentBrightness;
    }
    if (display.setBrightness(currentBrightness)) {
      Serial.print(F("brightness = "));
      Serial.println(currentBrightness);
    } else {
      Serial.println(F("setBrightness failed"));
    }
    return;
  }

  if (command == '-') {
    if (currentBrightness > 0) {
      --currentBrightness;
    }
    if (display.setBrightness(currentBrightness)) {
      Serial.print(F("brightness = "));
      Serial.println(currentBrightness);
    } else {
      Serial.println(F("setBrightness failed"));
    }
    return;
  }

  if (command == 'o') {
    Serial.println(display.displayOn() ? F("display on") : F("displayOn failed"));
    return;
  }

  if (command == 'f') {
    Serial.println(display.displayOff() ? F("display off") : F("displayOff failed"));
    return;
  }
}