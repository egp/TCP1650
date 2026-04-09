# TCP1650 API notes

## Public class

    class TCP1650 {
    public:
      explicit TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire = Wire);

      bool begin();
      bool displayOn();
      bool displayOff();
      bool setBrightness(uint8_t level);
      bool setNumber(uint16_t value, bool leadingZeros = false);
      bool setHex(uint16_t value, bool leadingZeros = false);
      bool setDot(uint8_t position, bool on);
      uint8_t getButtons();
    };

## API choices intentionally not included

Not included:

- arbitrary character rendering beyond hex digits
- string rendering
- multiple dots at once
- debounce or event helpers
- scrolling or animation helpers
- rotary/switch decoding beyond raw-byte reads

## Constructor notes

The constructor accepts `(SDA, SCL)` and an optional `TwoWire&`.

The current Wire adapter uses the supplied pins only on platforms where runtime pin selection is supported by the core. On boards with fixed I2C pins, the values are still accepted and stored, but may be ignored by the transport layer.

## `begin()` notes

`begin()` currently means:

- bring up the transport
- set display enabled
- set brightness to `7`
- write `0000`

This keeps power-up behavior deterministic and visible.

## Numeric display notes

`setNumber()`:

- accepts `0..9999`
- supports optional leading zeros
- preserves the currently configured dot position in cache

## Hex display notes

`setHex()`:

- accepts `0x0000..0xFFFF`
- supports optional leading zeros
- preserves the currently configured dot position in cache
- uses the conventional mixed-case seven-segment hex glyphs:
  - `A`
  - `b`
  - `C`
  - `d`
  - `E`
  - `F`

## Dot notes

`setDot()` uses the display `DP` bit and enforces a one-dot policy:

- enabling a dot clears any previous dot
- clearing a dot affects only the selected position

## Button-read notes

`getButtons()` returns a raw byte.

The library keeps normal display operation in 8-segment mode, temporarily enters 7-segment/key mode to read buttons, then restores the previous display state.