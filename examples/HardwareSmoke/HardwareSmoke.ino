#include <TCP1650.h>

TCP1650 display(SDA, SCL);

namespace {

enum class DisplayMode {
  Number,
  Hex,
};

static constexpr unsigned long kPollIntervalMs = 1000UL;

uint8_t currentBrightness = 4;
int8_t configuredDot = -1;
uint16_t currentNumberValue = 0;
uint16_t currentHexValue = 0;
bool currentLeadingZeros = true;
DisplayMode currentDisplayMode = DisplayMode::Number;

bool pollMode = false;
bool haveLastButtons = false;
uint8_t lastButtons = 0;
unsigned long lastPollAtMs = 0;

void printHelp() {
  Serial.println();
  Serial.println(F("Commands:"));
  Serial.println(F("  h              help"));
  Serial.println(F("  n <0-9999>     display number"));
  Serial.println(F("  z <0|1>        leading zeros off/on"));
  Serial.println(F("  d <0-3>        set decimal point position"));
  Serial.println(F("  x              clear decimal point"));
  Serial.println(F("  b              start/continue 1 Hz button poll mode"));
  Serial.println(F("                 any non-b command exits poll mode"));
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

bool applyCurrentDisplay() {
  if (currentDisplayMode == DisplayMode::Hex) {
    if (configuredDot >= 0) {
      (void)display.setDot(static_cast<uint8_t>(configuredDot), false);
    }
    return display.setHex(currentHexValue, currentLeadingZeros);
  }

  if (!display.setNumber(currentNumberValue, currentLeadingZeros)) {
    return false;
  }
  if (configuredDot >= 0) {
    return display.setDot(static_cast<uint8_t>(configuredDot), true);
  }
  return true;
}

void printButtonsRaw(uint8_t buttons) {
  Serial.print(F("buttons raw = 0x"));
  if (buttons < 0x10) {
    Serial.print('0');
  }
  Serial.println(buttons, HEX);
}

void pollButtons(bool force) {
  const unsigned long now = millis();
  if (!force && (now - lastPollAtMs) < kPollIntervalMs) {
    return;
  }
  lastPollAtMs = now;

  const uint8_t buttons = display.getButtons();
  if (!haveLastButtons || buttons != lastButtons) {
    haveLastButtons = true;
    lastButtons = buttons;
    currentHexValue = buttons;
    currentDisplayMode = DisplayMode::Hex;
    if (applyCurrentDisplay()) {
      printButtonsRaw(buttons);
    } else {
      Serial.println(F("setHex failed"));
    }
  }
}

bool readCommandChar(char& command) {
  while (Serial.available()) {
    const int raw = Serial.read();
    if (raw < 0) {
      return false;
    }
    const char c = static_cast<char>(raw);
    if (c == '\r' || c == '\n' || c == ' ' || c == '\t') {
      continue;
    }
    command = c;
    return true;
  }
  return false;
}

void enterPollMode() {
  if (!pollMode) {
    Serial.println(F("poll mode on"));
  }
  pollMode = true;
  haveLastButtons = false;
  lastPollAtMs = 0;
  pollButtons(true);
}

void handleCommand(char command) {
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
    currentNumberValue = static_cast<uint16_t>(value);
    currentDisplayMode = DisplayMode::Number;
    if (applyCurrentDisplay()) {
      Serial.print(F("displayed "));
      Serial.println(currentNumberValue);
    } else {
      Serial.println(F("setNumber failed"));
    }
    return;
  }

  if (command == 'z') {
    long enabled = 0;
    (void)readIntFromSerial(enabled);
    currentLeadingZeros = (enabled != 0);
    if (applyCurrentDisplay()) {
      Serial.print(F("leading zeros "));
      Serial.println(currentLeadingZeros ? F("on") : F("off"));
    } else {
      Serial.println(F("display update failed"));
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
    if (configuredDot >= 0) {
      (void)display.setDot(static_cast<uint8_t>(configuredDot), false);
    }
    configuredDot = static_cast<int8_t>(position);
    if (display.setDot(static_cast<uint8_t>(configuredDot), true)) {
      Serial.print(F("dot set to "));
      Serial.println(configuredDot);
    } else {
      Serial.println(F("setDot failed"));
    }
    return;
  }

  if (command == 'x') {
    if (configuredDot >= 0) {
      (void)display.setDot(static_cast<uint8_t>(configuredDot), false);
      configuredDot = -1;
    }
    Serial.println(F("dot cleared"));
    return;
  }

  if (command == 'b') {
    enterPollMode();
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
    Serial.println(display.displayOn() ? F("display on")
                                       : F("displayOn failed"));
    return;
  }

  if (command == 'f') {
    Serial.println(display.displayOff() ? F("display off")
                                        : F("displayOff failed"));
    return;
  }

  Serial.print(F("unknown command: "));
  Serial.println(command);
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
  currentDisplayMode = DisplayMode::Number;
  (void)applyCurrentDisplay();
  printHelp();
}

void loop() {
  char command = '\0';
  if (readCommandChar(command)) {
    if (pollMode && command != 'b') {
      pollMode = false;
      Serial.println(F("poll mode off"));
    }
    handleCommand(command);
    return;
  }

  if (pollMode) {
    pollButtons(false);
  }
}