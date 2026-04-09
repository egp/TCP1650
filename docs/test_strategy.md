# TCP1650 test strategy

## 1. Host-side tests

Purpose:
- verify the host-testable core without Arduino dependencies

Coverage:
- digit encoding
- cached display image
- control-byte generation
- `begin()` write sequence
- `setNumber()` write sequence
- `setDot()` write sequence and one-dot policy
- `getButtons()` sequence:
  - switch to 7-segment/key mode
  - read key byte
  - switch back to 8-segment mode
  - restore cached display

Mechanism:
- fake byte transport
- logged write/read operations
- exact comparison against expected sequences

## 2. Hardware smoke tests

Purpose:
- confirm the assumptions that cannot be trusted without real hardware

Checks:
- visible numeric output
- visible decimal point
- brightness changes
- display on/off
- button reads
- display restoration after button reads

Tool:
- serial-driven operator sketch in `examples/HardwareSmoke/HardwareSmoke.ino`

## 3. Scope boundary

The thin Wire adapter is intentionally not the main target of host tests.
The design goal is to keep that layer small enough that most important logic remains outside it.