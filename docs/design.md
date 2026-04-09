# TCP1650 design

## Architecture

The library is split into three layers.

### 1. Public Arduino wrapper
File set:
- `src/TCP1650.h`
- `src/TCP1650.cpp`

Role:
- user-facing API
- owns the transport and low-level device core
- delegates behavior

### 2. Host-testable device core
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
- button-read mode-switch sequence
- display restore after button read
- display restore after re-enabling output

This layer does not include Arduino headers.

### 3. Thin Wire adapter
File set:
- `src/TCP1650_Wire.h`
- `src/TCP1650_Wire.cpp`

Role:
- `Wire.begin(...)`
- single-byte write
- single-byte read

## State model

The low-level core keeps:
- display enabled flag
- brightness level
- cached segment bytes for digits `0..3`

The cached segment bytes are the final display image, including the dot bit.

## Number formatting

`setNumber(value, leadingZeros)`:
- accepts `0..9999`
- emits blank leading positions when `leadingZeros == false`
- preserves any currently active dot bit on each position

## Hex formatting

`setHex(value, leadingZeros)`:
- accepts `0x0000..0xFFFF`
- emits blank leading positions when `leadingZeros == false`
- preserves any currently active dot bit on each position
- uses mixed-case `A b C d E F`

## Dot policy

- dot bit is `0x80`
- `setDot(pos, true)` clears any existing dot and sets the chosen one
- `setDot(pos, false)` clears the chosen one

## Display power policy

- `displayOff()` disables visible output but does not clear the cached display state
- `setNumber(...)`, `setHex(...)`, and `setDot(...)` continue updating the cache while display output is off
- `displayOn()` re-enables output and actively rewrites the cached display image

This gives the library a stronger restore contract and avoids relying on undocumented display-memory retention in the chip.

## Button-read policy

The device stays in 8-segment display mode during normal operation.

`getButtons()`:
1. write control bytes for temporary 7-segment/key mode
2. read raw key byte
3. write control bytes for normal 8-segment mode
4. rewrite cached display bytes

## Timing policy

The current implementation does not add fixed delays around button reads or display restore.

Observed hardware behavior so far:
- temporary flicker during rapid poll/display cycling is very brief
- the flicker is currently acceptable in the smoke test
- button reads and display restoration work correctly without delays on tested hardware

Because performance is not a priority, small delays such as `50 ms` or `100 ms` are acceptable if later hardware testing shows they are needed. They should only be added in response to observed problems.

## Error model

- public methods that change state return `bool`
- raw key reads return a `uint8_t`
- if a low-level read path fails, the current implementation returns `0` after attempting to restore display mode and display data

## Known hardware-risk item

The key-read address and any required delay between switching modes and reading keys must be confirmed on real hardware. The current implementation keeps the address and related constants isolated in `TCP1650_Regs.h` so they can be adjusted without reshaping the rest of the design.