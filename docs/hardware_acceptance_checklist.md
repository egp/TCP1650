# TCP1650 hardware acceptance checklist

Use this checklist when validating the library on:

- Arduino Uno R3
- Arduino Uno R4 Minima
- Arduino Uno R4 WiFi

Record the date, board, module, and outcome for each run.

## Test metadata

- Date:
- Board:
- Module/vendor:
- Power source:
- Notes:

---

## 1. Build and upload

- [ ] Library example compiles cleanly
- [ ] `examples/HardwareSmoke/HardwareSmoke.ino` uploads cleanly
- [ ] Serial monitor opens and shows startup banner

Pass notes:
-

Fail notes:
-

---

## 2. Initialization and default state

Expected after `begin()`:
- brightness defaults to `7`
- display is enabled
- display shows `0000`

- [ ] Display powers up
- [ ] Display shows `0000`
- [ ] No visible flicker at idle

Pass notes:
-

Fail notes:
-

---

## 3. Brightness control

Expected:
- `0` is dimmest
- `7` is brightest

- [ ] `setBrightness(0)` works
- [ ] `setBrightness(7)` works
- [ ] brightness ordering looks correct

Pass notes:
-

Fail notes:
-

---

## 4. Decimal number display

- [ ] `setNumber(0, false)` displays blank blank blank `0`
- [ ] `setNumber(0, true)` displays `0000`
- [ ] `setNumber(42, false)` displays blank blank `4 2`
- [ ] `setNumber(42, true)` displays `0042`
- [ ] `setNumber(1234, false)` displays `1234`
- [ ] `setNumber(9999, false)` displays `9999`

Pass notes:
-

Fail notes:
-

---

## 5. Hex display

- [ ] `setHex(0x0000, false)` displays blank blank blank `0`
- [ ] `setHex(0x0000, true)` displays `0000`
- [ ] `setHex(0x003A, false)` displays blank blank `3 A`
- [ ] `setHex(0x03AF, true)` displays `03AF`
- [ ] `setHex(0xFFFF, true)` displays `FFFF`

Visual glyph check:
- [ ] `A` is readable
- [ ] `b` is readable
- [ ] `C` is readable
- [ ] `d` is readable
- [ ] `E` is readable
- [ ] `F` is readable

Pass notes:
-

Fail notes:
-

---

## 6. Dot behavior

- [ ] `setDot(0, true)` lights digit 0 dot
- [ ] `setDot(1, true)` lights digit 1 dot
- [ ] `setDot(2, true)` lights digit 2 dot
- [ ] `setDot(3, true)` lights digit 3 dot
- [ ] moving the dot clears the old one
- [ ] `setDot(pos, false)` clears the selected dot
- [ ] dot survives subsequent `setNumber(...)`
- [ ] dot survives subsequent `setHex(...)`

Pass notes:
-

Fail notes:
-

---

## 7. Display enable/disable and cache

- [ ] `displayOff()` blanks the display
- [ ] `displayOn()` restores the display
- [ ] off-state cache behavior works for decimal values
- [ ] off-state cache behavior works for hex values
- [ ] dot state survives off/on
- [ ] the most recent off-state update is what appears after `displayOn()`

Pass notes:
-

Fail notes:
-

---

## 8. Button / rotary read behavior

Observed rotary switch values from prior test:
- `0x44`
- `0x4C`
- `0x54`
- `0x5C`
- `0x64`
- `0x6C`

Checklist:
- [ ] `getButtons()` returns plausible nonzero values
- [ ] repeated reads are stable at a fixed switch position
- [ ] all rotary positions can be observed
- [ ] observed values match expected pattern or are documented
- [ ] display contents are restored correctly after read
- [ ] dot state is restored correctly after read
- [ ] no unacceptable visible flicker during button polling

Observed values:
-

Pass notes:
-

Fail notes:
-

---

## 9. Poll mode smoke behavior

Using `HardwareSmoke.ino`:

- [ ] poll mode enters correctly
- [ ] poll mode updates display when the switch changes
- [ ] displayed raw hex matches serial output
- [ ] exiting poll mode on another command works

Pass notes:
-

Fail notes:
-

---

## 10. Board-specific notes

### Uno R3
- [ ] compile
- [ ] upload
- [ ] runtime OK

Notes:
-

### Uno R4 Minima
- [ ] compile
- [ ] upload
- [ ] runtime OK

Notes:
-

### Uno R4 WiFi
- [ ] compile
- [ ] upload
- [ ] runtime OK

Notes:
-

---

## Final verdict

- [ ] Accepted for Uno R3
- [ ] Accepted for Uno R4 Minima
- [ ] Accepted for Uno R4 WiFi

Summary:
-