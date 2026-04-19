# TCP1650 test strategy

## 1. Host-side tests

Purpose:

- verify the host-testable device core without Arduino runtime or real hardware

Coverage:

- decimal and hex glyph encoding
- cached display image
- control-byte generation
- `begin()` write sequence
- `setNumber()` write sequence
- `setHex()` write sequence
- `setDot()` write sequence and one-dot policy
- `displayOn()` / `displayOff()` power-state behavior
- off-state cache-only updates
- `getButtons()` sequence:
  - switch to key-read mode
  - read the raw key byte
  - restore the prior visible/off state
  - repaint cached display only when required
- selected low-level write/read failure paths

Mechanism:

- fake byte transport
- logged write/read operations
- exact comparison against expected sequences

## 2. Hardware smoke tests

Purpose:

- confirm the assumptions that cannot be trusted without real hardware

Checks:

- visible decimal output
- visible hexadecimal output
- visible decimal point
- brightness changes
- display on/off
- raw button reads
- display restoration after button reads
- cache preservation across display off/on
- acceptable flicker behavior

Tool:

- serial-driven operator sketch in `examples/HardwareSmoke/HardwareSmoke.ino`

## 3. Board coverage

Useful validation targets:

- Arduino Uno R3
- Arduino Uno R4 Minima
- Arduino Uno R4 WiFi

## 4. Scope boundary

The thin TCP1819-backed transport layer is intentionally not the main target of host tests.

The design goal is to keep that layer small enough that most important behavior remains outside it.