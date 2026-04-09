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
- no dot is lit

- [ ] Display powers up
- [ ] Display shows `0000`
- [ ] No dot is lit
- [ ] No visible flicker at idle

Pass notes:
-

Fail notes:
-

---

## 3. Brightness control

Check all public brightness values `0..7`.

Expected:
- `0` is dimmest
- `7` is brightest
- all settings are accepted
- brightness changes are visibly monotonic enough for practical use

- [ ] `setBrightness(0)` works
- [ ] `setBrightness(1)` works
- [ ] `setBrightness(2)` works
- [ ] `setBrightness(3)` works
- [ ] `setBrightness(4)` works
- [ ] `setBrightness(5)` works
- [ ] `setBrightness(6)` works
- [ ] `setBrightness(7)` works
- [ ] `0` appears dimmest
- [ ] `7` appears brightest

Pass notes:
-

Fail notes:
-

---

## 4. Decimal number display

Check representative values.

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

Check representative values.

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

Expected:
- dot bit is on one position only
- setting a new dot clears the previous dot

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

## 7. Display enable/disable

Expected:
- `displayOff()` blanks the display
- `displayOn()` restores visibility without corrupting cached content

- [ ] `displayOff()` blanks the display
- [ ] `displayOn()` restores the display
- [ ] display content after restore is correct
- [ ] dot state after restore is correct

Pass notes:
-

Fail notes:
-

---

## 8. Cached display while off

Expected:
- sending `setNumber(...)` or `setHex(...)` while the display is off updates the cache
- re-enabling the display shows the cached digits
- disabling the display does not clear the cache

- [ ] `displayOff()` then `setNumber(...)` then `displayOn()` shows the cached decimal value
- [ ] `displayOff()` then `setHex(...)` then `displayOn()` shows the cached hex value
- [ ] cached dot state is preserved across off/on
- [ ] multiple off-state updates show the most recent cached value after `displayOn()`

Pass notes:
-

Fail notes:
-

---

## 9. Button / rotary read behavior

Expected:
- raw reads are stable
- display survives temporary mode switch to 7-segment/key mode
- no objectionable flicker during reads

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
- [ ] no objectionable visible flicker during button polling

Observed values:
-

Pass notes:
-

Fail notes:
-

---

## 10. Poll mode smoke behavior

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

## 11. Board-specific notes

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