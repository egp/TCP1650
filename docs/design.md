# TCP1650 design notes

## Overview

TCP1650 is split into three layers:

1. small Arduino-facing API
2. host-testable device core
3. thin TCP1819-backed transport layer

The goal is to keep display logic and state restoration behavior outside the transport layer so most behavior can be tested without Arduino runtime or hardware.

## Layer 1. Public Arduino-facing API

File set:

- `src/TCP1650.h`
- `src/TCP1650.cpp`

Role:

- own a `TCP1650_I2CTransport`
- own a `TCP1650_Device`
- expose the small public API:
  - `begin()`
  - `displayOn()`
  - `displayOff()`
  - `setBrightness(...)`
  - `setNumber(...)`
  - `setHex(...)`
  - `setDot(...)`
  - `getButtons()`

The public constructor currently accepts a `BBI2C&` supplied by `TCP1819`.

## Layer 2. Host-testable device core

File set:

- `src/TCP1650_Device.h`
- `src/TCP1650_Device.cpp`
- `src/TCP1650_Regs.h`
- `src/TCP1650_Font.h`
- `src/TCP1650_Font.cpp`

Role:

- segment cache
- decimal and hexadecimal digit encoding
- control-byte creation
- number formatting
- one-dot policy
- power-state behavior
- button-read mode-switch sequence
- display restore after button read
- display restore after re-enabling output

This layer does not include Arduino headers.

## Layer 3. Thin TCP1819-backed transport

File set:

- `src/TCP1650_I2C.h`
- `src/TCP1650_I2C.cpp`

Role:

- send single-byte writes to TM1650 register addresses
- read the raw key byte
- keep transport details out of the device core

The design goal is to keep this layer as small as practical.

## State model

The device core keeps:

- display enabled flag
- brightness level
- cached segment bytes for digits `0..3`

The cached segment bytes are the final display image, including any active dot bit.

## Number formatting

`setNumber(value, leadingZeros)`:

- accepts `0..9999`
- emits blank leading positions when `leadingZeros == false`
- preserves the currently selected dot bit in the cached image

## Hex formatting

`setHex(value, leadingZeros)`:

- accepts `0x0000..0xFFFF`
- emits blank leading positions when `leadingZeros == false`
- preserves the currently selected dot bit in the cached image
- uses conventional seven-segment hex glyphs

## Button reads

`getButtons()` is intentionally raw-byte oriented.

The library temporarily switches mode as needed to read the key byte, then restores the prior visible or off-state display condition.

Any interpretation of raw values into switch positions or UI semantics belongs outside the library.