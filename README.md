# TCP1650

TCP1650 is an Arduino library for TM1650-based 4-digit LED display modules with optional button/key reads.

This library is intentionally small in scope:

- TM1650-style 4-digit display modules
- decimal display
- hexadecimal display
- one decimal point at a time
- display on/off control
- brightness control
- raw button reads only

The implementation is split into a host-testable device core plus a small Arduino-facing transport layer.

## Companion dependency

TCP1650 currently uses the `TCP1819` companion library for its Arduino-facing I2C transport type and helpers.

The public API uses:

- `BBI2C`
- `I2CInit(...)`

Install both libraries before building the examples.

## Current public API

class TCP1650 {
public:
  explicit TCP1650(BBI2C& i2c);

  bool begin();
  bool displayOn();
  bool displayOff();
  bool setBrightness(uint8_t level);
  bool setNumber(uint16_t value, bool leadingZeros = false);
  bool setHex(uint16_t value, bool leadingZeros = false);
  bool setDot(uint8_t position, bool on);
  uint8_t getButtons();
};

## Installation

Install these libraries side by side in your Arduino libraries directory:

- `TCP1650`
- `TCP1819`

Example layout:

- `~/Documents/Arduino/libraries/TCP1650`
- `~/Documents/Arduino/libraries/TCP1819`

## Wiring

Typical TM1650 module connections:

- VCC -> board power
- GND -> GND
- SDA -> chosen controller GPIO for the TCP1819 I2C bus
- SCL -> chosen controller GPIO for the TCP1819 I2C bus

The simple examples use `SDA` and `SCL` as the chosen pins. `HardwareSmoke` keeps its own explicit pin constants near the top of the sketch.

## Minimal example

#include <string.h>

#include <TCP1819.h>
#include <TCP1650.h>

namespace {
static const uint8_t kDisplaySdaPin = SDA;
static const uint8_t kDisplaySclPin = SCL;
static const uint32_t kDisplayI2cHz = 100000UL;

BBI2C displayBus{};
TCP1650 display(displayBus);

void setupDisplayBus() {
  memset(&displayBus, 0, sizeof(displayBus));
  displayBus.bWire = 0;
  displayBus.iSDA = kDisplaySdaPin;
  displayBus.iSCL = kDisplaySclPin;
  I2CInit(&displayBus, kDisplayI2cHz);
}
}  // namespace

void setup() {
  setupDisplayBus();

  (void)display.begin();
  (void)display.setBrightness(4);
  (void)display.setNumber(1234, false);
}

void loop() {
}

## API notes

### begin()

Initializes the device and puts it into normal display operation.

Nominal `begin()` behavior:

- initialize the transport-backed device state
- enable display output
- set default brightness/state as implemented by the device core
- write an initial visible display image

### setBrightness(level)

Accepts `level` in `0..7`.

Public meaning:

- `0` is the dimmest visible level
- `7` is the brightest visible level

### setNumber(value, leadingZeros)

Displays decimal values in `0..9999`.

### setHex(value, leadingZeros)

Displays hexadecimal values in `0x0000..0xFFFF`.

### setDot(position, on)

Controls the decimal point bit for one digit position at a time.

### getButtons()

Performs a raw button/key read and restores the display state afterward.

The returned value is intentionally raw. Any higher-level decoding stays outside this library.

## Examples

### BasicDisplay

Minimal decimal display example.

### DecimalPoint

Minimal decimal-point example.

### DisplayAndButtons

Shows a changing display value while also printing raw button bytes.

### ReadButtons

Prints raw button bytes to serial output.

### HardwareSmoke

Interactive bring-up sketch for manual hardware validation.

Supported commands are printed by the sketch at startup.

## Repository layout

- `src/` library source
- `examples/` Arduino examples
- `test/host/` host-side tests
- `docs/` design and hardware notes
- `library.properties` Arduino library metadata
- `keywords.txt` Arduino IDE keyword highlighting

## Local development

Common local commands:

make -C test/host test

## Notes

This library does not attempt to decode raw button bytes into switch positions or application-level events.

That interpretation belongs in application code, because TM1650-based modules differ in their wiring and front-panel behavior.

## License

MIT License. See `LICENSE`.