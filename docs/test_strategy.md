# TCP1650 test strategy

## 1. Host-side tests

Purpose:
- verify the host-testable core without Arduino dependencies

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
  - switch to 7-segment/key mode
  - read key byte
  - restore the prior visible/off state
  - repaint cached display only when required
- selected low-level failure paths for writes and reads

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

## 3. Board acceptance

Board acceptance is tracked separately in:
- `docs/hardware_acceptance_checklist.md`

Target boards:
- Arduino Uno R3
- Arduino Uno R4 Minima
- Arduino Uno R4 WiFi

## 4. Scope boundary

The thin Wire adapter is intentionally not the main target of host tests.

The design goal is to keep that layer small enough that most important logic remains outside it.