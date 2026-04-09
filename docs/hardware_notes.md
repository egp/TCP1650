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

## Important open hardware checks

1. Confirm the button-read address on the actual module.
2. Confirm whether the module needs a delay after switching into 7-segment/key mode before reading keys.
3. Confirm whether display restore needs any additional delay after switching back to 8-segment mode.
4. Confirm the exact behavior on Uno R3, Uno R4 Minima, and Uno R4 WiFi.

## Implementation strategy

Keep all protocol constants centralized so hardware findings can be folded in with small localized edits.