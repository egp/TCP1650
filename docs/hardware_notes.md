# TCP1650 hardware notes

## Assumptions in the current code

The current code assumes these 7-bit addresses:

- control base: `0x24`
- display base: `0x34`
- button-read address: `0x24`

The display addresses map to four consecutive digits:
- `0x34`
- `0x35`
- `0x36`
- `0x37`

The control addresses map similarly:
- `0x24`
- `0x25`
- `0x26`
- `0x27`

## Segment encoding

The numeric digit mapping is the common 7-segment encoding with `DP` in bit `7`:

- `0` = `0x3F`
- `1` = `0x06`
- `2` = `0x5B`
- `3` = `0x4F`
- `4` = `0x66`
- `5` = `0x6D`
- `6` = `0x7D`
- `7` = `0x07`
- `8` = `0x7F`
- `9` = `0x6F`

Hex extensions use:
- `A` = `0x77`
- `b` = `0x7C`
- `C` = `0x39`
- `d` = `0x5E`
- `E` = `0x79`
- `F` = `0x71`

## Brightness encoding note

The TM1650 brightness field is quirky:

- the hardware field is `0..7`
- field value `0` means brightness level `8`
- field value `1` is the dimmest visible level

The library therefore interprets the public `setBrightness(0..7)` scale as:

- `0` = dimmest
- `7` = brightest

and encodes it as:

- `encoded = (userLevel + 1) mod 8`

## Observed rotary-switch raw values

Observed hardware results for the rotary switch positions:

- `0x44`
- `0x4C`
- `0x54`
- `0x5C`
- `0x64`
- `0x6C`

The public API remains raw-byte oriented. Any decoding of those values into switch positions should stay outside the core library unless a later requirement says otherwise.

## Important open hardware checks

1. Confirm whether the module needs a delay after switching into 7-segment/key mode before reading keys.
2. Confirm whether display restore needs any additional delay after switching back to 8-segment mode.
3. Confirm the exact behavior on Uno R3, Uno R4 Minima, and Uno R4 WiFi.

## Implementation strategy

Keep all protocol constants centralized so hardware findings can be folded in with small localized edits.