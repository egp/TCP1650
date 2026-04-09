
# TCP1650 requirements

## Functional requirements

R1. The library shall depend only on the standard Arduino core and `Wire`.

R2. The library shall provide a constructor that accepts `(SDA, SCL)` pin assignments and an optional `TwoWire&`.

R3. The library shall provide `begin()`.

R4. `begin()` shall:
- initialize the I2C transport
- select normal display operation
- enable display output
- set brightness to `7`
- write `0000`
- clear the decimal point

R5. The library shall provide `displayOn()` and `displayOff()`.

R6. The library shall provide `setBrightness(level)` for `level` in `0..7`.

R7. The library shall provide `setNumber(value, leadingZeros)` for `value` in `0..9999`.

R8. The library shall provide `setHex(value, leadingZeros)` for `value` in `0x0000..0xFFFF`.

R9. The library shall support numeric digits `0..9`.

R10. The library shall support hexadecimal digits `A..F` using conventional seven-segment glyphs that remain visually distinct from decimal digits where practical.

R11. The library shall provide `setDot(position, on)` for positions `0..3`.

R12. The decimal point bit shall be the high-order bit `0x80`.

R13. The library shall permit at most one decimal point at a time.

R14. The library shall provide `getButtons()` and return a raw byte.

R15. Normal display mode shall be 8-segment mode.

R16. `getButtons()` shall:
- switch temporarily to 7-segment/key mode
- read the raw key byte
- switch back to 8-segment mode
- refresh the cached display image

R17. The library shall cache the final segment bytes for all four digits so display restoration after a button read does not require client recomputation.

## Non-functional requirements

R18. The Arduino/Wire-specific layer shall be kept as thin as practical.

R19. Logic that does not require Arduino runtime shall be host-testable.

R20. Host-side tests shall run without third-party test frameworks.

R21. Host-side tests shall be suitable for GitHub CI.

R22. Out-of-range inputs shall fail cleanly by returning `false` from the affected API call where the API returns `bool`.