# TCP1650 Design Specification

## 1. Architectural overview

The planned implementation has three layers:

1. public API layer (`TCP1650`)
2. internal device/register layer
3. internal font/segment-encoding layer

The public API shall remain mid-level. Users should not need to manage individual TM1650 register writes for common operations.

## 2. Proposed source layout

- `src/TCP1650.h` / `src/TCP1650.cpp`
  - public class
  - display buffer ownership
  - API entry points
- `src/TCP1650_Device.h` / `src/TCP1650_Device.cpp`
  - TM1650 register addressing
  - low-level transport helpers
  - mode changes
  - raw key reads
- `src/TCP1650_Font.h` / `src/TCP1650_Font.cpp`
  - character-to-segment encoding tables/helpers
- `src/TCP1650_Regs.h`
  - register constants and bit masks

## 3. Public API direction

Planned operations:

- constructor taking SDA and SCL pin assignments
- `begin()`
- `clear()`
- `setBrightness(level)`
- `setDigits(...)`
- `setDecimalPosition(position)`
- `readButtonsRaw()`
- `refresh()`

The final API may add overloads, but these operations define the intended scope.

## 4. Internal state model

The public object is expected to cache at least:

- requested SDA pin
- requested SCL pin
- current brightness
- current display mode intent
- four digit segment bytes
- decimal-point position

## 5. Mode-switch design

### 5.1 Problem
TM1650 appears to require 8-segment mode for decimal points and 7-segment/key mode for button reads.

### 5.2 Planned strategy
The default runtime mode shall be display-oriented.

For raw button reads, the current design direction is:

1. switch to key-read-compatible mode
2. perform raw key read
3. switch back to display mode
4. refresh cached display state if needed

### 5.3 Risks

- visible flicker
- transient loss of decimal points
- need for settling delay between mode change and key read
- board/core specific timing behavior

These risks are explicitly deferred to hardware validation during Phase 2 and Phase 3.

## 6. Board and transport policy

The constructor shall accept SDA/SCL assignments because that is a project requirement.

However, actual enforcement of runtime pin assignment may depend on the target Arduino core. The implementation shall therefore separate:

- requested pin assignments
- actual `Wire` initialization behavior supported by the board core

Phase 1 shall define whether unsupported assignments fail explicitly or are treated as advisory on each target.

## 7. Error-handling direction

Initial implementation direction:

- use `bool` return values for operational success/failure
- keep API simple
- document unsupported cases and hardware caveats

## 8. Host-testability direction

Host-side tests shall focus on:

- segment encoding
- display-buffer transformations
- decimal-position logic
- mode-switch sequencing policy via fakes/mocks internal to the repository

Direct hardware I2C interaction is outside host-side CI scope.
