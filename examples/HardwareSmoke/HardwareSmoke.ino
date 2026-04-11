#include <Arduino.h>
#include <BitBang_I2C.h>
#include <TCP1650.h>

namespace {

enum class DisplayMode {
  Number,
  Hex,
};

static constexpr unsigned long kPollIntervalMs = 1000UL;
static constexpr uint8_t kBusSdaPin = 10;
static constexpr uint8_t kBusSclPin = 11;
static constexpr uint32_t kBusFrequencyHz = 100000UL;

BBI2C displayBus{};
TCP1650 display(displayBus);

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
  Serial.println(F(" h help"));
  Serial.println(F(" n <0-9999> display decimal number"));
  Serial.println(F(" x <0-FFFF> display hexadecimal value"));
  Serial.println(F(" z <0|1> leading zeros off/on"));
  Serial.println(F(" d <0-3> set decimal point position"));
  Serial.println(F(" c clear decimal point"));
  Serial.println(F(" b start/continue 1 Hz button poll mode"));
  Serial.println(F(" any non-b command exits poll mode"));
  Serial.println(F(" + brightness up"));
  Serial.println(F(" - brightness down"));
  Serial.println(F(" o display on"));
  Serial.println(F(" f display off"));
  Serial.println();
}

void setupDisplayBus() {
  memset(&displayBus, 0, sizeof(displayBus));
  displayBus.bWire = 0;
  displayBus.iSDA = kBusSdaPin;
  displayBus.iSCL = kBusSclPin;
  I2CInit(&displayBus, kBusFrequencyHz);
}

bool readToken(char* buffer, size_t bufferSize) {
  if (bufferSize == 0) {
    return false;
  }

  size_t used = 0;
  bool started = false;

  while (true) {
    while (!Serial.available()) {
    }

    const int raw = Serial.read();
    if (raw < 0) {
      continue;
    }

    const char c = static_cast<char>(raw);
    if (!started) {
      if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
        continue;
      }
      started = true;
    } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      break;
    }

    if (used + 1 < bufferSize) {
      buffer[used++] = c;
    }
  }

  buffer[used] = '\0';
  return used > 0;
}

bool readDecimalValue(long& value) {
  char token[16] = {};
  if (!readToken(token, sizeof(token))) {
    return false;
  }

  char* end = nullptr;
  value = strtol(token, &end, 10);
  return end != token && *end == '\0';
}

bool readHexValue(unsigned long& value) {
  char token[16] = {};
  if (!readToken(token, sizeof(token))) {
    return false;
  }

  char* end = nullptr;
  value = strtoul(token, &end, 16);
  return end != token && *end == '\0';
}

bool applyCurrentDisplay() {
  bool ok = false;

  if (currentDisplayMode == DisplayMode::Hex) {
    ok = display.setHex(currentHexValue, currentLeadingZeros);
  } else {
    ok = display.setNumber(currentNumberValue, currentLeadingZeros);
  }

  if (!ok) {
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
      Serial.println(F("display update failed"));
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
    if (!readDecimalValue(value)) {
      Serial.println(F("invalid decimal value"));
      return;
    }
    if (value < 0 || value > 9999) {
      Serial.println(F("number out of range"));
      return;
    }

    currentNumberValue = static_cast<uint16_t>(value);
    currentDisplayMode = DisplayMode::Number;
    if (applyCurrentDisplay()) {
      Serial.print(F("decimal = "));
      Serial.println(currentNumberValue);
    } else {
      Serial.println(F("display update failed"));
    }
    return;
  }

  if (command == 'x') {
    unsigned long value = 0;
    if (!readHexValue(value)) {
      Serial.println(F("invalid hex value"));
      return;
    }
    if (value > 0xFFFFUL) {
      Serial.println(F("hex value out of range"));
      return;
    }

    currentHexValue = static_cast<uint16_t>(value);
    currentDisplayMode = DisplayMode::Hex;
    if (applyCurrentDisplay()) {
      Serial.print(F("hex = 0x"));
      Serial.println(currentHexValue, HEX);
    } else {
      Serial.println(F("display update failed"));
    }
    return;
  }

  if (command == 'z') {
    long enabled = 0;
    if (!readDecimalValue(enabled)) {
      Serial.println(F("invalid leading-zero value"));
      return;
    }

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
    if (!readDecimalValue(position)) {
      Serial.println(F("invalid dot position"));
      return;
    }
    if (position < 0 || position > 3) {
      Serial.println(F("dot position out of range"));
      return;
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

  if (command == 'c') {
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
    Serial.println(display.displayOn() ? F("display on") : F("displayOn failed"));
    return;
  }

  if (command == 'f') {
    Serial.println(display.displayOff() ? F("display off") : F("displayOff failed"));
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

  setupDisplayBus();

  Serial.println(F("TCP1650 HardwareSmoke"));
  Serial.print(F("SDA pin: "));
  Serial.println(kBusSdaPin);
  Serial.print(F("SCL pin: "));
  Serial.println(kBusSclPin);

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