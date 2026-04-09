# TCP1650

Arduino library for the TM1650/TCP1650 4-digit LED driver with these constraints:

- no dependencies beyond standard Arduino core and `Wire`
- numeric display plus hexadecimal display
- one decimal point at a time
- raw button reads only
- normal display mode is 8-segment
- button reads temporarily switch to 7-segment/key mode, then restore 8-segment mode and the cached display

## Current public API

```cpp
TCP1650 display(SDA, SCL);

display.begin();
display.setBrightness(4);
display.setNumber(1234, false);
display.setHex(0x3A, false);
display.setDot(2, true);
const uint8_t buttons = display.getButtons();
display.displayOff();
display.displayOn();