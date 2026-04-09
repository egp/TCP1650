# TCP1650

`TCP1650` is an Arduino library for TM1650-based 4-digit LED display modules with button input.

## Status

Phase 0 scaffold.

This repository currently contains:

- Arduino library layout and metadata
- planning and specification documents under `docs/`
- source skeleton under `src/`
- host-side test scaffold suitable for GitHub Actions without third-party test frameworks
- example sketch placeholders

## Goals

- No runtime dependencies beyond standard Arduino core headers and `Wire`
- Mid-level API
- Decimal-point support in 8-segment mode
- Raw button reads
- Constructor accepts SDA/SCL pin assignments
- Host-side tests that can run in CI

## Planned public API direction

```cpp
class TCP1650 {
public:
  TCP1650(uint8_t sdaPin, uint8_t sclPin, TwoWire& wire = Wire);

  bool begin();
  bool clear();
  bool setBrightness(uint8_t level);
  bool setDigits(const char digits[4]);
  bool setDigits(const uint8_t segments[4]);
  bool setDecimalPosition(int8_t position);   // -1 for none, 0..3 for digit position
  uint8_t readButtonsRaw();
  bool refresh();
};
```

The exact signatures may evolve during Phase 1, but this is the intended level of abstraction.

## Repository layout

- `docs/` — requirements, design, test strategy, hardware notes, project plan
- `src/` — library code
- `examples/` — Arduino sketches
- `test/host/` — host-side CI tests

## Host-side CI tests

The current scaffold uses only the system C++ compiler.

```bash
make -C test/host test
```

## Notes on display + buttons

TM1650 appears to force a tradeoff between:

- 8-segment mode for decimal points
- 7-segment/key mode for button scanning

The current design direction is to keep the display in 8-segment mode most of the time, switch temporarily into key-scan mode for raw button reads, then restore display mode and refresh the visible digits as needed. This behavior is called out explicitly in `docs/requirements.md` and `docs/design.md` because it is a central risk item.
